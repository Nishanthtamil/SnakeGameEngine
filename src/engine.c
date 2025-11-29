#include "game_types.h"
#include <string.h> 

void InitSnake(SnakeData* s, Vector2 startPos) {
    s->count = 1;
    s->capacity = 10;
    s->body = (Vector2*)malloc(s->capacity * sizeof(Vector2));
    s->body[0] = startPos;
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
    s->body[0].x += s->direction.x * 20;
    s->body[0].y += s->direction.y * 20;

    if (s->body[0].x < 0) s->body[0].x = 0;
    if (s->body[0].y < 0) s->body[0].y = 0;
    if (s->body[0].x >= SCREEN_W) s->body[0].x = SCREEN_W - 20;
    if (s->body[0].y >= SCREEN_H) s->body[0].y = SCREEN_H - 20;
}

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
    
    Event* e = &state->eventQueue[state->eventTail];
    e->type = type;
    e->sender = a;
    e->receiver = b;
    
    state->eventTail = (state->eventTail + 1) % MAX_EVENTS;
    state->pendingEvents++;
}

void ResolveCollisions(GameState* state) {
    SpatialGrid grid;
    memset(&grid, 0, sizeof(SpatialGrid));

    for (int i = 0; i < state->entityCount; i++) {
        Entity* e = &state->entities[i];
        if (!e->active) continue;

        int startCol = (int)(e->position.x / CELL_SIZE);
        int startRow = (int)(e->position.y / CELL_SIZE);
        int endCol = (int)((e->position.x + e->size.x) / CELL_SIZE);
        int endRow = (int)((e->position.y + e->size.y) / CELL_SIZE);

        if (startCol < 0) startCol = 0;
        if (startRow < 0) startRow = 0;
        if (endCol >= GRID_COLS) endCol = GRID_COLS - 1;
        if (endRow >= GRID_ROWS) endRow = GRID_ROWS - 1;

        for (int r = startRow; r <= endRow; r++) {
            for (int c = startCol; c <= endCol; c++) {
                int idx = c + (r * GRID_COLS);
                
                if (grid.cells[idx].count < 20) {
                    grid.cells[idx].entries[grid.cells[idx].count++] = e;
                }
            }
        }
    }

    for (int i = 0; i < TOTAL_CELLS; i++) {
        GridCell* cell = &grid.cells[i];
        if (cell->count < 2) continue;

        for (int a = 0; a < cell->count; a++) {
            for (int b = a + 1; b < cell->count; b++) {
                Entity* e1 = cell->entries[a];
                Entity* e2 = cell->entries[b];
                
                bool hit = (e1->position.x < e2->position.x + e2->size.x &&
                            e1->position.x + e1->size.x > e2->position.x &&
                            e1->position.y < e2->position.y + e2->size.y &&
                            e1->position.y + e1->size.y > e2->position.y);
                
                if (hit) PushEvent(state, EVENT_COLLISION, e1, e2);
            }
        }
    }
}

void ProcessEvents(GameState* state) {
    while (state->pendingEvents > 0) {
        Event e = state->eventQueue[state->eventHead];
        state->eventHead = (state->eventHead + 1) % MAX_EVENTS;
        state->pendingEvents--;

        if (e.type == EVENT_COLLISION) {
            Entity* snake = (e.sender->type == ENTITY_SNAKE) ? e.sender : e.receiver;
            Entity* other = (e.sender->type == ENTITY_SNAKE) ? e.receiver : e.sender;

            if (snake->type == ENTITY_SNAKE && other->type == ENTITY_APPLE) {
                SnakeData* sData = (SnakeData*)snake->data;
                Vector2 tailPos = sData->body[sData->count-1];
                AppendSnake(sData, tailPos); 
                other->active = false; 
                state->score += 10;
            }
            else if (snake->type == ENTITY_SNAKE && other->type == ENTITY_WALL) {
                state->gameOver = true;
            }
        }
    }
}

void CheckLevelProgression(GameState* state){
    bool levelChanged = false;
    if (state->currentLevel == 1 && state->score >= 30){
        state->currentLevel =2;
        LoadLevel(state,"assets/level2.eng");
        levelChanged = true;
    }
    else if (state->currentLevel ==2 && state->score >= 60){
        state->currentLevel = 3;
        LoadLevel(state,"assets/level3.eng");
        levelChanged = true;
    }
    if (levelChanged){
        printf("LEVEL UP! Entered Level %d\n",state->currentLevel);
    }
}

void LoadLevel(GameState* state, const char* filename) {
    state->entityCount = 0;
    state->gameOver = false;

    FILE* file = fopen(filename, "r");
    if (!file) { printf("Failed to load level\n"); return; }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;

        char type;
        if (sscanf(line, "%c", &type) > 0) {
            int x, y, w, h;
            if (type == 'P') { 
                sscanf(line + 1, "%d %d", &x, &y);
                Entity* e = SpawnEntity(state, ENTITY_SNAKE, (Vector2){x,y}, (Vector2){20,20});
                SnakeData* sData = (SnakeData*)malloc(sizeof(SnakeData));
                InitSnake(sData, (Vector2){x,y});
                e->data = sData;
            }
            else if (type == 'A') {
                sscanf(line + 1, "%d %d", &x, &y);
                Entity* e = SpawnEntity(state, ENTITY_APPLE, (Vector2){x,y}, (Vector2){20,20});
                e->data = malloc(sizeof(AppleData)); 
            }
            else if (type == 'W') {
                sscanf(line + 1, "%d %d %d %d", &x, &y, &w, &h);
                SpawnEntity(state, ENTITY_WALL, (Vector2){x,y}, (Vector2){w,h});
            }
        }
    }
    fclose(file);
    printf("Level Loaded!\n");
}