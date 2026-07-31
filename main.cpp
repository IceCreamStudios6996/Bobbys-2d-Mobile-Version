/* 
 * PROJECT: BOBBY'S STRIKE (MOBILE ENGINE)
 * AUTHOR: Bentley
 * PLATFORM: C++ / Raylib / WebAssembly
 * DATE: 2026
 */

#include "raylib.h"
#include "raymath.h"
#include <vector>

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

int screenWidth = 1280;
int screenHeight = 720;
int kills = 0;
GameState state = MENU;
Vector2 bobbyPos;
Vector2 bobbyVel;
std::vector<Bullet> bullets;
std::vector<Enemy> enemies;
Color currentMapColor = DARKGRAY;
float friction = 0.85f;

void ResetGame() {
    kills = 0;
    bobbyPos = { (float)screenWidth/2, (float)screenHeight/2 };
    bobbyVel = { 0, 0 };
    bullets.clear(); 
    
    enemies.clear();
    for (int i = 0; i < 6; i++) {
        enemies.push_back({ 
            {(float)GetRandomValue(0, screenWidth), (float)GetRandomValue(0, screenHeight)},
            true,
            3
        });
    }
}

int main() {
    InitWindow(screenWidth, screenHeight, "Bobby's 2D Strike: SOURCE");
    InitAudioDevice(); 

    SetTargetFPS(60);

    Wave wavePew = GenWaveSquare(44100, 440.0f, 0.1f);
    Sound sfxShoot = LoadSoundFromWave(wavePew);
    SetSoundVolume(sfxShoot, 0.5f);
    
    Wave waveBoom = GenWaveWhiteNoise(44100, 0.2f); 
    Sound sfxBoom = LoadSoundFromWave(waveBoom);
    SetSoundPitch(sfxBoom, 0.5f); 

    UnloadWave(wavePew);
    UnloadWave(waveBoom);

    float shakeTime = 0.0f;
    Camera2D cam = { 0 };
    cam.zoom = 1.0f;
    cam.offset = { screenWidth/2.0f, screenHeight/2.0f };
    cam.target = { screenWidth/2.0f, screenHeight/2.0f };

    Color mySkin = BLUE;
    float mySize = 25.0f; 
    float acceleration = 1.2f; 

    ResetGame();

    while (!WindowShouldClose()) {
        
        if (shakeTime > 0) {
            shakeTime -= 1.0f;
            cam.offset.x = (screenWidth/2.0f) + GetRandomValue(-10, 10);
            cam.offset.y = (screenHeight/2.0f) + GetRandomValue(-10, 10);
        } else {
            cam.offset = { screenWidth/2.0f, screenHeight/2.0f };
        }
        
        cam.target = Vector2Lerp(cam.target, bobbyPos, 0.1f);

        if (state == MENU) {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("BOBBY'S STRIKE", screenWidth/2 - 220, 100, 60, WHITE);
            DrawText("C++ MOBILE ENGINE", screenWidth/2 - 150, 170, 30, YELLOW);
            DrawText("TAP A MAP TO START", screenWidth/2 - 140, 220, 20, GRAY);

            DrawRectangle(screenWidth/2 - 300, 300, 250, 150, BEIGE);
            DrawText("DUST II", screenWidth/2 - 240, 350, 30, DARKBROWN);
            
            DrawRectangle(screenWidth/2 + 50, 300, 250, 150, SKYBLUE);
            DrawText("ICE WORLD", screenWidth/2 + 100, 350, 30, WHITE);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                PlaySound(sfxShoot); 
                
                Vector2 t = GetMousePosition();
                if (t.x > screenWidth/2 - 300 && t.x < screenWidth/2 - 50 && t.y > 300 && t.y < 450) {
                    currentMapColor = BEIGE; friction = 0.85f; state = GAME;
                }
                if (t.x > screenWidth/2 + 50 && t.x < screenWidth/2 + 300 && t.y > 300 && t.y < 450) {
                    currentMapColor = SKYBLUE; friction = 0.99f; state = GAME;
                }
            }
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
            
            if (bobbyPos.x < 0 || bobbyPos.x > screenWidth) bobbyVel.x *= -1;
            if (bobbyPos.y < 0 || bobbyPos.y > screenHeight) bobbyVel.y *= -1;

            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                Vector2 t = GetMousePosition();
                t = GetScreenToWorld2D(t, cam);
                
                Vector2 aim = Vector2Normalize(Vector2Subtract(t, bobbyPos));
                bullets.push_back({ bobbyPos, Vector2Scale(aim, 18.0f), true }); 
                bobbyVel = Vector2Subtract(bobbyVel, Vector2Scale(aim, 5.0f)); 
                
                PlaySound(sfxShoot); 
            }

            for (auto &b : bullets) {
                if (b.active) b.pos = Vector2Add(b.pos, b.vel);
                
                for (auto &e : enemies) {
                    if (e.active && CheckCollisionCircles(b.pos, 8, e.pos, 30)) {
                        b.active = false;
                        e.hp = e.hp - 1;
                        shakeTime = 5.0f;
                        
                        if (e.hp <= 0) {
                            e.active = false; 
                            shakeTime = 30.0f;
                            PlaySound(sfxBoom); 
                            
                            if (kills < 10) {
                                e.pos = { (float)GetRandomValue(0, screenWidth), (float)GetRandomValue(0, screenHeight) };
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
            for(int x=0; x<screenWidth; x+=100) DrawLine(x, 0, x, screenHeight, Fade(BLACK, 0.2f));
            for(int y=0; y<screenHeight; y+=100) DrawLine(0, y, screenWidth, y, Fade(BLACK, 0.2f));

            for (auto &e : enemies) {
                if (e.active) {
                    Color enemyColor = RED;
                    if (e.hp == 2) enemyColor = ORANGE;
                    if (e.hp == 1) enemyColor = MAROON;
                    DrawCircleV(e.pos, 30, enemyColor);
                }
            }
            DrawCircleV(bobbyPos, mySize, mySkin);
            for (auto &b : bullets) if (b.active) DrawCircleV(b.pos, 8, YELLOW);
            EndMode2D(); 

            DrawText(TextFormat("KILLS: %i / 10", kills), 30, 30, 40, BLACK);
            EndDrawing();
        }

        else if (state == WIN) {
            BeginDrawing();
            ClearBackground(WHITE);
            DrawText("MISSION COMPLETE", screenWidth/2 - 250, screenHeight/2 - 50, 50, GREEN);
            DrawText("Tap to Restart", screenWidth/2 - 120, screenHeight/2 + 20, 30, GRAY);
            EndDrawing();
            
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                ResetGame();
                state = MENU;
            }
        }
    }
    
    UnloadSound(sfxShoot);
    UnloadSound(sfxBoom);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
