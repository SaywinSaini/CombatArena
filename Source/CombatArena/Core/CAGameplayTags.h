#pragma once

#include "NativeGameplayTags.h"

// Central declaration of all native gameplay tags used by the game.
// Declaring them here (instead of raw strings) makes every reference
// compile-checked — a typo becomes a build error, not a silent runtime bug.
namespace CATags
{
	// Combat state tags — describe what a character is currently doing.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Attacking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Comboing);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Blocking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Staggered);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Invulnerable);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead);
}