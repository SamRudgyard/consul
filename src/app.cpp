#include "app.hpp"
#include "utils/utils.hpp"

#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"

using namespace std;

void App::Init(const char* title, int x, int y, int width, int height, Uint32 flags) {
    Log("(SDL) Initialising video subsystem...");
    if (SDL_Init(SDL_INIT_VIDEO)) {
        Log("(SDL) Done.");
    } else Error("(SDL) Failed to initialize SDL. " + string(SDL_GetError()));
    Log("(SDL) Creating window and renderer...");
    if (SDL_CreateWindowAndRenderer(title, width, height, flags, &window, &renderer)) {
        Log("(SDL) Done.");
    } else Error("(SDL) Failed to create window and/or renderer. " + string(SDL_GetError()));

    if (!window)   Error("(SDL) Window could not be created. " + string(SDL_GetError()));
    if (!renderer) Error("(SDL) Renderer could not be created. " + string(SDL_GetError()));
}

void App::Run() {
    state = AppState::Loading;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    state = AppState::Running;

    while (state != AppState::Quitting) {
        switch (state) {
            case AppState::Startup:
                break;
            case AppState::Loading:
                // TODO: Load assets
                break;
            case AppState::Menu:
                // TODO: Menu
                break;
            case AppState::Paused:
                // TODO: Paused
                break;
            case AppState::Running:
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_EVENT_QUIT) state = AppState::Quitting;
                }
                break;
            case AppState::GameOver:
                // TODO: Game over
                break;
            case AppState::Quitting:
                break;
        }
        
    }
}