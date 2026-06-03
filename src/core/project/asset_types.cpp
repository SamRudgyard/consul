#include "core/project/asset_types.hpp"

#include <limits>
#include <random>

// Initialize the random number generator for AssetID generation
static std::random_device randomDevice;
static std::mt19937_64 randomEngine(randomDevice());
static std::uniform_int_distribution<std::uint64_t> distribution(1, std::numeric_limits<std::uint64_t>::max());

AssetID::AssetID()
    : id(distribution(randomEngine))
{
}

AssetID::AssetID(std::uint64_t id)
    : id(id)
{
}

AssetID::AssetID(const AssetID& other)
    : id(other.id)
{
}
