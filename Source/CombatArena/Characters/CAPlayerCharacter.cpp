// Fill out your copyright notice in the Description page of Project Settings.


#include "CAPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "CACharacterData.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include  "CACharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "Abilities/CAAttributeSet.h"
#include "Abilities/CABlockAbility.h"
#include "Abilities/CAMeleeAbility.h"
#include "Abilities/CAProjectileAbility.h"
#include "Combat/CAHitDetectionComponent.h"
#include "Combat/CATargetingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"


ACAPlayerCharacter::ACAPlayerCharacter(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer.SetDefaultSubobjectClass<UCACharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	
	
	// Configure default locomotion settings
	GetCapsuleComponent()->InitCapsuleSize(42.f,96.f);
	
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	
	// Third-person camera setup
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bDoCollisionTest = true;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;
	
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	
	CreateDefaultSubobject<UCAAttributeSet>(TEXT("AttributeSet"));
	
	HitDetectionComponent = CreateDefaultSubobject<UCAHitDetectionComponent>(TEXT("HitDetectionComponent"));
	
	PerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuliSource"));
	PerceptionStimuliSource->bAutoRegister = true;
	
	TargetingComponent = CreateDefaultSubobject<UCATargetingComponent>(TEXT("TargetingComponent"));
}


void ACAPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    //Apply character data values to movement component
    if (CharacterData)
    {
        GetCharacterMovement()->MaxWalkSpeed  = CharacterData->MovementSpeed;
        GetCharacterMovement()->JumpZVelocity = CharacterData->JumpZVelocity;
        GetCharacterMovement()->AirControl    = CharacterData->AirControl;
        GetCharacterMovement()->GravityScale  = CharacterData->GravityScale;
        GetCharacterMovement()->RotationRate  = FRotator(0.0f, CharacterData->RotationRate, 0.0f);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("CAPlayerCharacter: No CharacterData assigned on %s"), *GetName());
    }
    
	// Register the default Enhanced Input mapping context
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(PlayerMappingContext, 0);
        }
    }
    
	// Initialize GAS owner and avatar actor information
    AbilitySystemComponent->InitAbilityActorInfo(this, this);
    
	// Grant startup abilities and cache the melee ability handle
    for (TSubclassOf<UGameplayAbility> Ability : DefaultAbilities)
    {
        if (Ability)
        {
            FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability));
            
            if (Ability->IsChildOf(UCAMeleeAbility::StaticClass()))
            {
                MeleeAbilityHandle = Handle;
            }
        }
    }

	// Track melee ability activation state for input handling
	AbilitySystemComponent->AbilityActivatedCallbacks.AddUObject(this, &ACAPlayerCharacter::OnAbilityActivated);
	AbilitySystemComponent->AbilityEndedCallbacks.AddUObject(this, &ACAPlayerCharacter::OnAbilityEnded);
    
    PerceptionStimuliSource->RegisterForSense(TSubclassOf<UAISense>(UAISense_Sight::StaticClass()));
    PerceptionStimuliSource->RegisterWithPerceptionSystem();
}

void ACAPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACAPlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	
	if (!Controller) return;
	
	if (TargetingComponent && TargetingComponent->IsTargetLocked())
	{
		AActor* Target = TargetingComponent->GetLockedTarget();
		if (Target)
		{
			// Move relative to the locked target instead of camera direction
			FVector ToEnemy = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			ToEnemy.Z = 0.0f;
			
			// Generate strafing direction around the target
			FVector RightDir = FVector::CrossProduct(FVector::UpVector,ToEnemy).GetSafeNormal();
			
			AddMovementInput(ToEnemy,Axis.Y);
			AddMovementInput(RightDir,Axis.X);
		}
	}
	
	else
	{
		// Move relative to camera yaw when target lock is inactive
		const FRotator Rotation = Controller->GetControlRotation();
		
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		
		const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		 
		AddMovementInput(ForwardDir,Axis.Y);
		AddMovementInput(RightDir,Axis.X);
	}
}

void ACAPlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void ACAPlayerCharacter::StartCrouch()
{
	Crouch();
}

void ACAPlayerCharacter::StopCrouch()
{
	UnCrouch();
}

// Forward input to the active melee ability when one is running;
// otherwise activate the granted melee ability.
void ACAPlayerCharacter::ActivateMeleeAbility()
{
	if (ActiveMeleeAbility)
	{
		ActiveMeleeAbility->SetComboInputReceived();
	}
	else
	{
		AbilitySystemComponent->TryActivateAbility(MeleeAbilityHandle);
	}
}

void ACAPlayerCharacter::ActivateRangedAbility()
{
	FGameplayTagContainer AbilityTags;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Ranged")));
	AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags);
}

void ACAPlayerCharacter::StartBlockAbility()
{
	FGameplayTagContainer AbilityTags;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Block")));
	AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags);
}

void ACAPlayerCharacter::StopBlockAbility()
{
	// nothing
}

void ACAPlayerCharacter::ToggleTargetLock()
{
	if (TargetingComponent)
	{
		TargetingComponent->ToggleTargetLock();
	}
}

void ACAPlayerCharacter::OnAbilityActivated(UGameplayAbility* Ability)
{
	// Store the active melee ability so subsequent attack presses can advance combos.
	if (UCAMeleeAbility* Melee = Cast<UCAMeleeAbility>(Ability))
	{
		ActiveMeleeAbility = Melee;
	}
}

void ACAPlayerCharacter::OnAbilityEnded(UGameplayAbility* Ability)
{
	// Stop forwarding combo inputs once the melee ability has ended.
	if (Cast<UCAMeleeAbility>(Ability))
	{
		ActiveMeleeAbility = nullptr;
	}
}

void ACAPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered,this, &ACAPlayerCharacter::Move);

		EIC->BindAction(LookAction, ETriggerEvent::Triggered,this, &ACAPlayerCharacter::Look);

		EIC->BindAction(JumpAction, ETriggerEvent::Triggered,this, &ACAPlayerCharacter::Jump);

		EIC->BindAction(JumpAction, ETriggerEvent::Completed,this, &ACAPlayerCharacter::StopJumping);
		
		EIC->BindAction(CrouchAction,ETriggerEvent::Triggered,this,&ACAPlayerCharacter::StartCrouch);
		
		EIC->BindAction(CrouchAction,ETriggerEvent::Completed,this,&ACAPlayerCharacter::StopCrouch);
		
		EIC->BindAction(MeleeAction,ETriggerEvent::Started,this,&ACAPlayerCharacter::ActivateMeleeAbility);
		
		EIC->BindAction(RangedAction,ETriggerEvent::Started,this,&ACAPlayerCharacter::ActivateRangedAbility);
		
		EIC->BindAction(BlockAction,ETriggerEvent::Started,this,&ACAPlayerCharacter::StartBlockAbility);
		
		EIC->BindAction(BlockAction,ETriggerEvent::Completed,this,&ACAPlayerCharacter::StopBlockAbility);
		
		EIC->BindAction(TargetLockAction,ETriggerEvent::Started,this,&ACAPlayerCharacter::ToggleTargetLock);
		
		UCACharacterMovementComponent* CMC = Cast<UCACharacterMovementComponent>(GetCharacterMovement());
		if (CMC)
		{
			EIC->BindAction(SprintAction,ETriggerEvent::Triggered,CMC,&UCACharacterMovementComponent::StartSprinting);
		
			EIC->BindAction(SprintAction,ETriggerEvent::Completed,CMC,&UCACharacterMovementComponent::StopSprinting);
			
			EIC->BindAction(DodgeAction,ETriggerEvent::Triggered,CMC,&UCACharacterMovementComponent::Dodge);
		}
	}
}

