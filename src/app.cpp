#include "app.hpp"
#include "utils/utils.hpp"

#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

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

    Log("Initialising Objects...");
    paddlePos = vec2(10, height/2);
    ballPos = vec2(width/2, height/2);
    Log("Done.");
}

void App::Run() {
    state = AppState::Loading;
    // TODO: Any loading should be done here
    float paddleWidth = 20.f;
    float paddleHeight = 200.f;
    float paddleSpeed = 0.f;
    SDL_FRect paddle = {paddlePos.x - paddleWidth/2, paddlePos.y - paddleHeight/2, paddleWidth, paddleHeight};
    float ballRadius = 20.f;
    SDL_FRect ball = {ballPos.x - ballRadius, ballPos.y - ballRadius, ballRadius, ballRadius};

    state = AppState::Running;

    const float oneTargetFPS = 1.f/targetFPS;

    while (state != AppState::Quitting) {
        timer.Tick();
        deltaTime += timer.timeElapsedSecs;
        Log("timer.timeElapsedSecs = " + to_string(timer.timeElapsedSecs));
        Log("deltaTime = " + to_string(deltaTime));
        if (isless(deltaTime, oneTargetFPS)) continue;
        deltaTime -= oneTargetFPS;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        switch (state) {
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
                    if (event.type == SDL_EVENT_KEY_DOWN) {
                        if (event.key.key == SDLK_W) paddleSpeed -= 1*oneTargetFPS;
                        if (event.key.key == SDLK_S) paddleSpeed += 1*oneTargetFPS;
                    }
                }

                if (abs(paddleSpeed) < 0.05f) paddleSpeed = 0.f;
                paddle.y += paddleSpeed;
                paddleSpeed *= 0.5f;
                
                // Set draw colour
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

                // Draw objects
                SDL_RenderFillRect(renderer, &paddle);
                SDL_RenderFillRect(renderer, &ball);

                // Log("(Engine) PaddleSpeed = " + to_string(paddleSpeed));
                break;
            case AppState::GameOver:
                // TODO: Game over
                break;
            default:
                break;
        }
        SDL_RenderPresent(renderer);
    }
}

App::~App() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}