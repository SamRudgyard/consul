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

/**
 * Determines whether a given file path exists.
 * 
 * @param filePath Path to the file.
 * @returns True if the file exists; false otherwise.
 */
bool doesFileExist(const char* filePath);

/**
 * Read the contents of the given file.
 * 
 * @param filePath Path to the file.
 * @returns File contents as a string.
 */
const std::string readFile(const char* filePath);

/** 
 * Get the file extension from a given file path.
 * 
 * @param filePath Path to the file.
 * @returns File extension including the dot (e.g. ".txt"), or empty string if none.
 */
const std::string getFileExtension(const char* filePath);

/**
 * Frees memory previously allocated for a text buffer.
 * 
 * @param text Pointer to the char buffer to delete.
 */
void unloadFileText(char* text);

/**
 * Determine if a substring exists within a string.
 * Note: This is case-sensitive.
 * 
 * @param str The string to search within.
 * @param substr The substring to search for.
 * @returns True if substr is found within str; false otherwise.
 */
bool isSubstring(const std::string& str, const std::string& substr);

void waitTime(double seconds);

/** 
 * Check for OpenGL errors and log them with file and line information.
 * 
 * @param file Source file name where the check is called.
 * @param line Line number in the source file.
 */
void glCheckError_(const char* file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)