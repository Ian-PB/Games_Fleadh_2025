#include "raylib.h"
#include "stdio.h"

#include "../include/Globals.h"
#include "../include/game.h"
#include "../include/MainMenu.h"
#include "../include/SceneCamera.h"

// Specific include for build_web
#if defined(WEB_BUILD)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

void run();
// Initialise Game
Game game;
MainMenu mainMenu;

int main(void)
{
    #if defined(WEB_BUILD)
    double cssWidth, cssHeight;
    emscripten_get_element_css_size("#canvas", &cssWidth, &cssHeight);
    SCREEN_WIDTH = (int)cssWidth;
    SCREEN_HEIGHT = (int)cssHeight;
    #else
    // Get the monitor's resolution
    SCREEN_WIDTH = GetMonitorWidth(0);
    SCREEN_HEIGHT = GetMonitorHeight(0);
    // Desktop flags
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_UNDECORATED | FLAG_FULLSCREEN_MODE);
    #endif

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib StarterKit GPPI");

    InitAudioDevice();

    game.initialize();
    mainMenu.initialize();

    SetExitKey(KEY_BACKSPACE);

    // For web builds, do not use WindowShouldClose
    // see https://github.com/raysan5/raylib/wiki/Working-for-Web-(HTML5)#41-avoid-raylib-whilewindowshouldclose-loop

#if defined(WEB_BUILD)
    emscripten_set_main_loop(run, 0, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Call GameLoop
        run();
    }
#endif

    CloseWindow();

    return 0;
}

int counter = 0;
char message[11];

void run()
{
    // Update Game Data
    switch (SceneCamera::currentScene)
    {
    case Scene::GAME:
        game.update();
        BeginDrawing();

        ClearBackground(BLACK);
        game.draw();
        EndDrawing();
        break; 

    case Scene::MAIN_MENU:
        mainMenu.update();
        BeginDrawing();
    
        ClearBackground(BLACK);
        mainMenu.draw();
        EndDrawing();
        break; 
    }
}
