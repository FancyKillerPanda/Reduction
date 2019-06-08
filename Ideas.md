# Reduction

### Gameplay
- 2 player game (WASD/Arrow Keys)
- Each player controls a ship that can shoot
    - Each shot on target will have a small knockback
- Objective is to try to destroy the other player (will take multiple hits)
- The outside wall will gradually move in
    - Life is drained the further out of the main area the player is
- Obstacles will stop you from moving and will block shots
- Powerups can be purchased in exchange for life
    - Speed boost
    - Accuracy boost
    - Shot cooldown time reduction
    - Damage boost
    - Knockback boost
- Win counter like 1-0
    - First to 3 (short game), 5 (medium game), or 7 (long game)

### TODO
- [x] Set up small logging system
- [x] Link SDL with Visual Studio
- [x] Open window using SDL
- [x] Create player texture
- [x] Create player in game to control
- [x] Make player able to shoot
- [x] Cooldown timer for bullets
- [x] Spawn 2 players that can shoot each other
- [x] Create circle moving inwards from edges
- [x] Add life to players
- [x] Add flames of different lengths for player at different speeds
- [x] Drain life when player is outside the combat area
- [x] Make life drain faster when further out
- [x] Add start screen
- [x] Add powerup purchasing on start screen
- [x] Make powerups affect gameplay
- [x] Show life left at top of screen
- [x] Implement knockback on each hit
- [x] Add a win counter
- [x] Implement choosing between short/medium/long games
- [x] Add game over screen
