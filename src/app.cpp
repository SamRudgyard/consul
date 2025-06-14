#include "app.hpp"
#include "utils/utils.hpp"

#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"
#include "ecs/components/transform_2d.hpp"
#include "ecs/components/rectangle.hpp"

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

    state = AppState::Running;

    const float oneTargetFPS = 1.f/targetFPS;

    float velocity = 0.f;
    const bool* keyboardState;
    float paddleSpeed = 300.f;

    Entity paddle = entityManager->CreateEntity();

    // Register components
    componentManager->RegisterComponent<Transform2D>();
    componentManager->RegisterComponent<Rectangle>();

    // Add components to paddle in entityManager
    entityManager->Add(paddle, componentManager->GetComponentID<Transform2D>());
    entityManager->Add(paddle, componentManager->GetComponentID<Rectangle>());

    // Add components to paddle in componentManager
    componentManager->AddComponent(paddle, Transform2D());
    componentManager->AddComponent(paddle, Rectangle());

    // Access the paddle's transform component and adjust its position
    Transform2D& paddleTransform = componentManager->GetComponent<Transform2D>(paddle);
    Log("Initial paddle position: " + to_string(paddleTransform.GetPosition().x) + ", " + to_string(paddleTransform.GetPosition().y));
    paddleTransform.SetPosition(paddlePos);

    while (state != AppState::Quitting) {
        timer.Tick();
        deltaTime += timer.timeElapsedSecs;
        if (isless(deltaTime, oneTargetFPS)) continue;

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
                    if (event.type == SDL_EVENT_QUIT)
                        state = AppState::Quitting;
                }

                keyboardState = SDL_GetKeyboardState(NULL);
                if (keyboardState[SDL_SCANCODE_W]) velocity -= paddleSpeed * deltaTime;
                if (keyboardState[SDL_SCANCODE_S]) velocity += paddleSpeed * deltaTime;
                
                // Set draw colour
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

                // Draw objects

                break;
            case AppState::GameOver:
                // TODO: Game over
                break;
            default:
                break;
        }
        SDL_RenderPresent(renderer);
        deltaTime = 0.f;
    }
}

App::~App() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}