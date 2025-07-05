#include "app.hpp"
#include "utils/utils.hpp"

#include "SDL3/SDL.h"
#include "SDL3/SDL_video.h"
#include "SDL3_image/SDL_image.h"

#include "ecs/components/collisions.hpp"
#include "ecs/components/transform_2d.hpp"
#include "ecs/components/physics_2d.hpp"
#include "ecs/components/rectangle.hpp"
#include "ecs/components/player_controller.hpp"

#include "ecs/systems/render_system.hpp"
#include "ecs/systems/input_handler.hpp"
#include "ecs/systems/movement.hpp"
#include "ecs/systems/collisions.hpp"
#include "ecs/systems/physics.hpp"

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

App::~App() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

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

    SetUpEntities();

    RenderSystem renderSystem(renderer);
    InputHandler inputHandlerSystem;
    MovementSystem movementSystem;
    PhysicsSystem physicsSystem;
    Collisions collisions;

    systemManager->AddRenderSystem(&renderSystem);
    systemManager->AddUpdateSystem(&inputHandlerSystem);
    systemManager->AddUpdateSystem(&physicsSystem);
    systemManager->AddUpdateSystem(&collisions);
    systemManager->AddUpdateSystem(&movementSystem); // Order matters - movement should go last

    state = AppState::Running;

    const float oneTargetFPS = 1.f/targetFPS;

    while (state != AppState::Quitting) {
        timer.Tick();
        deltaTime += timer.timeElapsedSecs;
        if (isless(deltaTime, oneTargetFPS)) continue;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                state = AppState::Quitting;
        }

        switch (state) {
            case AppState::Loading:
                // TODO: Load assets
                break;
            case AppState::Menu:
                // TODO: Menu
                break;
            case AppState::Paused:
                systemManager->Render();
                break;
            case AppState::Running:
                systemManager->Update(deltaTime);
                systemManager->Render();

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

void App::SetUpEntities() {
    
    // Create paddle
    Entity paddle = entityManager->CreateEntity();
    Transform2D paddleTransform;
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    paddleTransform.position = vec2(10, height/2.f);
    entityManager->AddComponent<Transform2D>(paddle, paddleTransform);
    Rectangle paddleRect;
    paddleRect.colour = vec4(255.f, 0.f, 0.f, 255.f);
    paddleRect.width = 20.f;
    paddleRect.height = 100.f;
    paddleRect.isCentred = true;
    entityManager->AddComponent<Rectangle>(paddle, paddleRect);
    Physics2D paddlePhysics;
    paddlePhysics.speed = 500.f;
    paddlePhysics.coefficientOfFriction = 0.9f;
    entityManager->AddComponent<Physics2D>(paddle, paddlePhysics);
    entityManager->AddComponent<PlayerController>(paddle);
    CollisionEdgeOfScreen edgeOfScreen;
    edgeOfScreen.reflectionMultiplier = 0.f;
    entityManager->AddComponent<CollisionEdgeOfScreen>(paddle, edgeOfScreen);

    // Create bll
    Entity ball = entityManager->CreateEntity();
    Transform2D balltransform;
    balltransform.position = vec2(width/2.f, height/2.f);
    entityManager->AddComponent<Transform2D>(ball, balltransform);
    Rectangle ballRect;
    ballRect.colour = vec4(0.f, 0.f, 255.f, 255.f);
    ballRect.width = 20.f;
    ballRect.height = 20.f;
    ballRect.isCentred = true;
    entityManager->AddComponent<Rectangle>(ball, ballRect);
    Physics2D ballPhysics;
    ballPhysics.velocity = vec2(-250.f, 100.f);
    entityManager->AddComponent<Physics2D>(ball, ballPhysics);
    entityManager->AddComponent<CollisionEdgeOfScreen>(ball);

    entityManager->AddComponent<RectCollider>(ball, RectCollider(vec2(20.f, 20.f)));
    entityManager->AddComponent<RectCollider>(paddle, RectCollider(vec2(20.f, 100.f)));
}