#include "CAHitDetectionComponent.h"

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


UCAHitDetectionComponent::UCAHitDetectionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCAHitDetectionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	ACAPlayerCharacter* Player = Cast<ACAPlayerCharacter>(GetOwner());
	
	if (!Player) return;
	
	CharacterData = Player->GetCharacterData();
	
	if (!CharacterData) return;
	
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
	if (!bIsTracing || !CharacterData) return;
	
		USkeletalMeshComponent* Mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	
	if (!Mesh) return;
	
		const FVector StartPoint = Mesh->GetSocketLocation(CharacterData->WeaponSocketName);
		const FVector EndPoint = StartPoint + GetOwner()->GetActorForwardVector() * CharacterData->TraceRange;
	
		TArray<FHitResult> HitResults;
		
		const FCollisionShape Sphere = FCollisionShape::MakeSphere(CharacterData->TraceRadius);
		
		GetWorld()->SweepMultiByChannel(HitResults, StartPoint, EndPoint, FQuat::Identity,ECC_Weapon,Sphere);
	
	    DrawDebugSphere(GetWorld(),StartPoint,CharacterData->TraceRadius,12,FColor::Red,false,1.0f);
	    DrawDebugSphere(GetWorld(), EndPoint, CharacterData->TraceRadius, 12, FColor::Green, false, 1.0f);
	
	    ACAPlayerCharacter* Player = Cast<ACAPlayerCharacter>(GetOwner());
	    if (!Player) return;
			
	    UAbilitySystemComponent* SourceASC = Player->GetAbilitySystemComponent();
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
			
			UE_LOG(LogTemp, Warning, TEXT("CAHitDetectionComponent: Hit %s"), *HitActor->GetName());
			
			// Get the target's AbilitySystemComponent — if none they cannot receive damage
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
			if (!TargetASC) continue;
			
			if (TargetASC->HasMatchingGameplayTag(CATags::State_Invulnerable))
			{
				UE_LOG(LogTemp, Warning, TEXT("CAHitDetectionComponent: %s is invulnerable, hit ignored"), *HitActor->GetName());
				continue;
			}
			
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
			UE_LOG(LogTemp, Warning, TEXT("CAHitDetectionComponent: Applied damage to %s"), *HitActor->GetName());
		}
		
		
	
}

