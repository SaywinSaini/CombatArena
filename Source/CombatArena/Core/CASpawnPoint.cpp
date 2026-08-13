#include "CASpawnPoint.h"
#include "Components/BillboardComponent.h"


ACASpawnPoint::ACASpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	SetRootComponent(Billboard);
}
