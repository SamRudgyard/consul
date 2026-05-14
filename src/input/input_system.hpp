#pragma once

#include "keyboard_state.hpp"
#include "mouse_state.hpp"

class InputSystem
{
public:
    InputSystem();
    ~InputSystem();

    /**
     * Begin a new frame for input state tracking.
     * Updates last frame states to current states.
     */
    void beginTick();

    void endTick();

    void setExitKey(KeyboardKey key);
    KeyboardKey getExitKey() const;

    void setKeyDown(KeyboardKey key);
    void setKeyUp(KeyboardKey key);
    bool isKeyDown(KeyboardKey key) const;
    bool isKeyPressed(KeyboardKey key) const;

    void setMouseButtonDown(MouseButton button);
    void setMouseButtonUp(MouseButton button);
    bool isMouseButtonDown(MouseButton button) const;
    bool isMouseButtonPressed(MouseButton button) const;
    bool isMouseButtonUp(MouseButton button) const;

    glm::vec2 getMousePosition() const;
    void setMousePosition(const glm::vec2& position);
    glm::vec2 getPreviousMousePosition() const;
    void setMouseScrollOffset(const glm::vec2& offset);
    glm::vec2 getMouseScrollOffset() const;

    bool isMouseInsideWindow() const;
    void setMouseInsideWindow(bool inside);
    void setMouseVisibility(bool visible);
    bool getMouseVisibility() const;

private:
    KeyboardState keyboard;
    MouseState mouse;
};
