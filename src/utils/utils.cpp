#include "utils.hpp"

namespace utils {

void Error_(const string& message, const char* file, int line) {
    cout << TEXT_RED << "Error: " << TEXT_YELLOW << message << TEXT_RESET << " | " << file << " (" << line << ")" << endl;
    throw exception();
}

void Log_(const std::string& message) {
    cout << TEXT_GREEN << "LOG: " << message << TEXT_RESET << endl;
}

} // namespace utils