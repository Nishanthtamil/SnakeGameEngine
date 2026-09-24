# 🐍 Snake Engine Pro

<div align="center">
  <img src="https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white" alt="C" />
  <img src="https://img.shields.io/badge/raylib-FFFFFF?style=for-the-badge&logo=raylib&logoColor=black" alt="Raylib" />
  <img src="https://img.shields.io/badge/Game_Engine-2B2D31?style=for-the-badge&logo=unrealengine&logoColor=white" alt="Game Engine" />
</div>

<br/>

Snake Engine Pro started as a simple classic Snake game but rapidly evolved into a fully functional, data-driven **2D Game Engine** built entirely in **C** using **Raylib**.

The engine now supports generic entity management, an integrated level editor, custom `.eng` map files, and hot-reloading capabilities.

---

## ✨ Features

- **🎮 Data-Driven Architecture**: Levels and entities are stored in custom `.eng` text files, allowing you to design and load levels dynamically without recompiling.
- **🏗️ Integrated Level Editor**: Comes with a built-in Unity-style visual GUI editor using `raygui`. You can place walls, spawns, and items on a snap-to-grid system and save them directly to the active level file.
- **⚡ Hot Reloading**: Tweak your `.eng` level file in the editor (or a text editor) and press **F5** in-game to instantly hot-reload the level without restarting the engine!
- **🧩 Entity System**: A flexible entity struct system handling Snakes, Apples, Coins, Walls, and Basic AI Enemies with built-in collision resolution and state management.
- **🎨 Cyberpunk Aesthetic**: Built-in rendering pipeline with a cool cyber-grid background and colorful primitive graphics.

## 🛠️ Technology Stack

| Technology | Purpose |
| ---------- | ------- |
| **C99** | Core engine logic and memory management |
| **Raylib** | Hardware-accelerated 2D graphics, window management, input handling |
| **Raygui** | Immediate-mode GUI for the integrated Level Editor |

## 🚀 Getting Started

### Prerequisites
Make sure you have **C compiler (GCC/Clang)** and **Raylib** installed on your system.

### Compilation
You can compile the Game and the Editor separately. Make sure to link `raylib` and the math library `m`.

**Compile the Game:**
```bash
gcc -o build/game src/main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

**Compile the Editor:**
```bash
gcc -o build/editor src/editor.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

### Running the Engine
1. Run the Editor to create a level:
```bash
./build/editor
```
2. Run the Game to play it:
```bash
./build/game
```

## 🏗️ Project Structure

```text
SnakeGameEngine/
├── src/
│   ├── main.c           # Game loop, rendering, and hot-reloading
│   ├── engine.c         # Physics, collisions, event processing
│   ├── editor.c         # Visual GUI level editor
│   ├── game_types.h     # Core structs, constants, and entity definitions
│   └── raygui.h         # Immediate mode GUI library
├── assets/
│   └── level1.eng       # Data-driven level file containing entity coordinates
└── build/               # Compiled binaries
```

## 🎮 How to Use the Editor
- **Paint Mode**: Select an entity from the right sidebar and click on the grid to place it.
- **Erase Mode**: Switch to Erase mode and click on placed entities to remove them.
- **Save Level**: Click the big "SAVE LEVEL" button to export the current grid to `assets/level1.eng`.

## 🕹️ Controls (Game)
- **Arrow Keys**: Move the player entity.
- **F5**: Hot-reload the current level file.
- **Enter**: Reboot system / Restart after game over.

## 🤝 Contributing
Contributions are always welcome! Feel free to open a pull request or issue to add more entity types, better physics, or enhanced editor tools.
