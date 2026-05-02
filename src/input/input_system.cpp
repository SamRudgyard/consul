#include "input_system.hpp"

#include "core/profiling/profiler_scope.hpp"

InputSystem::InputSystem() = default;
InputSystem::~InputSystem() = default;

void InputSystem::beginTick()
{
    CONSUL_PROFILE_METHOD();
    keyboard.beginTick();
    mouse.beginTick();
}

void InputSystem::endTick()
{
    CONSUL_PROFILE_METHOD();
    mouse.endTick();
}

void InputSystem::setExitKey(KeyboardKey key)
{
    keyboard.setExitKey(key);
}

KeyboardKey InputSystem::getExitKey() const
{
    return keyboard.getExitKey();
}

void InputSystem::setKeyDown(KeyboardKey key)
{
    keyboard.setKeyDown(key);
}

void InputSystem::setKeyUp(KeyboardKey key)
{
    keyboard.setKeyUp(key);
}

bool InputSystem::isKeyDown(KeyboardKey key) const
{
    return keyboard.isKeyDown(key);
}

bool InputSystem::isKeyPressed(KeyboardKey key) const
{
    return keyboard.isKeyPressed(key);
}

void InputSystem::setMouseButtonDown(MouseButton button)
{
    mouse.setButtonDown(button);
}

void InputSystem::setMouseButtonUp(MouseButton button)
{
    mouse.setButtonUp(button);
}

bool InputSystem::isMouseButtonDown(MouseButton button) const
{
    return mouse.isButtonDown(button);
}

bool InputSystem::isMouseButtonPressed(MouseButton button) const
{
    return mouse.isButtonPressed(button);
}

bool InputSystem::isMouseButtonUp(MouseButton button) const
{
    return mouse.isButtonUp(button);
}

glm::vec2 InputSystem::getMousePosition() const
{
    return mouse.getMousePosition();
}

void InputSystem::setMousePosition(const glm::vec2& position)
{
    mouse.setMousePosition(position);
}

glm::vec2 InputSystem::getPreviousMousePosition() const
{
    return mouse.getPreviousMousePosition();
}

void InputSystem::setMouseScrollOffset(const glm::vec2& offset)
{
    mouse.setScrollOffset(offset);
}

glm::vec2 InputSystem::getMouseScrollOffset() const
{
    return mouse.getScrollOffset();
}

bool InputSystem::isMouseInsideWindow() const
{
    return mouse.getIsInsideWindow();
}

void InputSystem::setMouseInsideWindow(bool inside)
{
    mouse.setIsInsideWindow(inside);
}

void InputSystem::setMouseVisibility(bool visible)
{
    mouse.setMouseVisibility(visible);
}

bool InputSystem::getMouseVisibility() const
{
    return mouse.getMouseVisibility();
}
