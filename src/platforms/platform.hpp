#pragma once

#include "core/window_config.hpp"

class IPlatform
{
public:
    IPlatform(WindowConfig* config) : windowConfig(config) {}

    virtual void initialiseWindow() = 0;

protected:
    WindowConfig* windowConfig;
};