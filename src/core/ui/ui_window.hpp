#pragma once

class UIWindow
{
public:
    virtual ~UIWindow() = default;

    virtual void update() = 0;
};
