# Raylib Gameplay Screen Module

This is a C++/Raylib drawing module based on the gameplay-screen prototype. It
is intentionally separate from the game's `GameApp` and model code. It does
not process input, mutate the tree/heap, or own gameplay state.

## Files

- `VaultHeistGameplayUI.h` declares the UI state, tree nodes/edges, image
  handles, and `DrawVaultHeistGameplayUI`.
- `VaultHeistGameplayUI.cpp` draws the panels, tree and edges, event/status
  areas, palette, and reaction images.
- `assets/` contains the background and four supplied cat images.

## Intended use later

1. Add `VaultHeistGameplayUI.cpp` to the Raylib project's build.
2. Load the five PNGs with `LoadTexture` when the application starts and fill
   a `VaultHeistUiAssets` value with the returned textures.
3. Each frame, fill a `VaultHeistUiState` from the current model and UI state.
   Tree node coordinates are normalized from 0 to 1 relative to the play
   area. Edge endpoints are indexes into the node array.
4. Call `DrawVaultHeistGameplayUI(state, assets)` between `BeginDrawing()` and
   `EndDrawing()`.
5. The caller handles clicks, reset, pause, reaction timing, and model
   operations. Pass `VHG_REACTION_WRONG` for a short wrong-answer reaction,
   then `VHG_REACTION_IDLE`; pass `VHG_REACTION_NICE` briefly before
   `VHG_REACTION_COMPLETE` when a level is finished.
6. Call `UnloadTexture` for every loaded image when the game exits.

The provided images are ordinary rectangular PNGs rather than transparent
cutouts. The Raylib version displays them as small framed picture cards above
the active node.

No changes have been made to the actual game screen or gameplay flow. This
module is only a starting point to integrate when requested.
