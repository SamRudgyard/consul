#pragma once

#include <cstdint>
#include <cstddef>
#include <functional>
#include <string>

class AssetID
{
public:
    AssetID();
    AssetID(std::uint64_t id);
    AssetID(const AssetID& other);
    ~AssetID() = default;

    bool operator==(const AssetID& other) const { return id == other.id; }
    bool operator!=(const AssetID& other) const { return id != other.id; }
    operator std::uint64_t() const { return id; }

    std::string toString() const { return std::to_string(id); }

private:
    std::uint64_t id = 0;
};

static const AssetID INVALID_ASSET_ID = AssetID(0); // Reserve ID 0 for invalid AssetID

struct AssetMetadata
{
    std::string name;
    std::string sourcePath;
};

// Custom hash function for AssetID, allowing it to be used as a key in std::unordered_map
namespace std
{
    template<>
    struct hash<AssetID>
    {
        std::size_t operator()(const AssetID& id) const
        {
            // Hash to the underlying uint64_t value of the AssetID
            return std::hash<std::uint64_t>()(static_cast<std::uint64_t>(id));
        }
    };
}
