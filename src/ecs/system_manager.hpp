#pragma once

#include "entity_manager.hpp"

class IUpdateSystem {
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

    vector<IUpdateSystem*> updateSystems;
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

    void AddUpdateSystem(IUpdateSystem* system) { updateSystems.push_back(system); }
    void AddRenderSystem(IRenderSystem* system) { renderSystems.push_back(system); }
    void Update(float deltaTime) {
        for (auto system : updateSystems) {
            system->Update(deltaTime);
        }
    }
    void Render() {
        for (auto system : renderSystems) {
            system->Render();
        }
    }
};