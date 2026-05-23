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

### AI Enemy System
Three enemy types built on a shared C++ base class. Each enemy runs a single
Behavior Tree driven by a UAIPerceptionComponent for sight detection. All
tuneable values live in a CAEnemyData DataAsset — movement speed, health,
attack range and sight radius are configured per enemy type without touching
code.

Enemies transition between three states — patrol, chase and attack — driven
entirely by Blackboard key updates. When the player enters the sight cone,
OnTargetPerceptionUpdated writes to the Blackboard and the BT switches states
immediately via Observer Abort decorators.

Patrol uses a custom BTTask that queries UNavigationSystemV1 for a random
reachable point each cycle. Attack is gated by a custom BTDecorator that
evaluates distance to the player each tick. An EQS query with a custom
C++ context class generates candidate positions around the player for
tactical positioning.

Enemy health is initialized from the DataAsset via GAS SetNumericAttributeBase
— damage from the player's hit detection applies correctly through the shared
IAbilitySystemInterface implementation.

---

## Tech
- Unreal Engine 5.7
- C++
- Rider
