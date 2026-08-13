#include "CAGameplayTags.h"

namespace CATags
{
	// The string on the right is the actual tag path. Declared once here;
	// referenced by symbol everywhere else.
	UE_DEFINE_GAMEPLAY_TAG(State_Attacking, "State.Attacking");
	UE_DEFINE_GAMEPLAY_TAG(State_Comboing,  "State.Comboing");
	UE_DEFINE_GAMEPLAY_TAG(State_Blocking,  "State.Blocking");
	UE_DEFINE_GAMEPLAY_TAG(State_Staggered, "State.Staggered");
	UE_DEFINE_GAMEPLAY_TAG(State_Invulnerable, "State.Invulnerable");
	UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead");
}