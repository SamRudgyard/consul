#pragma once

#include "entity_manager.hpp"

class IRunUpdateSystem {
public:
    virtual void Update(float deltaTime) = 0;
};

class IPauseUpdateSystem {
public:
    virtual void Update(float deltaTime) = 0;
};

class IRenderSystem {
public:
    virtual void Render() = 0;
};

class SystemManager {
private:
    static SystemManager* instance;

    vector<IRunUpdateSystem*> runUpdateSystems;
    vector<IPauseUpdateSystem*> pauseUpdateSystems;
    vector<IRenderSystem*> renderSystems;
protected:
    SystemManager() = default;
    ~SystemManager() = default;
public:
    SystemManager(SystemManager const&) = delete; // Delete copy constructor
    void operator=(SystemManager const&) = delete; // Delete copy assignment

    static SystemManager* GetInstance() {
        if (instance == nullptr) instance = new SystemManager();
        return instance;
    }

    void AddRunUpdateSystem(IRunUpdateSystem* system) { runUpdateSystems.push_back(system); }
    void AddPauseUpdateSystem(IPauseUpdateSystem* system) { pauseUpdateSystems.push_back(system); }
    void AddRenderSystem(IRenderSystem* system) { renderSystems.push_back(system); }
    
    void RunUpdate(float deltaTime) {
        for (auto system : pauseUpdateSystems) {
            system->Update(deltaTime);
        }

        for (auto system : runUpdateSystems) {
            system->Update(deltaTime);
        }
    }

    void PauseUpdate(float deltaTime) {
        for (auto system : pauseUpdateSystems) {
            system->Update(deltaTime);
        }
    }

    void Render() {
        for (auto system : renderSystems) {
            system->Render();
        }
    }
};