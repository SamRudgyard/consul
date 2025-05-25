#pragma once

#include "SDL3/SDL.h"

enum class AppState {
    Startup,
    Loading,
    Menu,
    Paused,
    Running,
    GameOver,
    Quitting
};

class App {
public:
    App() = default;
    ~App() = default;
    void Init(const char* title, int x, int y, int width, int height, Uint32 flags);
    void Run();

    AppState state = AppState::Startup;
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
};