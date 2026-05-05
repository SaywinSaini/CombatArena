#include "CAGameMode.h"
#include "Characters/CAPlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACAGameMode::ACAGameMode()
{
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClass(
           TEXT("/Game/CombatArena/Characters/Player/BP_PlayerCharacter"));
       
       if (PlayerPawnClass.Class)
       {
           DefaultPawnClass = PlayerPawnClass.Class;
       }
}
