#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CAAnimNotify_TakedownImpact.generated.h"

/**
 * Fired on the takedown's impact frame so the victim dies in time with the
 * animation rather than when the montage ends.
 */
UCLASS()
class COMBATARENA_API UCAAnimNotify_TakedownImpact : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
	UPROPERTY(EditAnywhere, Category = "Takedown")
	bool bIsWithdrawal = false;
};