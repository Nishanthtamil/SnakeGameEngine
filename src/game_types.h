#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_W 800
#define SCREEN_H 600
#define CELL_SIZE 100
#define GRID_COLS (SCREEN_W / CELL_SIZE)
#define GRID_ROWS (SCREEN_H / CELL_SIZE)
#define TOTAL_CELLS (GRID_COLS * GRID_ROWS)
#define MAX_ENTITIES 1000
#define MAX_EVENTS 100

typedef enum EntityType{
    ENTITY_SNAKE,
    ENTITY_APPLE,
    ENTITY_WALL
}EntityType;

typedef enum EventType{
    EVENT_NONE,
    EVENT_COLLISION,
    EVENT_GAME_OVER
}EventType;

typedef struct SnakeData{
    Vector2* body;
    int count;
    int capacity;
    Vector2 direction;
    float moveTimer;
}SnakeData;

typedef struct AppleData{
    int value;
}AppleData;

typedef struct Entity{
    int id;
    Vector2 position;
    Vector2 size;
    EntityType type;
    bool active;
    void* data;
}Entity;

typedef struct Event{
    EventType type;
    Entity* sender;
    Entity* receiver;
}Event;

typedef struct GridCell{
    Entity* entries[20];
    int count;
}GridCell;

typedef struct SpatialGrid{
    GridCell cells[TOTAL_CELLS];
}SpatialGrid;

typedef struct GameState{
    Entity entities[MAX_ENTITIES];
    int entityCount;
    Event eventQueue[MAX_EVENTS];
    int eventHead;
    int eventTail;
    int pendingEvents;
    int score;
    bool gameOver;
    int currentLevel;
}GameState;

void InitSnake(SnakeData* s,Vector2 startPos);
void AppendSnake(SnakeData* s, Vector2 newPart);
void MoveSnake(SnakeData* s);
Entity* SpawnEntity(GameState* state, EntityType type, Vector2 pos, Vector2 size);
void PushEvent(GameState* state, EventType type, Entity* a, Entity* b);
void ResolveCollision(GameState* state);
void ProcessEvents(GameState* state);
void LoadLevel(GameState* state, const char* filename);

#endif