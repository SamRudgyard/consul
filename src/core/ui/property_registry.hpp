#pragma once

#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "glm/glm.hpp"
#include "graphics/colour.hpp"

enum class PropertyType
{
    INT,
    FLOAT,
    VEC2,
    VEC3,
    VEC4,
    COLOUR,
    MAT4
};

using PropertyValue = std::variant<
    float,
    int,
    glm::vec2,
    glm::vec3,
    glm::vec4,
    Colour,
    glm::mat4
>;

struct PropertyInfo {
    std::string className;
    PropertyType type;
    std::string label;

    PropertyInfo(const std::string& className, PropertyType propertyType, const std::string& propertyLabel)
        : className(className), type(propertyType), label(propertyLabel)
    {}
};

struct RegisteredProperty {
    PropertyInfo propertyInfo;
    std::function<PropertyValue(void*)> getter;
    std::function<void(void*, const PropertyValue&)> setter;
};

class PropertyRegistry
{
public:
    /**
     * Gets the singleton instance of the PropertyRegistry.
     * @returns Reference to the PropertyRegistry instance.
     */
    static PropertyRegistry& get()
    {
        static PropertyRegistry instance;
        return instance;
    }

    /**
     * Registers a property for a given class, with the specified getter and setter functions.
     * @tparam Owner The class that owns the property.
     * @tparam Value The value that the getter accepts and the setter returns.
     * @param propertyInfo Information about the property.
     * @param getter Pointer to the getter function.
     * @param setter Pointer to the setter function.
     */
    template<typename Owner, typename Value>
    void registerProperty(PropertyInfo propertyInfo, Value (Owner::*getter)() const, void (Owner::*setter)(Value))
    {
        registerPropertyForOwner<Owner>(std::move(propertyInfo), getter, setter);
    }

    /**
     * Gets the registered properties for a given class name.
     * @param className The name of the class to get properties for.
     * @returns Reference to the vector of registered properties.
     */
    const std::vector<RegisteredProperty>& getProperties(const std::string& className) const
    {
        static const std::vector<RegisteredProperty> emptyProperties;
        auto it = properties.find(className);
        return it != properties.end() ? it->second : emptyProperties;
    }

private:

    template<typename Owner, typename Value>
    void registerPropertyForOwner(PropertyInfo propertyInfo, Value (Owner::*getter)() const, void (Owner::*setter)(Value))
    {
        RegisteredProperty property = {
            std::move(propertyInfo),
            [getter](void* instance) -> PropertyValue {
                auto* owner = (Owner*)(instance);
                return (owner->*getter)();
            },
            [setter](void* instance, const PropertyValue& value) {
                auto* owner = (Owner*)(instance);
                (owner->*setter)(std::get<Value>(value));
            }
        };
        properties[propertyInfo.className].push_back(property);
    }

    std::unordered_map<std::string, std::vector<RegisteredProperty>> properties;
};
