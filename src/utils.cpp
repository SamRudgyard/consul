#include "utils.hpp"

#include "glad/glad.h"
#include "core/console/console.hpp"

bool DoesFileExist(const char* filePath)
{
    std::ifstream file(filePath);
    return file.good();
}

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

const std::string GetFileExtension(const char* filePath) {
    std::string pathStr(filePath);
    size_t dotPos = pathStr.find_last_of('.');
    if (dotPos == std::string::npos || dotPos == pathStr.length() - 1) {
        Console::Get().LogOnDebug("[GetFileExtension] No file extension found in path: '" + std::string(filePath) + "'");
        return "";
    }
    return pathStr.substr(dotPos);
}

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

void glCheckError_(const char* file, int line) {
    Console& console = Console::Get();
    
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            default:                               error = "UNKNOWN_ERROR"; break;
        }
        console.Error("[OpenGL] " + error + " at " + std::string(file) + " (line " + std::to_string(line) + ")");
    }
}