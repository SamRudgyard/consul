#include "render_system.hpp"

void RenderSystem::Render() {
    entityManager->ForEach<Transform2D, Rectangle>([&](Transform2D& transform, Rectangle& rect) {
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
    });
}