#include "core/app.hpp"

int main(int argc, char **argv) {
    App app("Game", 800, 600, false);

    while (app.Run()) {
        
    }

    app.Close();

    return 0;
}