#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// --- CONSTANTS ---
#define SCREEN_W 800
#define SCREEN_H 600
#define CELL_SIZE 50        // Changed to 50 to match your Editor Grid
#define GRID_COLS (SCREEN_W / CELL_SIZE)
#define GRID_ROWS (SCREEN_H / CELL_SIZE)
#define TOTAL_CELLS (GRID_COLS * GRID_ROWS)

#define MAX_ENTITIES 1000
#define MAX_EVENTS 100

// --- ENUMS ---
typedef enum EntityType {
    ENTITY_NONE = 0,
    ENTITY_SNAKE,
    ENTITY_APPLE,
    ENTITY_WALL,
    ENTITY_ENEMY_BASIC, // New: Simple moving enemy
    ENTITY_COIN         // New: Different score item
} EntityType;

typedef enum EventType {
    EVENT_NONE,
    EVENT_COLLISION,
    EVENT_GAME_OVER
} EventType;

// --- SPECIFIC RUNTIME DATA (The "Game" side) ---
typedef struct SnakeData {
    Vector2* body;
    int count;
    int capacity;
    Vector2 direction;
    float moveTimer;
} SnakeData;

typedef struct AppleData {
    int value; // Points worth
} AppleData;

typedef struct EnemyData {
    Vector2 direction;
    float speed;
    float moveTimer;
} EnemyData;

// --- GENERIC ENTITY (The "Editor" side) ---
typedef struct Entity {
    int id;
    bool active;
    EntityType type;
    Vector2 position;
    Vector2 size;
    
    // EDITOR PROPERTIES (Generic slots for data)
    // The Editor writes to these. The Engine reads these and fills the structs above.
    int propertyValue;      // Used for: Score, Damage, ID
    int propertySubtype;    // Used for: Direction (0=Up, 1=Right...), State
    float propertySpeed;    // Used for: Movement Speed
    
    // RUNTIME DATA (Pointer to specific structs above)
    void* data; 
} Entity;

// --- THE WORLD STATE ---
typedef struct GameState {
    Entity entities[MAX_ENTITIES];
    int entityCount;
    
    // LEVEL SETTINGS (Meta)
    int levelTargetScore;
    float levelBaseSpeed;
    
    // RUNTIME STATE
    int score;
    bool gameOver;
    int currentLevel;
    
    // EVENTS
    struct Event {
        EventType type;
        Entity* sender;
        Entity* receiver;
    } eventQueue[MAX_EVENTS];
    int eventHead;
    int eventTail;
    int pendingEvents;
} GameState;

// --- PROTOTYPES ---
void InitSnake(SnakeData* s, Vector2 startPos);
void AppendSnake(SnakeData* s, Vector2 newPart);
void MoveSnake(SnakeData* s);

Entity* SpawnEntity(GameState* state, EntityType type, Vector2 pos, Vector2 size);
void PushEvent(GameState* state, EventType type, Entity* a, Entity* b);
void ResolveCollisions(GameState* state);
void ProcessEvents(GameState* state);
void CheckLevelProgression(GameState* state);
void LoadLevel(GameState* state, const char* filename);

#endif