#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <string>

enum GameState { 
    MENU,
    SHOP,
    GAME,
    WIN 
};

struct Bullet { 
    Vector2 pos; 
    Vector2 vel; 
    bool active; 
};

struct Enemy { 
    Vector2 pos; 
    bool active; 
    int hp;
};

int main() {
    InitWindow(800, 450, "Bobby's 2D Strike: JUICE UPDATE");
    SetTargetFPS(60);

    GameState state = MENU;
    int kills = 0;
    
    float shakeTime = 0.0f;
    Camera2D cam = { 0 };
    cam.zoom = 1.0f;

    Color currentMapColor = DARKGRAY;
    float friction = 0.85f;
    Vector2 bobbyPos = { 400, 225 };
    Vector2 bobbyVel = { 0, 0 };
    float acceleration = 0.8f;
    
    Color mySkin = BLUE;
    float mySize = 15.0f;
    const char* skinName = "Default Blue";

    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;
    
    for (int i = 0; i < 5; i++) {
        enemies.push_back({ 
            {(float)GetRandomValue(0, 800), (float)GetRandomValue(0, 450)},
            true,
            3
        });
    }

    while (!WindowShouldClose()) {
        
        if (shakeTime > 0) {
            shakeTime -= 1.0f;
            cam.offset.x = (float)GetRandomValue(-5, 5);
            cam.offset.y = (float)GetRandomValue(-5, 5);
        } else {
            cam.offset = { 0, 0 };
        }

        if (state == MENU) {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("BOBBY'S STRIKE", 240, 50, 40, WHITE);
            DrawText("The Juice Update", 320, 90, 20, YELLOW);

            DrawRectangle(150, 150, 200, 100, BEIGE);
            DrawText("DUST II", 200, 180, 20, DARKBROWN);
            
            DrawRectangle(450, 150, 200, 100, SKYBLUE);
            DrawText("ICE WORLD", 490, 180, 20, WHITE);

            DrawRectangle(300, 300, 200, 60, PURPLE);
            DrawText("CUSTOMIZE", 340, 320, 20, WHITE);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 t = GetMousePosition();
                if (t.x > 150 && t.x < 350 && t.y > 150 && t.y < 250) {
                    currentMapColor = BEIGE; friction = 0.85f; state = GAME;
                }
                if (t.x > 450 && t.x < 650 && t.y > 150 && t.y < 250) {
                    currentMapColor = SKYBLUE; friction = 0.99f; state = GAME;
                }
                if (t.x > 300 && t.x < 500 && t.y > 300 && t.y < 360) {
                    state = SHOP;
                }
            }
            EndDrawing();
        }

        else if (state == SHOP) {
            BeginDrawing();
            ClearBackground(DARKGRAY);
            DrawText("ARMORY", 320, 30, 40, WHITE);

            DrawRectangle(50, 140, 80, 80, RED);
            DrawRectangle(150, 140, 80, 80, GOLD);
            DrawRectangle(250, 140, 80, 80, BLACK); DrawRectangleLines(250, 140, 80, 80, WHITE);
            DrawRectangle(50, 230, 80, 80, MAGENTA);
            DrawRectangle(150, 230, 80, 80, YELLOW);
            DrawRectangle(50, 350, 100, 50, GRAY); DrawText("NINJA", 70, 365, 20, WHITE);
            DrawRectangle(180, 350, 100, 50, GRAY); DrawText("TANK", 215, 365, 20, WHITE);
            DrawRectangle(20, 20, 80, 40, RED); DrawText("BACK", 35, 30, 20, WHITE);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 t = GetMousePosition();
                if (t.x > 50 && t.x < 130 && t.y > 140 && t.y < 220) { mySkin = RED; skinName = "Commando"; }
                if (t.x > 150 && t.x < 230 && t.y > 140 && t.y < 220) { mySkin = GOLD; skinName = "Golden God"; }
                if (t.x > 250 && t.x < 330 && t.y > 140 && t.y < 220) { mySkin = BLACK; skinName = "Stealth"; }
                if (t.x > 50 && t.x < 130 && t.y > 230 && t.y < 310) { mySkin = MAGENTA; skinName = "Glitch Pink"; }
                if (t.x > 150 && t.x < 230 && t.y > 230 && t.y < 310) { mySkin = YELLOW; skinName = "Hazard"; }
                if (t.x > 50 && t.x < 150 && t.y > 350 && t.y < 400) mySize = 10.0f;
                if (t.x > 180 && t.x < 280 && t.y > 350 && t.y < 400) mySize = 25.0f;
                if (t.x > 20 && t.x < 100 && t.y > 20 && t.y < 60) state = MENU;
            }
            DrawRectangle(500, 120, 250, 250, BLACK);
            DrawText("PREVIEW", 570, 140, 20, LIGHTGRAY);
            DrawCircleV({625, 245}, mySize, mySkin);
            DrawText(skinName, 580, 320, 20, mySkin);
            EndDrawing();
        }

        else if (state == GAME) {
            
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                Vector2 t = GetMousePosition();
                t = GetScreenToWorld2D(t, cam); 
                
                if (Vector2Distance(bobbyPos, t) > 10.0f) {
                    Vector2 dir = Vector2Normalize(Vector2Subtract(t, bobbyPos));
                    bobbyVel = Vector2Add(bobbyVel, Vector2Scale(dir, acceleration));
                }
            }

            bobbyVel = Vector2Scale(bobbyVel, friction);
            bobbyPos = Vector2Add(bobbyPos, bobbyVel);
            
            if (bobbyPos.x < 0 || bobbyPos.x > 800) bobbyVel.x *= -1;
            if (bobbyPos.y < 0 || bobbyPos.y > 450) bobbyVel.y *= -1;

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                Vector2 t = GetMousePosition();
                t = GetScreenToWorld2D(t, cam);
                
                Vector2 aim = Vector2Normalize(Vector2Subtract(t, bobbyPos));
                bullets.push_back({ bobbyPos, Vector2Scale(aim, 12.0f), true });
                bobbyVel = Vector2Subtract(bobbyVel, Vector2Scale(aim, 2.0f)); 
            }

            for (auto &b : bullets) {
                if (b.active) b.pos = Vector2Add(b.pos, b.vel);
                
                for (auto &e : enemies) {
                    if (e.active && CheckCollisionCircles(b.pos, 5, e.pos, 20)) {
                        b.active = false;
                        e.hp = e.hp - 1;
                        
                        shakeTime = 10.0f; 
                        
                        if (e.hp <= 0) {
                            e.active = false; 
                            shakeTime = 20.0f;
                            
                            if (kills < 10) {
                                e.pos = { (float)GetRandomValue(0, 800), (float)GetRandomValue(0, 450) };
                                e.active = true;
                                e.hp = 3;
                                kills++;
                            } else {
                                state = WIN;
                            }
                        }
                    }
                }
            }

            BeginDrawing();
            ClearBackground(currentMapColor);
            
            BeginMode2D(cam); 

            for (auto &e : enemies) {
                if (e.active) {
                    Color enemyColor = RED;
                    if (e.hp == 2) enemyColor = ORANGE;
                    if (e.hp == 1) enemyColor = MAROON;
                    DrawCircleV(e.pos, 20, enemyColor);
                }
            }

            DrawCircleV(bobbyPos, mySize, mySkin);
            for (auto &b : bullets) if (b.active) DrawCircleV(b.pos, 5, YELLOW);
            
            EndMode2D(); 

            DrawText(TextFormat("KILLS: %i / 10", kills), 20, 20, 20, BLACK);
            EndDrawing();
        }

        else if (state == WIN) {
            BeginDrawing();
            ClearBackground(WHITE);
            DrawText("MISSION COMPLETE", 200, 180, 40, GREEN);
            DrawText("Tap to Play Again", 290, 250, 20, DARKGRAY);
            EndDrawing();
            
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                kills = 0; 
                bobbyVel = {0,0}; 
                bobbyPos = {400, 225};
                shakeTime = 0;
                cam.offset = {0,0};
                for (auto &e : enemies) { 
                    e.active = true; 
                    e.hp = 3; 
                    e.pos = {(float)GetRandomValue(0,800), (float)GetRandomValue(0,450)}; 
                }
                state = MENU;
            }
        }
    }
    CloseWindow();
    return 0;
}

