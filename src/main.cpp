#include "core/consul.hpp"

int main(int argc, char **argv) {
    Consul consul("Game", false);

    consul.VSync(true);

    while (consul.Run()) {

    }

    consul.Terminate();

    return 0;
}