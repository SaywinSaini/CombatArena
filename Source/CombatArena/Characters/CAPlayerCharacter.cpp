// Fill out your copyright notice in the Description page of Project Settings.


#include "CAPlayerCharacter.h"

#include "CACharacterData.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


ACAPlayerCharacter::ACAPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	//Capsule size
	GetCapsuleComponent()->InitCapsuleSize(42.f,96.f);
	
	//Don't Rotate the Character with Camera
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	
	//Character rotates in direction of movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	
	//Spring arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bDoCollisionTest = true;
	
	//Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;
}


void ACAPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Apply character data values to movement component
	
	if (CharacterData)
	{
		GetCharacterMovement()->MaxWalkSpeed = CharacterData->MovementSpeed;
		GetCharacterMovement()->JumpZVelocity = CharacterData->JumpZVelocity;
		GetCharacterMovement()->AirControl = CharacterData->AirControl;
		GetCharacterMovement()->GravityScale = CharacterData->GravityScale;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, CharacterData->RotationRate, 0.0f);
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("CAPlayerCharacter: No CharacterData assigned on %s"),*GetName());
	}
	
	//Activating input mapping context
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext,0);
		}
	}
	
}


void ACAPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACAPlayerCharacter::Move(const FInputActionValue& Value)
{
	//2D axis from input
	const FVector2D Axis = Value.Get<FVector2D>();
	
	if (Controller)
	{
		//Get camera facing direction
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

void ACAPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered,
			this, &ACAPlayerCharacter::Move);

		EIC->BindAction(LookAction, ETriggerEvent::Triggered,
			this, &ACAPlayerCharacter::Look);

		EIC->BindAction(JumpAction, ETriggerEvent::Triggered,
			this, &ACAPlayerCharacter::Jump);

		EIC->BindAction(JumpAction, ETriggerEvent::Completed,
			this, &ACAPlayerCharacter::StopJumping);
	}
}

