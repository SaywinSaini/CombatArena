#include "CAEnvQueryContext_Player.h"

#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void UCAEnvQueryContext_Player::ProvideContext(
	FEnvQueryInstance& QueryInstance,
	FEnvQueryContextData& ContextData) const
{
	// Get first local player — index 0 = player 1
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(
		QueryInstance.World.Get(), 0);

	if (!PlayerCharacter) return;

	// Register player as context location for EQS generator
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, PlayerCharacter);
}