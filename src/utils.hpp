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

void Error_(const std::string& message, const char* file, int line) {
    std::cout << TEXT_RED << "[ERROR] " << TEXT_YELLOW << message << TEXT_RESET << " | " << file << " (" << line << ")" << std::endl;
    throw std::exception();
}
#define Error(message) Error_(message, __FILE__, __LINE__)

void Log_(const std::string& message) {
    std::cout << TEXT_GREEN << "[LOG] " << TEXT_RESET << message << std::endl;
}
#define Log(message) Log_(message)

void LogOnDebug_(const std::string& message) {
    #ifdef DEBUG
    std::cout << TEXT_BLUE << "[DEBUG] " << TEXT_RESET << message << std::endl;
    #endif
}
#define LogOnDebug(message) LogOnDebug_(message)

/**
 * @brief Reads the entire contents of a file into a dynamically allocated C-string.
 *
 * This function attempts to open the given file, reads its contents,
 * and returns a pointer to a character array containing the file data.
 *
 * @param filePath The path to the file to be read.
 * @return A pointer to a character array containing the file contents,
 *         or nullptr on failure.
 */
char* ReadFile(const char* filePath)
{
    if (!filePath) Error("[ReadFile] Provided file path is null");

    if (!DoesFileExist(filePath)) Error("[ReadFile] File does not exist: '" + std::string(filePath) + "'");

    // Read the file contents
    std::ifstream file(filePath);
    if (!file) Error("[ReadFile] File could not be read: '" + std::string(filePath) + "'");

    // Get the file size
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Allocate memory for the file contents
    char* text = new char[size + 1];
    if (!text) Error("[ReadFile] Memory allocation failed for file: '" + std::string(filePath) + "'");

   // Read the file contents into the allocated memory
   file.read(text, size);
   text[size] = '\0';  // Null-terminate the C-string, making it safe for external use

   file.close();

    LogOnDebug("[ReadFile] Successfully read file: '" + std::string(filePath) + "'");

   return text;
}

bool DoesFileExist(const char* filePath)
{
    std::ifstream file(filePath);
    return file.good();
}

/**
 * @brief Frees memory previously allocated for a text buffer.
 *
 * @param text Pointer to the text buffer to be freed.
 */
void UnloadFileText(char* text) {
    if (!text) {
        LogOnDebug("[UnloadFile] Unnecessary call to unload text");
        return;
    }

    delete[] text;
}