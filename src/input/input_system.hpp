#pragma once

#include "keyboard_state.hpp"
#include "mouse_state.hpp"

class InputSystem
{
public:
    InputSystem() = default;
    ~InputSystem() = default;

    /**
     * Begin a new frame for input state tracking.
     * Updates last frame states to current states.
     */
    void beginTick()
    {   
        keyboard.beginTick();
        mouse.beginTick();
    }

    void setExitKey(KeyboardKey key)
    {
        keyboard.setExitKey(key);
    }

    KeyboardKey getExitKey() const
    {
        return keyboard.getExitKey();
    }

    void setKeyDown(KeyboardKey key)
    {
        keyboard.setKeyDown(key);
    }

    void setKeyUp(KeyboardKey key)
    {
        keyboard.setKeyUp(key);
    }

    bool isKeyDown(KeyboardKey key) const
    {
        return keyboard.isKeyDown(key);
    }

    bool isKeyPressed(KeyboardKey key) const
    {
        return keyboard.isKeyPressed(key);
    }

    void setMouseButtonDown(MouseButton button)
    {
        mouse.setButtonDown(button);
    }

    void setMouseButtonUp(MouseButton button)
    {
        mouse.setButtonUp(button);
    }

    bool isMouseButtonDown(MouseButton button) const
    {
        return mouse.isButtonDown(button);
    }

    bool isMouseButtonPressed(MouseButton button) const
    {
        return mouse.isButtonPressed(button);
    }

    bool isMouseButtonUp(MouseButton button) const
    {
        return mouse.isButtonUp(button);
    }

    glm::vec2 getMousePosition() const
    {
        return mouse.getMousePosition();
    }

    void setMousePosition(const glm::vec2& position)
    {
        mouse.setMousePosition(position);
    }

    void setMouseScrollOffset(const glm::vec2& offset)
    {
        mouse.setScrollOffset(offset);
    }

    glm::vec2 getMouseScrollOffset() const
    {
        return mouse.getScrollOffset();
    }

    bool isMouseInsideWindow() const
    {
        return mouse.getIsInsideWindow();
    }

    void setMouseInsideWindow(const bool inside)
    {
        mouse.setIsInsideWindow(inside);
    }

    void setMouseVisibility(const bool visible)
    {
        mouse.setMouseVisibility(visible);
    }

    bool getMouseVisibility() const
    {
        return mouse.getMouseVisibility();
    }

private:
    KeyboardState keyboard;
    MouseState mouse;
};