#include "utils.hpp"

void Error_(const std::string& message, const char* file, int line)
{
    std::cout << TEXT_RED << "[ERROR] " << TEXT_YELLOW << message << TEXT_RESET << " | " << file << " (" << line << ")" << std::endl;
    throw std::exception();
}

void Log_(const std::string& message)
{
    std::cout << TEXT_GREEN << "[LOG] " << TEXT_RESET << message << std::endl;
}

void LogOnDebug_(const std::string& message)
{
    #ifndef NDEBUG
    std::cout << TEXT_BLUE << "[DEBUG] " << TEXT_RESET << message << std::endl;
    #endif
}

bool DoesFileExist(const char* filePath)
{
    std::ifstream file(filePath);
    return file.good();
}

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