#include "render_system.hpp"

void RenderSystem::Render() {
    for (const auto& entityContainer : entityManager->GetEntities()) {
        Entity entity = entityContainer.entity;
        ComponentMask mask = entityContainer.mask;

        // Check if entity has both Transform2D and Rectangle components
        unsigned int transform = componentManager->GetComponentID<Transform2D>();
        unsigned int rect = componentManager->GetComponentID<Rectangle>();

        if (mask.test(transform) && mask.test(rect)) {
            Transform2D& transform = componentManager->GetComponent<Transform2D>(entity);
            Rectangle& rect = componentManager->GetComponent<Rectangle>(entity);

            SDL_FRect sdlRect;
            sdlRect.x = transform.position.x;
            sdlRect.y = transform.position.y;
            sdlRect.w = rect.width;
            sdlRect.h = rect.height;

            if (rect.isCentred) {
                sdlRect.x -= rect.width/2;
                sdlRect.y -= rect.height/2;
            }
            
            SDL_SetRenderDrawColor(renderer, rect.colour.r, rect.colour.g, rect.colour.b, rect.colour.a);
            SDL_RenderFillRect(renderer, &sdlRect);
        }
    }
}