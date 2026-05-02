#include "utils.hpp"

#include <thread>
#include <chrono>

#include "glad/glad.h"
#include "core/console/console.hpp"
#include "core/profiling/profiler_scope.hpp"

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
    CONSUL_PROFILE_METHOD();
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
    CONSUL_PROFILE_METHOD();
    if (seconds < 0) return;    // Security check

    // System halt functions
    #if defined(_WIN32)
        Sleep((unsigned long)(seconds*SECONDS_TO_MILLISECONDS));
    #endif
    #if defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__EMSCRIPTEN__)
        struct timespec req = { 0 };
        time_t sec = seconds;
        long nsec = (seconds - sec)*1000000000L;
        req.tv_sec = sec;
        req.tv_nsec = nsec;

        // NOTE: Use nanosleep() on Unix platforms... usleep() it's deprecated
        while (nanosleep(&req, &req) == -1) continue;
    #endif
    #if defined(__APPLE__)
        usleep(seconds*SECONDS_TO_MICROSECONDS);
    #endif
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
