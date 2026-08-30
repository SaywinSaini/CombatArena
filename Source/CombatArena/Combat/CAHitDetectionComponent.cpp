#include "CAHitDetectionComponent.h"
#include "AbilitySystemInterface.h"
#include "Characters/CACharacterData.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CAHitstopComponent.h"
#include "CAStunComponent.h"
#include "CombatArena.h"
#include "AI/CAEnemyBase.h"
#include "Abilities/CAAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/CAEnemyData.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
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
			DefaultTraceSocketName = TraceSocketName;
		}
	}
	else if (ACAEnemyBase* Enemy = Cast<ACAEnemyBase>(GetOwner()))
	{
		if (UCAEnemyData* Data = Enemy->GetEnemyData())
		{
			TraceSocketName = Data->WeaponSocketName;
			TraceRange      = Data->TraceRange;
			TraceRadius     = Data->TraceRadius;
			DefaultTraceSocketName = TraceSocketName;
		}
	}
}
void UCAHitDetectionComponent::StartTrace()
{
	bIsTracing = true;
	HitActors.Empty();
	bPlayedHitSoundThisSwing = false;
}

void UCAHitDetectionComponent::StopTrace()
{
	if (SwooshSounds.Num() > 0 && !bPlayedHitSoundThisSwing)
	{
		USoundBase* Sound = SwooshSounds[FMath::RandRange(0, SwooshSounds.Num() - 1)];
		if (Sound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, Sound, GetOwner()->GetActorLocation());
		}
	}
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
			
			// Skip same-team actors so enemies cannot damage each other.
			const IGenericTeamAgentInterface* OwnerTeam = Cast<IGenericTeamAgentInterface>(GetOwner());
			const IGenericTeamAgentInterface* TargetTeam = Cast<IGenericTeamAgentInterface>(HitActor);

			if (OwnerTeam && TargetTeam &&
				OwnerTeam->GetGenericTeamId() == TargetTeam->GetGenericTeamId())
			{
				continue;
			}
			
				HitActors.Add(TWeakObjectPtr<AActor>(HitActor));
			
			// Get the target's AbilitySystemComponent — if none they cannot receive damage
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
			
			if (!TargetASC) continue;
			
			if (TargetASC->HasMatchingGameplayTag(CATags::State_Invulnerable)) continue;
			
			// Blocked hits are negated when the attacker is in the defender's front arc.
			if (TargetASC->HasMatchingGameplayTag(CATags::State_Blocking))
			{
				const FVector TargetForward = HitActor->GetActorForwardVector();
				const FVector ToAttacker = (GetOwner()->GetActorLocation() - HitActor->GetActorLocation()).GetSafeNormal();
				const float FacingDot = FVector::DotProduct(TargetForward, ToAttacker);

				if (FacingDot > 0.f)
				{
					if (UCAHitstopComponent* BlockHitstop = HitActor->FindComponentByClass<UCAHitstopComponent>())
					{
						BlockHitstop->ApplyHitstop(HitActor, true, GetOwner());
					}
					
					if (ACAPlayerCharacter* BlockingPlayer = Cast<ACAPlayerCharacter>(HitActor))
					{
						if (UCACharacterData* Data = BlockingPlayer->GetCharacterData())
						{
							if (Data->BlockFlinchMontage)
							{
								BlockingPlayer->PlayAnimMontage(Data->BlockFlinchMontage, Data->BlockFlinchPlayRate, FName("Flinch"));
							}	
								const FVector KnockbackDir = (HitActor->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal2D();
								BlockingPlayer->LaunchCharacter(KnockbackDir* Data->BlockKnockbackStrength,true, false);
							
						}
					}
					if (PC)
					{
						const TSubclassOf<UCameraShakeBase> ShakeToPlay = BlockCameraShake ? BlockCameraShake : HitCameraShake;
						if (ShakeToPlay)
						{
							PC->ClientStartCameraShake(ShakeToPlay);
						}
					}
					// Blocking costs more stun than absorbing, so guarding is
					// not a free defence.
					if (PendingBlockedStunAmount > 0.f)
					{
						if (UCAStunComponent* Stun = HitActor->FindComponentByClass<UCAStunComponent>())
						{
							Stun->AddStun(PendingBlockedStunAmount);
						}
					}
					
					if (BlockSound && !bPlayedHitSoundThisSwing)
					{
						UGameplayStatics::PlaySoundAtLocation(this, BlockSound, Hit.ImpactPoint);
					}

					continue;
				}
			}
			
			FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
			Context.AddInstigator(GetOwner(), GetOwner());

			FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1, Context);
			
			if (!SpecHandle.IsValid()) continue;
			
			if (ACAEnemyBase* DyingEnemy = Cast<ACAEnemyBase>(HitActor))
			{
				DyingEnemy->SetPendingDeath(GetOwner(), PendingDeathSection);
			}
			else if (ACAPlayerCharacter* DyingPlayer = Cast<ACAPlayerCharacter>(HitActor))
			{
				DyingPlayer->SetPendingDeath(GetOwner(), PendingDeathSection);
			}
			
			SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(),TargetASC);
			
			if (HitSounds.Num() > 0 && !bPlayedHitSoundThisSwing)
			{
				USoundBase* Sound = HitSounds[FMath::RandRange(0, HitSounds.Num() - 1)];
				if (Sound)
				{
					UGameplayStatics::PlaySoundAtLocation(this, Sound, Hit.ImpactPoint);
					bPlayedHitSoundThisSwing = true;
				}
			}
			
			if (PendingStunAmount > 0.f)
			{
				if (UCAStunComponent* Stun = HitActor->FindComponentByClass<UCAStunComponent>())
				{
					Stun->AddStun(PendingStunAmount);
				}
			}
			
			if (ACAEnemyBase* HitEnemy = Cast<ACAEnemyBase>(HitActor))
			{
				if (!HitEnemy->IsDead() && !HitEnemy->IsStaggered())
				{
					if (!HitEnemy->HasTakenFirstHit() && HitEnemy->GetEnemyData() && HitEnemy->GetEnemyData()->FirstHitReactMontage)
					{
						HitEnemy->PlayFirstHitReact(GetOwner());
					}
					else if (UCAEnemyData* Data = HitEnemy->GetEnemyData())
					{
						if (Data->HitReactMontage)
						{
							HitEnemy->PlayHitReact(Data->HitReactMontage, Data->HitReactPlayRate, PendingHitReactSection);
						}
					}
				}
			}
			
			else if (ACAPlayerCharacter* HitPlayer = Cast<ACAPlayerCharacter>(HitActor))
			{
				const bool bCanReact = !HitPlayer->IsDead() &&
					(!HitPlayer->IsStaggered() || bPendingReactWhileStaggered);

				if (bCanReact)
				{
					if (UCACharacterData* Data = HitPlayer->GetCharacterData())
					{
						UAnimMontage* ReactMontage = (bPendingReactWhileStaggered && Data->StaggerBreakReactMontage)
							? Data->StaggerBreakReactMontage
							: Data->HitReactMontage;

						if (ReactMontage)
						{
							HitPlayer->PlayHitReact(ReactMontage, Data->HitReactPlayRate, PendingHitReactSection);
						}
					}
				}
			}
			
			if (UCAHitstopComponent* EnemyHitStop = HitActor->FindComponentByClass<UCAHitstopComponent>())
			{
				EnemyHitStop->ApplyHitstop(HitActor, !bIsPlayerFrozen, GetOwner());
			}
			
			if (PC && HitCameraShake)
			{
				PC->ClientStartCameraShake(HitCameraShake);
			}
		}
		
		
	
}

void UCAHitDetectionComponent::SetTraceSocket(FName SocketName)
{
	TraceSocketName = SocketName.IsNone() ? DefaultTraceSocketName : SocketName;
}

void UCAHitDetectionComponent::ResetTraceSocket()
{
	TraceSocketName = DefaultTraceSocketName;
}

void UCAHitDetectionComponent::SetHitReactSection(FName SectionName)
{
	PendingHitReactSection = SectionName;
}

void UCAHitDetectionComponent::SetDeathSection(FName SectionName)
{
	PendingDeathSection = SectionName;
}

void UCAHitDetectionComponent::SetStunAmount(float Amount)
{
	PendingStunAmount = Amount;
}

void UCAHitDetectionComponent::SetBlockedStunAmount(float Amount)
{
	PendingBlockedStunAmount = Amount;
}

void UCAHitDetectionComponent::SetReactWhileStaggered(bool bAllow)
{
	bPendingReactWhileStaggered = bAllow;
}

