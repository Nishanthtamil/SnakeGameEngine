#include "game_types.h"
#include <string.h> 

// --- SNAKE LOGIC ---
void InitSnake(SnakeData* s, Vector2 startPos) {
    s->count = 1;
    s->capacity = 10;
    s->body = (Vector2*)malloc(s->capacity * sizeof(Vector2));
    if (s->body) s->body[0] = startPos;
    s->direction = (Vector2){1, 0};
    s->moveTimer = 0.0f;
}

void AppendSnake(SnakeData* s, Vector2 newPart) {
    if (s->count >= s->capacity) {
        s->capacity *= 2;
        s->body = (Vector2*)realloc(s->body, s->capacity * sizeof(Vector2));
    }
    s->body[s->count] = newPart;
    s->count++;
}

void MoveSnake(SnakeData* s) {
    for (int i = s->count - 1; i > 0; i--) {
        s->body[i] = s->body[i-1];
    }
    s->body[0].x += s->direction.x * CELL_SIZE;
    s->body[0].y += s->direction.y * CELL_SIZE;

    // Boundary Clamp
    if (s->body[0].x < 0) s->body[0].x = 0;
    if (s->body[0].y < 0) s->body[0].y = 0;
    if (s->body[0].x >= SCREEN_W) s->body[0].x = SCREEN_W - CELL_SIZE;
    if (s->body[0].y >= SCREEN_H) s->body[0].y = SCREEN_H - CELL_SIZE;
}

// --- ENTITY SYSTEM ---
Entity* SpawnEntity(GameState* state, EntityType type, Vector2 pos, Vector2 size) {
    if (state->entityCount >= MAX_ENTITIES) return NULL;
    
    Entity* e = &state->entities[state->entityCount++];
    e->id = state->entityCount;
    e->active = true;
    e->type = type;
    e->position = pos;
    e->size = size;
    e->data = NULL;
    return e;
}

void PushEvent(GameState* state, EventType type, Entity* a, Entity* b) {
    if (state->pendingEvents >= MAX_EVENTS) return;
    
    struct Event* e = &state->eventQueue[state->eventTail];
    e->type = type;
    e->sender = a;
    e->receiver = b;
    
    state->eventTail = (state->eventTail + 1) % MAX_EVENTS;
    state->pendingEvents++;
}

// --- PHYSICS ---
void ResolveCollisions(GameState* state) {
    // Basic N^2 check is fine for < 100 entities
    for (int i = 0; i < state->entityCount; i++) {
        Entity* e1 = &state->entities[i];
        if (!e1->active) continue;

        for (int j = i + 1; j < state->entityCount; j++) {
            Entity* e2 = &state->entities[j];
            if (!e2->active) continue;

            // AABB
            bool hit = (e1->position.x < e2->position.x + e2->size.x &&
                        e1->position.x + e1->size.x > e2->position.x &&
                        e1->position.y < e2->position.y + e2->size.y &&
                        e1->position.y + e1->size.y > e2->position.y);
            
            if (hit) PushEvent(state, EVENT_COLLISION, e1, e2);
        }
    }
}

// --- LOGIC ---
void ProcessEvents(GameState* state) {
    while (state->pendingEvents > 0) {
        struct Event e = state->eventQueue[state->eventHead];
        state->eventHead = (state->eventHead + 1) % MAX_EVENTS;
        state->pendingEvents--;

        if (e.type == EVENT_COLLISION) {
            Entity* snake = (e.sender->type == ENTITY_SNAKE) ? e.sender : e.receiver;
            Entity* other = (e.sender->type == ENTITY_SNAKE) ? e.receiver : e.sender;

            // Snake Logic
            if (snake->type == ENTITY_SNAKE) {
                
                // Eat Apple or Coin
                if (other->type == ENTITY_APPLE || other->type == ENTITY_COIN) {
                    SnakeData* sData = (SnakeData*)snake->data;
                    int points = 10;
                    
                    // Retrieve specific data
                    if (other->data) {
                        AppleData* aData = (AppleData*)other->data;
                        points = aData->value;
                    }
                    
                    // Grow Snake
                    if (sData->count > 0) {
                        Vector2 tailPos = sData->body[sData->count-1];
                        AppendSnake(sData, tailPos); 
                    }
                    
                    other->active = false; 
                    state->score += points;
                }
                // Hit Wall or Enemy
                else if (other->type == ENTITY_WALL || other->type == ENTITY_ENEMY_BASIC) {
                    state->gameOver = true;
                }
            }
        }
    }
}

void CheckLevelProgression(GameState* state) {
    // If target score reached, you could load next level
    // For now, we just print
    if (state->score >= state->levelTargetScore) {
        DrawText("TARGET REACHED!", 200, 200, 40, GREEN);
    }
}

// --- FILE LOADER (The Bridge) ---
void LoadLevel(GameState* state, const char* filename) {
    // 1. Cleanup old memory
    for(int i=0; i<state->entityCount; i++) {
        if (state->entities[i].data) free(state->entities[i].data);
    }
    state->entityCount = 0;
    state->gameOver = false;
    state->levelTargetScore = 999;
    state->levelBaseSpeed = 0.15f;

    FILE* file = fopen(filename, "r");
    if (!file) { printf("Failed to load %s\n", filename); return; }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;

        // A. Parse Metadata
        if (strncmp(line, "META TARGET", 11) == 0) { sscanf(line, "META TARGET %d", &state->levelTargetScore); continue; }
        if (strncmp(line, "META SPEED", 10) == 0) { sscanf(line, "META SPEED %f", &state->levelBaseSpeed); continue; }

        // B. Parse Entity
        char typeChar;
        int x, y, w, h, val, sub;
        float spd;

        int matches = sscanf(line, "%c %d %d %d %d %d %d %f", &typeChar, &x, &y, &w, &h, &val, &sub, &spd);
        
        if (matches > 0) {
            // Defaults
            if (matches < 6) val = 10;
            if (matches < 7) sub = 1;
            if (matches < 8) spd = 0.0f;

            EntityType type = ENTITY_NONE;
            if (typeChar == 'P') type = ENTITY_SNAKE;
            else if (typeChar == 'A') type = ENTITY_APPLE;
            else if (typeChar == 'W') type = ENTITY_WALL;
            else if (typeChar == 'E') type = ENTITY_ENEMY_BASIC;
            else if (typeChar == 'C') type = ENTITY_COIN;

            Entity* e = SpawnEntity(state, type, (Vector2){(float)x, (float)y}, (Vector2){(float)w, (float)h});
            if (e) {
                // Store generics just in case
                e->propertyValue = val;
                e->propertySubtype = sub;
                e->propertySpeed = spd;

                // C. Map Generics to Specifics
                if (type == ENTITY_SNAKE) {
                    SnakeData* sData = (SnakeData*)malloc(sizeof(SnakeData));
                    InitSnake(sData, e->position);
                    // Map Subtype -> Direction
                    if (sub == 0) sData->direction = (Vector2){0, -1};      // Up
                    else if (sub == 1) sData->direction = (Vector2){1, 0};  // Right
                    else if (sub == 2) sData->direction = (Vector2){0, 1};  // Down
                    else if (sub == 3) sData->direction = (Vector2){-1, 0}; // Left
                    e->data = sData;
                }
                else if (type == ENTITY_APPLE || type == ENTITY_COIN) {
                    AppleData* aData = (AppleData*)malloc(sizeof(AppleData));
                    aData->value = val; // Map generic value -> specific points
                    e->data = aData;
                }
                else if (type == ENTITY_ENEMY_BASIC) {
                    EnemyData* enData = (EnemyData*)malloc(sizeof(EnemyData));
                    enData->speed = spd;
                    enData->moveTimer = 0;
                    if (sub == 0) enData->direction = (Vector2){0, -1};
                    else enData->direction = (Vector2){1, 0};
                    e->data = enData;
                }
            }
        }
    }
    fclose(file);
    printf("Level Loaded. Target: %d, Speed: %.2f\n", state->levelTargetScore, state->levelBaseSpeed);
}