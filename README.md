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

---

## Tech
- Unreal Engine 5.7
- C++
- Rider
