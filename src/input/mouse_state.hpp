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
    void beginTick()
    {
        lastFrameState = currentState;
    }

    void endTick()
    {
        previousPosition = position;
        scrollOffset = glm::vec2(0.f, 0.f);
    }

    void setButtonDown(MouseButton button)
    {
        currentState.set((size_t)button);
    }

    void setButtonUp(MouseButton button)
    {
        currentState.reset((size_t)button);
    }

    bool isButtonDown(MouseButton button) const
    {
        return currentState.test((size_t)button);
    }

    bool isButtonUp(MouseButton button) const
    {
        return !currentState.test((size_t)button);
    }

    bool wasButtonDown(MouseButton button) const
    {
        return lastFrameState.test((size_t)button);
    }

    bool wasButtonUp(MouseButton button) const
    {
        return !lastFrameState.test((size_t)button);
    }

    bool isButtonPressed(MouseButton button) const
    {
        return isButtonDown(button) && !wasButtonDown(button);
    }

    glm::vec2 getMousePosition() const
    {
        return position;
    }

    void setMousePosition(const glm::vec2& pos)
    {
        position = pos;
    }

    glm::vec2 getPreviousMousePosition() const
    {
        return previousPosition;
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

    void setIsInsideWindow(const bool inside)
    {
        isInsideWindow = inside;
    }

    void setMouseVisibility(const bool visible)
    {
        isVisible = visible;
    }

    bool getMouseVisibility() const
    {
        return isVisible;
    }

private:
    std::bitset<MAX_MOUSE_BUTTONS> currentState;
    std::bitset<MAX_MOUSE_BUTTONS> lastFrameState;
    glm::vec2 position = glm::vec2(0.f, 0.f);
    glm::vec2 previousPosition = glm::vec2(0.f, 0.f);
    glm::vec2 scrollOffset = glm::vec2(0.f, 0.f);
    bool isVisible = true;
    bool isInsideWindow = true;
};