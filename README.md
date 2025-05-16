
# Super Mario Bros 3 Clone - SE102 Introduction to Game Programming

A course project that recreates the classic levels 1-1 and 1-4 from Super Mario Bros 3 using game programming techniques learned throughout the semester.

## Project Overview

This project aims to implement a functional clone of the first two levels (1-1 and 1-4) from the classic Nintendo game Super Mario Bros 3, demonstrating core game development concepts including sprite animation, collision detection, physics simulation, and game state management.

### Key Features

- Accurate recreation of SMB3 levels 1-1 and 1-4
- Player character mechanics (movement, power-ups)
- Enemy patrolling and interactions
- Collision detection and physics
- Head-Up Display

## Technologies Used

- C++ programming language
- DirectX11 for rendering
- JSON for level data storage

## Installation & Setup

1. Clone the repository:
   ```bash
   git clone https://github.com/zAcherttp/SE102-SuperMarioBros3
   ```

2. Ensure you have the required dependencies:
   - C++ compiler (Visual Studio 2019+ recommended)
   - DirectX11TK

3. Open the project in your IDE and build the solution

4. Run the compiled executable

## Keybindings

| Key | Controller function |
| ------------- | ------------- |
| <kbd>W</kbd>, <kbd>↑</kbd> | D-pad up |
| <kbd>A</kbd>, <kbd>←</kbd> | D-pad left |
| <kbd>S</kbd>, <kbd>↓</kbd> | D-pad down |
| <kbd>D</kbd>, <kbd>→</kbd> | D-pad right |
| <kbd>J</kbd>, <kbd>C</kbd> | B button |
| <kbd>K</kbd>, <kbd>V</kbd> | A button |
| <kbd>I</kbd>, <kbd>F</kbd> | SELECT button |

## Project Structure

```
/SE102-SuperMarioBros3
├── Resources
│   ├── Assets
│   └── Texture
└── Source Files
    ├── Core
    ├── Debug
    ├── Entity
    ├── Graphics
    ├── Lib
    └── Physics
```

## Implementation Details

### Sprite System
The game uses sprite sheets for all game objects, with frame-by-frame animation handled through a custom animation system.

### Collision System
Implements AABB (Axis-Aligned Bounding Box) continuous collision detection.

### Level Design
Levels are stored in .json files with a entity-based design. The level loader parses these files to create the game world.

## Development Progress

- [x] Basic engine structure
- [x] Player movement and physics
- [x] Power-up system
- [x] Enemy AI implementation
- [ ] Level 1-1 implementation
- [ ] Level 1-4 implementation
- [ ] Final polishing and bug fixes

## Credits

- Original Super Mario Bros 3 by Nintendo
- Course instructor: [Thầy Dũng](https://github.com/dungdna2000)
- Assets used for educational purposes only.