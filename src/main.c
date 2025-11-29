#include "raylib.h"
#include "game_types.h"
#include "engine.c" 

// Helper to draw a cool grid background
void DrawCyberGrid() {
    for (int i = 0; i < SCREEN_W; i += CELL_SIZE) {
        DrawLine(i, 0, i, SCREEN_H, Fade(DARKGREEN, 0.2f));
    }
    for (int i = 0; i < SCREEN_H; i += CELL_SIZE) {
        DrawLine(0, i, SCREEN_W, i, Fade(DARKGREEN, 0.2f));
    }
}

int main() {
    InitWindow(SCREEN_W, SCREEN_H, "Snake Engine Pro");
    SetTargetFPS(60);

    GameState state;
    state.score = 0;
    state.currentLevel = 1;
    
    // Load the first level
    LoadLevel(&state, "assets/level1.eng");

    while (!WindowShouldClose()) {
        // --- HOT RELOAD ---
        if (IsKeyPressed(KEY_F5)) {
            // Reload current level based on ID
            if (state.currentLevel == 1) LoadLevel(&state, "assets/level1.eng");
            if (state.currentLevel == 2) LoadLevel(&state, "assets/level2.eng");
            if (state.currentLevel == 3) LoadLevel(&state, "assets/level3.eng");
        }

        if (!state.gameOver) {
            // --- INPUT ---
            for(int i=0; i<state.entityCount; i++) {
                if(state.entities[i].type == ENTITY_SNAKE) {
                    SnakeData* s = (SnakeData*)state.entities[i].data;
                    if (IsKeyPressed(KEY_UP) && s->direction.y == 0) s->direction = (Vector2){0, -1};
                    if (IsKeyPressed(KEY_DOWN) && s->direction.y == 0) s->direction = (Vector2){0, 1};
                    if (IsKeyPressed(KEY_LEFT) && s->direction.x == 0) s->direction = (Vector2){-1, 0};
                    if (IsKeyPressed(KEY_RIGHT) && s->direction.x == 0) s->direction = (Vector2){1, 0};
                    
                    s->moveTimer += GetFrameTime();
                    // Speed up slightly based on level!
                    float speed = 0.15f - (state.currentLevel * 0.02f); 
                    if (s->moveTimer >= speed) {
                        MoveSnake(s);
                        state.entities[i].position = s->body[0]; 
                        s->moveTimer = 0.0f;
                    }
                }
            }

            // --- ENGINE PIPELINE ---
            ResolveCollisions(&state); 
            ProcessEvents(&state);     
            CheckLevelProgression(&state); // NEW: Check if we passed the level
        }
        else {
            // Game Over Restart
            if (IsKeyPressed(KEY_ENTER)) {
                state.score = 0;
                state.currentLevel = 1;
                LoadLevel(&state, "assets/level1.eng");
            }
        }

        // --- RENDER ---
        BeginDrawing();
        ClearBackground(BLACK);
        DrawCyberGrid(); // Draw the background

        if (state.gameOver) {
            DrawText("SYSTEM FAILURE", 250, 200, 40, RED);
            DrawText("PRESS ENTER TO REBOOT", 260, 260, 20, DARKGRAY);
            DrawText(TextFormat("FINAL SCORE: %d", state.score), 320, 320, 20, WHITE);
        } else {
            // Draw Entities
            for (int i = 0; i < state.entityCount; i++) {
                Entity* e = &state.entities[i];
                if (!e->active) continue;

                if (e->type == ENTITY_WALL) {
                    DrawRectangleLines(e->position.x, e->position.y, e->size.x, e->size.y, BLUE);
                    DrawRectangle(e->position.x+2, e->position.y+2, e->size.x-4, e->size.y-4, Fade(BLUE, 0.5f));
                }
                else if (e->type == ENTITY_APPLE) {
                     DrawRectangleV(e->position, e->size, RED);
                     // Add a little shine
                     DrawRectangle(e->position.x+4, e->position.y+4, 4, 4, WHITE);
                }
                else if (e->type == ENTITY_SNAKE) {
                    SnakeData* s = (SnakeData*)e->data;
                    for(int j=0; j<s->count; j++) {
                        Color col = (j == 0) ? GREEN : DARKGREEN; // Head is brighter
                        DrawRectangleV(s->body[j], e->size, col);
                    }
                }
            }
            // UI
            DrawText(TextFormat("SCORE: %d", state.score), 10, 10, 20, GREEN);
            DrawText(TextFormat("LEVEL: %d", state.currentLevel), 10, 35, 20, GREEN);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}