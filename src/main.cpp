#include "app.hpp"

int main(int argc, char **argv) {
    App app;
    app.Init("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    app.Run();

    return 0;
}