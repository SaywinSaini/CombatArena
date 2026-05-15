# Combat Arena

A third person action combat game built in UE5 from a blank C++ project. 
No templates, no shortcuts — everything written from scratch.

Still in active development. Adding systems as I go.

---

## What's been built so far

### Player Character
Custom character class with a data-driven movement system. All values like 
walk speed, jump height and gravity live in a data asset so they can be tuned 
without touching code.

Enhanced Input System wired entirely in C++. GameMode spawns the correct 
Blueprint pawn at runtime using ConstructorHelpers.

### Movement
Subclassed UCharacterMovementComponent to add sprint, crouch and dodge roll.

Sprint smoothly interpolates to max speed using FInterpTo rather than snapping 
instantly. Dodge is directional — rolls toward whatever direction you are 
pressing, or backward if standing still. Input buffering lets you queue a dodge 
slightly early and it fires the moment the cooldown expires.

Owner reference is cached in BeginPlay rather than cast every frame.

### Combat — Gameplay Ability System
Full GAS implementation from scratch. AbilitySystemComponent lives on the
player character with a custom AttributeSet tracking Health, MaxHealth,
Stamina and MaxStamina. Attributes are clamped in PostGameplayEffectExecute
so they never go negative or exceed their maximum.

Three abilities implemented in C++:
- Melee attack — plays an animation montage on activation
- Ranged projectile — spawns a projectile actor in the character forward direction
- Defensive block — stays active while button held, ends on release

Abilities are granted via a designer-friendly array in Blueprint and activated
using GameplayTags. All ability values driven by a DataTable asset.

### Hit Detection
Melee hit detection built on a dedicated component rather than overlap events.
CAHitDetectionComponent performs a sphere sweep from the weapon hand socket
driven by an AnimNotify — damage windows are tied directly to animation timing.

CAMeleeAbility uses AbilityTask_PlayMontageAndWait to keep the ability alive
across frames while the montage plays. A TWeakObjectPtr TSet tracks hit actors
per swing to prevent multi-hit.

Damage is applied through GAS — GE_Damage is built as a spec from the source
ASC and applied to the target ASC on valid hit. A custom ECC_Weapon collision
channel controls which actors respond to weapon traces.

All trace values live in CACharacterData so designers can tune without touching
code.
---

## Tech
- Unreal Engine 5.7
- C++
- Rider
