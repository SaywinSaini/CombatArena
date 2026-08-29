
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CAHitstopComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMBATARENA_API UCAHitstopComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	

	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	float HitstopDuration = 0.1f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	float HitstopTimeDilation = 0.05;
	
	FTimerHandle HitstopTimerHandle;
	
	
public:
	
	UCAHitstopComponent();
	
	void ApplyHitstop(AActor* HitActor, bool bFreezePlayer,AActor* PlayerActor);
	
};
