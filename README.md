# Combat Arena

## About
UE5 C++ combat arena project built for AAA gameplay programmer portfolio.
Built from a blank project with no template code.

## Part 1 — Player Character Foundation
- Custom character built from scratch in C++
- Data driven movement via UCACharacterData asset
- Enhanced Input System wired entirely in C++
- Custom GameMode spawning correct Blueprint pawn

## Systems
- CAPlayerCharacter — custom character class
- CACharacterData — data asset for movement values
- CAGameMode — game mode with default pawn setup

## Part 2 — Custom Movement Component
- Custom UCharacterMovementComponent subclass in C++
- Sprint with smooth interpolation via FMath::FInterpTo
- Crouch using UE5 built in system
- Directional dodge roll with cooldown timer
- Input buffering system — early inputs fire when cooldown expires
- All values data driven via CACharacterData asset
- Cached owner reference for performance

 ## Systems
- CAPlayerCharacter — custom character class
- CACharacterData — data asset for all tuneable values
- CAGameMode — game mode with default pawn setup
- CACharacterMovementComponent — custom movement with sprint, crouch, dodge 

## Built With
- Unreal Engine 5.7
- C++
- Rider IDE
