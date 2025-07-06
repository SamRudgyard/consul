#pragma once

#include <cstdint>
#include <bitset>

using namespace std;

using Entity = uint32_t;
const Entity MAX_ENTITIES = 5000;
using ComponentType = uint32_t;
const ComponentType MAX_COMPONENTS = 64;
typedef bitset<MAX_COMPONENTS> ComponentMask;