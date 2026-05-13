#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CAHitDetectionComponent.generated.h"


class UCACharacterData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMBATARENA_API UCAHitDetectionComponent : public UActorComponent
{
	GENERATED_BODY()
	
protected:
	
	virtual void BeginPlay() override;
	
public:
	
	UCAHitDetectionComponent();
	
	void StartTrace();
	
	void StopTrace();
	
	void PerformTrace();
	
	private:
	// Tracks actors already hit this swing to prevent multiple hits per attack
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> HitActors;
	
	// Tracks actors already hit this swing,weak refs so dead actors GC naturally
	UPROPERTY()
	TObjectPtr<UCACharacterData> CharacterData;
	
	bool bIsTracing = false;
	
};
