#include "utils.hpp"

#include "glad/glad.h"
#include "core/console/console.hpp"

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
 * @return A string containing the file contents.
 */
const std::string ReadFile(const char* filePath)
{
    if (!filePath) Console::Get().Error("[ReadFile] Provided file path is null");

    if (!DoesFileExist(filePath)) Console::Get().Error("[ReadFile] File does not exist: '" + std::string(filePath) + "'");

    // Read the file contents
    std::ifstream file(filePath, std::ios::binary);
    if (!file) Console::Get().Error("[ReadFile] File could not be read: '" + std::string(filePath) + "'");

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string contents = buffer.str();

    Console::Get().LogOnDebug("[ReadFile] Successfully read file: '" + std::string(filePath) + "'");

    return contents;
}

/**
 * @brief Frees memory previously allocated for a text buffer.
 *
 * @param text Pointer to the text buffer to be freed.
 */
void UnloadFileText(char* text) {
    if (!text) {
        Console::Get().LogOnDebug("[UnloadFile] Unnecessary call to unload text");
        return;
    }

    delete[] text;
}

bool IsSubstring(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

void checkOpenGLErrors(const std::string& msgOnError) {
    GLenum error = glGetError();
    if (error == GL_NO_ERROR) return;

    Console& console = Console::Get();

    std::string errorStr;

    switch (error) {
        case GL_INVALID_ENUM:
            errorStr = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            errorStr = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            errorStr = "GL_INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            errorStr = "GL_STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            errorStr = "GL_STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            errorStr = "GL_OUT_OF_MEMORY";
            break;
        default:
            console.Error(msgOnError + ": Unknown OpenGL error code " + std::to_string(error));
            break;
    }

    console.Error(msgOnError + ": " + errorStr);
}