
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
#include "Combat/CAHitstopComponent.h"
#include "Combat/CATargetingComponent.h"
#include "Core/CAGameplayTags.h"
#include "Core/CAGameMode.h"
#include "AI/CAEnemyBase.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Combat/CAStunComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
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
	
	HitstopComponent = CreateDefaultSubobject<UCAHitstopComponent>(TEXT("HitstopComponent"));
	
	PerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuliSource"));
	PerceptionStimuliSource->bAutoRegister = true;
	
	TargetingComponent = CreateDefaultSubobject<UCATargetingComponent>(TEXT("TargetingComponent"));
	
	StunComponent = CreateDefaultSubobject<UCAStunComponent>(TEXT("StunComponent"));
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
	
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		if (!AbilitySystemComponent || !CharacterData) return;
		
		AbilitySystemComponent->SetNumericAttributeBase(UCAAttributeSet::GetMaxHealthAttribute(), CharacterData->MaxHealth);
		AbilitySystemComponent->SetNumericAttributeBase(UCAAttributeSet::GetHealthAttribute(), CharacterData->MaxHealth);
		AbilitySystemComponent->SetNumericAttributeBase(UCAAttributeSet::GetMaxStaminaAttribute(), CharacterData->MaxStamina);
		AbilitySystemComponent->SetNumericAttributeBase(UCAAttributeSet::GetStaminaAttribute(), CharacterData->MaxStamina);
		AbilitySystemComponent->SetNumericAttributeBase(UCAAttributeSet::GetMaxStunAttribute(), 100.f);
	});
	
}

void ACAPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FGenericTeamId ACAPlayerCharacter::GetGenericTeamId() const
{
	return FGenericTeamId(0);
}

bool ACAPlayerCharacter::CanJumpInternal_Implementation() const
{
	const UCACharacterMovementComponent* CMC = Cast<UCACharacterMovementComponent>(GetCharacterMovement());

	if (CMC && CMC->IsDodging()) return false;

	return Super::CanJumpInternal_Implementation();
}

void ACAPlayerCharacter::Die()
{
	if (bIsDead) return;
	bIsDead = true;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
		AbilitySystemComponent->AddLooseGameplayTag(CATags::State_Dead);
	}

	if (PerceptionStimuliSource)
	{
		PerceptionStimuliSource->UnregisterFromPerceptionSystem();
	}
	
	if (TargetingComponent && TargetingComponent->IsTargetLocked())
	{
		TargetingComponent->ToggleTargetLock();
	}
	
	if (DeathMontage)
	{
		if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
		{
			const FName Section = ResolveDeathSection();

			float Duration = 0.f;
			const int32 SectionIndex = DeathMontage->GetSectionIndex(Section);
			if (SectionIndex != INDEX_NONE)
			{
				float Start = 0.f, End = 0.f;
				DeathMontage->GetSectionStartAndEndTime(SectionIndex, Start, End);

				Anim->Montage_Play(DeathMontage, 1.f, EMontagePlayReturnType::MontageLength, Start);
				Duration = End - Start;
			}

			if (Duration > 0.f)
			{
				FTimerHandle FreezeHandle;
				GetWorldTimerManager().SetTimer(FreezeHandle, [Anim]()
				{
				   Anim->Montage_Pause();
				}, Duration - 0.05f, false);
			}
		}
	}
	// Clear the player from every enemy's Blackboard so they disengage at once.
	if (ACAGameMode* GameMode = Cast<ACAGameMode>(GetWorld()->GetAuthGameMode()))
	{
		for (const TWeakObjectPtr<ACAEnemyBase>& Enemy : GameMode->GetActiveEnemies())
		{
			if (!Enemy.IsValid()) continue;

			if (AAIController* AIC = Cast<AAIController>(Enemy->GetController()))
			{
				if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
				{
					BB->SetValueAsObject(TEXT("PlayerActor"), nullptr);
				}
				AIC->ClearFocus(EAIFocusPriority::Gameplay);
			}
		}
	}
	
}

void ACAPlayerCharacter::SetPendingDeath(AActor* Killer, FName SectionOverride)
{
	PendingKiller = Killer;
	PendingDeathSection = SectionOverride;
}

void ACAPlayerCharacter::OnTakedownImpact()
{
	if (TakedownTarget.IsValid())
	{
		TakedownTarget->Die();
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (TakedownCameraShake)
		{
			PC->ClientStartCameraShake(TakedownCameraShake);
		}
	}

	// Brief global slow-motion to punctuate the finisher.
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TakedownSlowMoScale);

	FTimerHandle SlowMoHandle;
	GetWorldTimerManager().SetTimer(SlowMoHandle, [this]()
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
	}, TakedownSlowMoDuration * TakedownSlowMoScale, false);
}

void ACAPlayerCharacter::OnTakedownWithdraw()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		const TSubclassOf<UCameraShakeBase> Shake = TakedownWithdrawShake ? TakedownWithdrawShake : TakedownCameraShake;
		if (Shake)
		{
			PC->ClientStartCameraShake(Shake);
		}
	}
}

void ACAPlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	
	if (!Controller) return;
	
	if (AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(CATags::State_Blocking))
	{
		return;
	}
	
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
	
	if (!AbilitySystemComponent) return;
	
	FGameplayTagContainer AbilityTags;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Block")));
	AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags);
}

void ACAPlayerCharacter::StopBlockAbility()
{
	if (!AbilitySystemComponent) return;

	FGameplayTagContainer AbilityTags;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Block")));
	AbilitySystemComponent->CancelAbilities(&AbilityTags);
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

FName ACAPlayerCharacter::ResolveDeathSection() const
{
	if (!PendingDeathSection.IsNone()) return PendingDeathSection;

	if (!PendingKiller) return FName("Side");

	const FVector Forward = GetActorForwardVector().GetSafeNormal2D();
	const FVector ToKiller = (PendingKiller->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();

	const float Dot = FMath::Clamp(FVector::DotProduct(Forward, ToKiller), -1.f, 1.f);
	const float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));
	
	return (Angle > 135.f) ? FName("Front") : FName("Side");
}

void ACAPlayerCharacter::TryTakedown()
{
	if (bIsDead) return;

	ACAGameMode* GameMode = Cast<ACAGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode) return;

	for (const TWeakObjectPtr<ACAEnemyBase>& Enemy : GameMode->GetActiveEnemies())
	{
		if (!Enemy.IsValid() || !Enemy->IsStaggered()) continue;

		const FVector EnemyLoc = Enemy->GetActorLocation();

		if (FVector::Dist2D(GetActorLocation(), EnemyLoc) > TakedownRange) continue;
		
		const FVector EnemyForward = Enemy->GetActorForwardVector().GetSafeNormal2D();
		const FVector ToPlayer = (GetActorLocation() - EnemyLoc).GetSafeNormal2D();

		const float Dot = FMath::Clamp(FVector::DotProduct(EnemyForward, ToPlayer), -1.f, 1.f);
		if (FMath::RadiansToDegrees(FMath::Acos(Dot)) > TakedownArcDegrees) continue;

		StartTakedown(Enemy.Get());
		return;
	}
}

void ACAPlayerCharacter::StartTakedown(ACAEnemyBase* Target)
{
	if (!Target || !CharacterData || !CharacterData->TakedownMontage) return;

	const FVector EnemyForward = Target->GetActorForwardVector().GetSafeNormal2D();

	SetActorLocation(Target->GetActorLocation() + EnemyForward * TakedownDistance, false, nullptr, ETeleportType::TeleportPhysics);
	SetActorRotation((-EnemyForward).Rotation());

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	GetCharacterMovement()->StopMovementImmediately();

	TakedownTarget = Target;
	Target->PlayTakedownVictim();

	const float Duration = PlayAnimMontage(CharacterData->TakedownMontage);

	GetWorldTimerManager().SetTimer(TakedownTimerHandle, this, &ACAPlayerCharacter::FinishTakedown, Duration, false);
}

void ACAPlayerCharacter::FinishTakedown()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		EnableInput(PC);
	}

	TakedownTarget = nullptr;
}


void ACAPlayerCharacter::EnterStagger()
{
	if (bIsDead || bIsStaggered) return;
	bIsStaggered = true;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
		AbilitySystemComponent->AddLooseGameplayTag(CATags::State_Staggered);
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	GetCharacterMovement()->StopMovementImmediately();
	
	if (CharacterData && CharacterData->StaggerEnterMontage)
	{
		if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
		{
			const float Dur = Anim->Montage_Play(CharacterData->StaggerEnterMontage);

			GetWorldTimerManager().SetTimer(StaggerPauseHandle, [Anim]()
			{
				Anim->Montage_Pause();
			}, Dur - 0.05f, false);
		}
	}

	GetWorldTimerManager().SetTimer(StaggerTimerHandle, this, &ACAPlayerCharacter::ExitStagger, StaggerDuration, false);
}

void ACAPlayerCharacter::ExitStagger()
{
	if (bIsDead) return;

	bIsStaggered = false;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(CATags::State_Staggered, 100);
	}

	if (StunComponent)
	{
		StunComponent->ResetStun();
	}

	float ExitDuration = 0.f;

	if (CharacterData && CharacterData->StaggerExitMontage)
	{
		if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
		{
			ExitDuration = Anim->Montage_Play(CharacterData->StaggerExitMontage);
		}
	}
	
	if (ExitDuration > 0.f)
	{
		GetWorldTimerManager().SetTimer(StaggerTimerHandle, [this]()
		{
			if (APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				EnableInput(PC);
			}
		}, ExitDuration, false);
	}
	else if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		EnableInput(PC);
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
		
		EIC->BindAction(TakedownAction, ETriggerEvent::Started, this, &ACAPlayerCharacter::TryTakedown);
		
		UCACharacterMovementComponent* CMC = Cast<UCACharacterMovementComponent>(GetCharacterMovement());
		if (CMC)
		{
			EIC->BindAction(SprintAction,ETriggerEvent::Triggered,CMC,&UCACharacterMovementComponent::StartSprinting);
		
			EIC->BindAction(SprintAction,ETriggerEvent::Completed,CMC,&UCACharacterMovementComponent::StopSprinting);
			
			EIC->BindAction(DodgeAction,ETriggerEvent::Started,CMC,&UCACharacterMovementComponent::Dodge);
		}
	}
}

