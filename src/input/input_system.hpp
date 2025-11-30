#pragma once

#include "keyboard_state.hpp"
#include "mouse_state.hpp"

class InputSystem
{
public:
    InputSystem(const InputSystem&) = delete;
    InputSystem& operator=(const InputSystem&) = delete;
    
    /**
     * Get the singleton InputSystem instance.
     * @returns The single InputSystem instance.
     */
    static InputSystem& get() {
        static InputSystem instance;
        return instance;
    }

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

    void setKeyDown(int keyCode)
    {
        keyboard.setKeyDown(keyCode);
    }

    void setKeyUp(int keyCode)
    {
        keyboard.setKeyUp(keyCode);
    }

    bool isKeyDown(int keyCode) const
    {
        return keyboard.isKeyDown(keyCode);
    }

    bool isKeyPressed(int keyCode) const
    {
        return keyboard.isKeyPressed(keyCode);
    }

    void setMouseButtonDown(int buttonCode)
    {
        mouse.setButtonDown(buttonCode);
    }

    void setMouseButtonUp(int buttonCode)
    {
        mouse.setButtonUp(buttonCode);
    }

    bool isMouseButtonDown(int buttonCode) const
    {
        return mouse.isButtonDown(buttonCode);
    }

    bool isMouseButtonPressed(int buttonCode) const
    {
        return mouse.isButtonPressed(buttonCode);
    }

    bool isMouseButtonUp(int buttonCode) const
    {
        return mouse.isButtonUp(buttonCode);
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

    void setMouseInsideWindow(bool inside)
    {
        mouse.setIsInsideWindow(inside);
    }

protected:
    InputSystem() = default;
    ~InputSystem() = default;

private:
    KeyboardState keyboard;
    MouseState mouse;
};