#include "CAHitDetectionComponent.h"

#include "Characters/CACharacterData.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CombatArena.h"
#include "Characters/CAPlayerCharacter.h"


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
}

void UCAHitDetectionComponent::PerformTrace()
{
	if (!bIsTracing || !CharacterData) return;
		
	    //Get the skeletal mesh to read socket position
		USkeletalMeshComponent* Mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	
	if (!Mesh) return;
	
		const FVector StartPoint = Mesh->GetSocketLocation(CharacterData->WeaponSocketName);
		const FVector EndPoint = StartPoint + GetOwner()->GetActorForwardVector() * CharacterData->TraceRange;
	
		TArray<FHitResult> HitResults;
		
		const FCollisionShape Sphere = FCollisionShape::MakeSphere(CharacterData->TraceRadius);
		
		GetWorld()->SweepMultiByChannel(HitResults, StartPoint, EndPoint, FQuat::Identity,ECC_Weapon,Sphere);
	
	    // Debug - Visualize sweep in editor so we can confirm position
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
	
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || HitActor == GetOwner()) continue;
			
			if (HitActors.Contains(TWeakObjectPtr<AActor>(HitActor))) continue;
			
				HitActors.Add(TWeakObjectPtr<AActor>(HitActor));
			//Confirms which actor was hit 
			UE_LOG(LogTemp, Warning, TEXT("CAHitDetectionComponent: Hit %s"), *HitActor->GetName());
			
			// Get the target's AbilitySystemComponent — if none they cannot receive damage
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
			if (!TargetASC) continue;
			
			FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass,1,SourceASC->MakeEffectContext());
			
			if (!SpecHandle.IsValid()) continue;
			
			SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(),TargetASC);
			UE_LOG(LogTemp, Warning, TEXT("CAHitDetectionComponent: Applied damage to %s"), *HitActor->GetName());
		}
		
		
	
}

