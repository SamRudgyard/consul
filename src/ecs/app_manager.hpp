#pragma once

enum class AppState {
    Startup,
    Loading,
    Menu,
    Paused,
    Running,
    GameOver,
    Quitting
};

class AppManager {
private:
    static AppManager* instance;
protected:
    AppManager() = default;
    ~AppManager() = default;
public:
    AppState state = AppState::Startup;

    AppManager(AppManager const&) = delete; // Delete copy constructor
    void operator=(AppManager const&) = delete; // Delete copy assignment

    static AppManager* GetInstance() {
        if (instance == nullptr) instance = new AppManager();
        return instance;
    }
};