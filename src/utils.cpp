#include "utils.hpp"

#include <thread>
#include <chrono>

#include "glad/glad.h"
#include "core/console/console.hpp"

#include "maths/unit_conversions.hpp"

#if defined(__APPLE__)
#include <unistd.h>
#endif

bool doesFileExist(const char* filePath)
{
    std::ifstream file(filePath);
    return file.good();
}

const std::string readFile(const char* filePath)
{
    if (!filePath) Console::get().error("[readFile] Provided file path is null");

    if (!doesFileExist(filePath)) Console::get().error("[readFile] File does not exist: '" + std::string(filePath) + "'");

    // Read the file contents
    std::ifstream file(filePath, std::ios::binary);
    if (!file) Console::get().error("[readFile] File could not be read: '" + std::string(filePath) + "'");

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string contents = buffer.str();

    Console::get().logOnDebug("[readFile] Successfully read file: '" + std::string(filePath) + "'");

    return contents;
}

const std::string getFileExtension(const char* filePath) {
    std::string pathStr(filePath);
    size_t dotPos = pathStr.find_last_of('.');
    if (dotPos == std::string::npos || dotPos == pathStr.length() - 1) {
        Console::get().logOnDebug("[getFileExtension] No file extension found in path: '" + std::string(filePath) + "'");
        return "";
    }
    return pathStr.substr(dotPos);
}

void unloadFileText(char* text) {
    if (!text) {
        Console::get().logOnDebug("[unloadFileText] Unnecessary call to unload text");
        return;
    }

    delete[] text;
}

bool isSubstring(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

void waitTime(double seconds)
{
    if (seconds <= 0.0) {
        return;
    }
    std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
}

void glCheckError_(const char* file, int line) {
#ifndef NDEBUG
    Console& console = Console::get();
    
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
        console.error("[OpenGL] " + error + " at " + std::string(file) + " (line " + std::to_string(line) + ")");
    }
#endif
}
