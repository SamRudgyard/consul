#pragma once

class ComponentManager {
private:
    static unsigned int nComponents;
public:
    ComponentManager() = default;
    ~ComponentManager() = default;

    template<class T>
    static unsigned int Get() {
        static unsigned int componentID = nComponents++; // Creates a new ID for a newly initialised component
        return componentID;
    }
};