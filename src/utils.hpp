#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

enum ColourCode {
    FG_RED      = 31,
    FG_GREEN    = 32,
    FG_BLUE     = 34,
    FG_YELLOW   = 33,
    FG_MAGENTA  = 35,
    FG_CYAN     = 36,
    FG_WHITE    = 37,
    FG_DEFAULT  = 39,
    BG_RED      = 41,
    BG_GREEN    = 42,
    BG_BLUE     = 44,
    BG_YELLOW   = 43,
    BG_MAGENTA  = 45,
    BG_CYAN     = 46,
    BG_WHITE    = 47,
};

class Modifier {
    ColourCode colour;
public:
    Modifier(ColourCode code) : colour(code) {}
    friend std::ostream& operator<<(std::ostream& os, const Modifier& modifier) {
        return os << "\033[" << modifier.colour << "m";
    }
};

#define TEXT_RESET      Modifier(FG_DEFAULT)
#define TEXT_RED        Modifier(FG_RED)
#define TEXT_GREEN      Modifier(FG_GREEN)
#define TEXT_BLUE       Modifier(FG_BLUE)
#define TEXT_YELLOW     Modifier(FG_YELLOW)
#define TEXT_MAGENTA    Modifier(FG_MAGENTA)
#define TEXT_CYAN       Modifier(FG_CYAN)
#define TEXT_WHITE      Modifier(FG_WHITE)

bool DoesFileExist(const char* filePath);

char* ReadFile(const char* filePath);

void UnloadFileText(char* text);

bool IsSubstring(const std::string& str, const std::string& substr);