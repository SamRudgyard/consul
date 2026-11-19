#pragma once

#include "core/engine.hpp"
#include "core/profiling/scope.hpp"

#if defined(_MSC_VER)
#define CREMA_PROFILE_CURRENT_FUNCTION __FUNCSIG__
#else
#define CREMA_PROFILE_CURRENT_FUNCTION __PRETTY_FUNCTION__
#endif

#define CREMA_PROFILE_CONCAT_IMPL(x, y) x##y
#define CREMA_PROFILE_CONCAT(x, y) CREMA_PROFILE_CONCAT_IMPL(x, y)

#define CREMA_PROFILE_SCOPE(nameLiteral) auto CREMA_PROFILE_CONCAT(_cremaProfileScope_, __LINE__) = Engine::get().profiler.profileScope(nameLiteral)

#define CREMA_PROFILE_METHOD() CREMA_PROFILE_SCOPE(CREMA_PROFILE_CURRENT_FUNCTION)
