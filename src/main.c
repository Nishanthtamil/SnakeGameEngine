#include "raylib.h"
#include "game_types.h"
#include "engine.c" 

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
    
    // Default load
    LoadLevel(&state, "assets/level1.eng");

    while (!WindowShouldClose()) {
        // --- HOT RELOAD ---
        if (IsKeyPressed(KEY_F5)) {
            LoadLevel(&state, "assets/level1.eng");
        }

        if (!state.gameOver) {
            // --- UPDATE LOOP ---
            
            // 1. Snake Input & Move
            for(int i=0; i<state.entityCount; i++) {
                Entity* e = &state.entities[i];
                if (!e->active) continue;

                if(e->type == ENTITY_SNAKE) {
                    SnakeData* s = (SnakeData*)e->data;
                    if (IsKeyPressed(KEY_UP) && s->direction.y == 0) s->direction = (Vector2){0, -1};
                    if (IsKeyPressed(KEY_DOWN) && s->direction.y == 0) s->direction = (Vector2){0, 1};
                    if (IsKeyPressed(KEY_LEFT) && s->direction.x == 0) s->direction = (Vector2){-1, 0};
                    if (IsKeyPressed(KEY_RIGHT) && s->direction.x == 0) s->direction = (Vector2){1, 0};
                    
                    s->moveTimer += GetFrameTime();
                    
                    // USE LEVEL SPEED
                    if (s->moveTimer >= state.levelBaseSpeed) {
                        MoveSnake(s);
                        e->position = s->body[0]; 
                        s->moveTimer = 0.0f;
                    }
                }
                
                // 2. Enemy Move (Basic)
                else if (e->type == ENTITY_ENEMY_BASIC) {
                    EnemyData* en = (EnemyData*)e->data;
                    en->moveTimer += GetFrameTime();
                    // Simple patrol logic could go here
                    // e->position.x += en->direction.x * en->speed;
                }
            }

            ResolveCollisions(&state); 
            ProcessEvents(&state);     
            CheckLevelProgression(&state);
        }
        else {
            if (IsKeyPressed(KEY_ENTER)) {
                state.score = 0;
                LoadLevel(&state, "assets/level1.eng");
            }
        }

        // --- RENDER ---
        BeginDrawing();
        ClearBackground(BLACK);
        DrawCyberGrid();

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
                    DrawRectangleRec((Rectangle){e->position.x, e->position.y, e->size.x, e->size.y}, BLUE);
                    DrawRectangleLinesEx((Rectangle){e->position.x, e->position.y, e->size.x, e->size.y}, 1, WHITE);
                }
                else if (e->type == ENTITY_APPLE) {
                     DrawRectangleV(e->position, e->size, RED);
                }
                else if (e->type == ENTITY_COIN) {
                     DrawRectangleV(e->position, e->size, GOLD);
                }
                else if (e->type == ENTITY_ENEMY_BASIC) {
                     DrawRectangleV(e->position, e->size, PURPLE);
                }
                else if (e->type == ENTITY_SNAKE) {
                    SnakeData* s = (SnakeData*)e->data;
                    for(int j=0; j<s->count; j++) {
                        Color col = (j == 0) ? GREEN : DARKGREEN; 
                        DrawRectangleV(s->body[j], e->size, col);
                    }
                }
            }
            // UI
            DrawText(TextFormat("SCORE: %d / %d", state.score, state.levelTargetScore), 10, 10, 20, GREEN);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}