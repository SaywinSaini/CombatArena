#include "CAGameMode.h"
#include "Characters/CAPlayerCharacter.h"

ACAGameMode::ACAGameMode()
{
    DefaultPawnClass = ACAPlayerCharacter::StaticClass();
}
