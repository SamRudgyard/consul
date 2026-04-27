#pragma once

#include <cstddef>

#include "core/engine_context.hpp"
#include "core/profiling/scope.hpp"

#if defined(_MSC_VER)
#define CONSUL_PROFILE_CURRENT_FUNCTION __FUNCSIG__
#else
#define CONSUL_PROFILE_CURRENT_FUNCTION __PRETTY_FUNCTION__
#endif

#define CONSUL_PROFILE_CONCAT_IMPL(x, y) x##y
#define CONSUL_PROFILE_CONCAT(x, y) CONSUL_PROFILE_CONCAT_IMPL(x, y)

#define CONSUL_PROFILE_SCOPE(nameLiteral)                                                                          \
    static const std::size_t CONSUL_PROFILE_CONCAT(_consulProfileMethodIndex_, __LINE__) =                       \
        EngineContext::get()->profiler.registerMethod(nameLiteral);                                               \
    auto CONSUL_PROFILE_CONCAT(_consulProfileScope_, __LINE__) =                                                  \
        EngineContext::get()->profiler.profileScope(CONSUL_PROFILE_CONCAT(_consulProfileMethodIndex_, __LINE__))

#define CONSUL_PROFILE_METHOD() CONSUL_PROFILE_SCOPE(CONSUL_PROFILE_CURRENT_FUNCTION)
