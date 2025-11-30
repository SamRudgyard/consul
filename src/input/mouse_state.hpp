#pragma once

#include <bitset>
#include "glm/glm.hpp"

#define MAX_MOUSE_BUTTONS 8

enum class MouseButton
{
    BUTTON_LEFT     = 0,    // Left mouse button
    BUTTON_RIGHT    = 1,    // Right mouse button
    BUTTON_MIDDLE   = 2,    // Middle mouse button (wheel click)
    BUTTON_EXTRA_1  = 3,    // Extra mouse button 1
    BUTTON_EXTRA_2  = 4,    // Extra mouse button 2
    BUTTON_EXTRA_3  = 5,    // Extra mouse button 3
    BUTTON_EXTRA_4  = 6,    // Extra mouse button 4
    BUTTON_EXTRA_5  = 7     // Extra mouse button 5
};

class MouseState
{
public:
    void beginNewFrame()
    {
        lastFrameState = currentState;
    }

    void setButtonDown(int buttonCode)
    {
        currentState.set(buttonCode);
    }

    void setButtonUp(int buttonCode)
    {
        currentState.reset(buttonCode);
    }

    bool isButtonDown(int buttonCode) const
    {
        return currentState.test(buttonCode);
    }

    bool isButtonUp(int buttonCode) const
    {
        return !currentState.test(buttonCode);
    }

    bool wasButtonDown(int buttonCode) const
    {
        return lastFrameState.test(buttonCode);
    }

    bool wasButtonUp(int buttonCode) const
    {
        return !lastFrameState.test(buttonCode);
    }

    bool isButtonPressed(int buttonCode) const
    {
        return isButtonDown(buttonCode) && !wasButtonDown(buttonCode);
    }

    glm::vec2 getMousePosition() const
    {
        return position;
    }

    void setMousePosition(const glm::vec2& pos)
    {
        position = pos;
    }

    glm::vec2 getScrollOffset() const
    {
        return scrollOffset;
    }

    void setScrollOffset(const glm::vec2& offset)
    {
        scrollOffset = offset;
    }

    bool getIsInsideWindow() const
    {
        return isInsideWindow;
    }

    void setIsInsideWindow(bool inside)
    {
        isInsideWindow = inside;
    }

private:
    std::bitset<MAX_MOUSE_BUTTONS> currentState;
    std::bitset<MAX_MOUSE_BUTTONS> lastFrameState;
    glm::vec2 position = glm::vec2(0.f, 0.f);
    glm::vec2 scrollOffset = glm::vec2(0.f, 0.f);
    bool isInsideWindow = true;
};