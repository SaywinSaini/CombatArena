#include "CAHitDetectionComponent.h"
#include "AbilitySystemInterface.h"
#include "Characters/CACharacterData.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CAHitstopComponent.h"
#include "CombatArena.h"
#include "AI/CAEnemyBase.h"
#include "Characters/CAEnemyData.h"
#include "GameFramework/PlayerController.h"
#include "Characters/CAPlayerCharacter.h"
#include "Core/CAGameplayTags.h"

TAutoConsoleVariable<bool> CVarHitDetectionDebugDrawing(TEXT("game.HitDetection.DebugDraw"), false,TEXT("Enable HitDetection component debug rendering.(0 = off , 1 = enabled"),ECVF_Cheat);

UCAHitDetectionComponent::UCAHitDetectionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCAHitDetectionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (ACAPlayerCharacter* Player = Cast<ACAPlayerCharacter>(GetOwner()))
	{
		if (UCACharacterData* Data = Player->GetCharacterData())
		{
			TraceSocketName = Data->WeaponSocketName;
			TraceRange = Data->TraceRange;
			TraceRadius = Data->TraceRadius;
		}
	}
	else if (ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(GetOwner()))
	{
		if (UCAEnemyData* Data = Enemy->GetEnemyData())
		{
			TraceSocketName = Data->WeaponSocketName;
			TraceRange      = Data->TraceRange;
			TraceRadius     = Data->TraceRadius;
		}
	}
}
void UCAHitDetectionComponent::StartTrace()
{
	bIsTracing = true;
	HitActors.Empty();
}

void UCAHitDetectionComponent::StopTrace()
{
	bIsTracing = false;
	bIsPlayerFrozen = false;
}

void UCAHitDetectionComponent::PerformTrace()
{
	if (!bIsTracing || TraceSocketName.IsNone()) return;
	
		USkeletalMeshComponent* Mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	
	if (!Mesh) return;
	
		const FVector StartPoint = Mesh->GetSocketLocation(TraceSocketName);
		const FVector EndPoint = StartPoint + GetOwner()->GetActorForwardVector() * TraceRange;
	
		TArray<FHitResult> HitResults;
		
		const FCollisionShape Sphere = FCollisionShape::MakeSphere(TraceRadius);
		
		GetWorld()->SweepMultiByChannel(HitResults, StartPoint, EndPoint, FQuat::Identity,ECC_Weapon,Sphere);
	
	    bool bEnabledDebugDraw = CVarHitDetectionDebugDrawing.GetValueOnGameThread();
	
	if (bEnabledDebugDraw)
	{
		DrawDebugSphere(GetWorld(),StartPoint,TraceRadius,12,FColor::Red,false,1.0f);
		DrawDebugSphere(GetWorld(), EndPoint, TraceRadius, 12, FColor::Green, false, 1.0f);
	}
		IAbilitySystemInterface* OwnerASI = Cast<IAbilitySystemInterface>(GetOwner());
		if (!OwnerASI) return;
		
	    UAbilitySystemComponent* SourceASC = OwnerASI->GetAbilitySystemComponent();
	    if (!SourceASC) return;
	
	    if (!DamageEffectClass)
	    {
		    UE_LOG(LogTemp, Error, TEXT("DamageEffectClass is NULL")); return;
	    }    
	
	    APlayerController* PC = GetWorld()->GetFirstPlayerController();
	    
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || HitActor == GetOwner()) continue;
			
			if (HitActors.Contains(TWeakObjectPtr<AActor>(HitActor))) continue;
			
				HitActors.Add(TWeakObjectPtr<AActor>(HitActor));
			
			// Get the target's AbilitySystemComponent — if none they cannot receive damage
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
			if (!TargetASC) continue;
			
			if (TargetASC->HasMatchingGameplayTag(CATags::State_Invulnerable)) continue;
			
			FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass,1,SourceASC->MakeEffectContext());
			
			if (!SpecHandle.IsValid()) continue;
			
			SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(),TargetASC);
			
			if (ACAEnemyBase* HitEnemy = Cast<ACAEnemyBase>(HitActor))
			{
				if (UCAEnemyData* Data = HitEnemy->GetEnemyData())
				{
					if (Data->HitReactMontage)
					{
						if (UAnimInstance* Anim = HitEnemy->GetMesh()->GetAnimInstance())
						{
							HitEnemy->PlayHitReact(Data->HitReactMontage, Data->HitReactPlayRate);
						}
					}
				}
			}
			
			UCAHitstopComponent* EnemyHitStop = HitActor->FindComponentByClass<UCAHitstopComponent>();
			
			if (EnemyHitStop)
			{   
				// Prevent multiple hitstop applications from freezing the player more than once
				// during a single trace window.
				EnemyHitStop->ApplyHitstop(HitActor,!bIsPlayerFrozen,GetOwner());
				bIsPlayerFrozen = true;
			}
			
			if (PC && HitCameraShake)
			{  
				PC->ClientStartCameraShake(HitCameraShake);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Camera shake failed — PC: %d | Shake: %d"), PC != nullptr, HitCameraShake != nullptr);
			}
		}
		
		
	
}

