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
            sdlRect.x = transform.GetPosition().x;
            sdlRect.y = transform.GetPosition().y;
            sdlRect.w = rect.GetWidth();
            sdlRect.h = rect.GetHeight();

            if (rect.IsCentred()) {
                sdlRect.x -= rect.GetWidth()/2;
                sdlRect.y -= rect.GetHeight()/2;
            }

            vec4 colour = rect.GetColour();
            SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
            SDL_RenderFillRect(renderer, &sdlRect);
        }
    }
}