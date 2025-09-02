#include "time.hpp"

double Time::currentTime = 0.0;
double Time::previousTime = 0.0;
double Time::deltaTime = 0.0;
double Time::frameTime = 0.0;
double Time::targetFrameTime = 0.0;
int Time::frameCount = 0;