#include "core/consul.hpp"

int main(int argc, char **argv) {
    Consul consul("Game", 800, 400, false);

    consul.VSync(true);

    while (consul.Run()) {

    }

    consul.Close();

    return 0;
}