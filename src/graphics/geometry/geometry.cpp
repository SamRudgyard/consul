#include "graphics/geometry/geometry.hpp"

#include <algorithm>
#include <cstdint>
#include <vector>

#include "core/console/console.hpp"
#include "maths/constants.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/geometric.hpp"

Mesh Geometry::cube(float width)
{
    return cuboid(width, width, width);
}

Mesh Geometry::cuboid(float width, float height, float depth)
{
    if (width <= 0.f) {
        Console::get().error("[Geometry::cuboid] Invalid width " + std::to_string(width) + ", must be +ve real.");
    }
    if (height <= 0.f) {
        Console::get().error("[Geometry::cuboid] Invalid height " + std::to_string(height) + ", must be +ve real.");
    }
    if (depth <= 0.f) {
        Console::get().error("[Geometry::cuboid] Invalid depth " + std::to_string(depth) + ", must be +ve real.");
    }

    const float halfWidth = 0.5f*width;
    const float halfHeight = 0.5f*height;
    const float halfDepth = 0.5f*depth;
    std::vector<glm::vec3> positions = {
        {  halfWidth, -halfHeight, -halfDepth }, {  halfWidth, -halfHeight,  halfDepth }, {  halfWidth,  halfHeight,  halfDepth }, {  halfWidth,  halfHeight, -halfDepth },
        { -halfWidth, -halfHeight,  halfDepth }, { -halfWidth, -halfHeight, -halfDepth }, { -halfWidth,  halfHeight, -halfDepth }, { -halfWidth,  halfHeight,  halfDepth },
        { -halfWidth,  halfHeight, -halfDepth }, {  halfWidth,  halfHeight, -halfDepth }, {  halfWidth,  halfHeight,  halfDepth }, { -halfWidth,  halfHeight,  halfDepth },
        { -halfWidth, -halfHeight,  halfDepth }, {  halfWidth, -halfHeight,  halfDepth }, {  halfWidth, -halfHeight, -halfDepth }, { -halfWidth, -halfHeight, -halfDepth },
        {  halfWidth, -halfHeight,  halfDepth }, { -halfWidth, -halfHeight,  halfDepth }, { -halfWidth,  halfHeight,  halfDepth }, {  halfWidth,  halfHeight,  halfDepth },
        { -halfWidth, -halfHeight, -halfDepth }, {  halfWidth, -halfHeight, -halfDepth }, {  halfWidth,  halfHeight, -halfDepth }, { -halfWidth,  halfHeight, -halfDepth },
    };

    std::vector<glm::vec3> normals = {
        {  1.0f,  0.0f,  0.0f }, {  1.0f,  0.0f,  0.0f }, {  1.0f,  0.0f,  0.0f }, {  1.0f,  0.0f,  0.0f },
        { -1.0f,  0.0f,  0.0f }, { -1.0f,  0.0f,  0.0f }, { -1.0f,  0.0f,  0.0f }, { -1.0f,  0.0f,  0.0f },
        {  0.0f,  1.0f,  0.0f }, {  0.0f,  1.0f,  0.0f }, {  0.0f,  1.0f,  0.0f }, {  0.0f,  1.0f,  0.0f },
        {  0.0f, -1.0f,  0.0f }, {  0.0f, -1.0f,  0.0f }, {  0.0f, -1.0f,  0.0f }, {  0.0f, -1.0f,  0.0f },
        {  0.0f,  0.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, {  0.0f,  0.0f,  1.0f }, {  0.0f,  0.0f,  1.0f },
        {  0.0f,  0.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }, {  0.0f,  0.0f, -1.0f },
    };

    std::vector<glm::vec2> uvs = {
        { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f },
        { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f },
        { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f },
        { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f },
        { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f },
        { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f },
    };

    std::vector<glm::vec4> tangents = {
        {  0.0f, 0.0f,  1.0f, 1.0f }, {  0.0f, 0.0f,  1.0f, 1.0f }, {  0.0f, 0.0f,  1.0f, 1.0f }, {  0.0f, 0.0f,  1.0f, 1.0f },
        {  0.0f, 0.0f, -1.0f, 1.0f }, {  0.0f, 0.0f, -1.0f, 1.0f }, {  0.0f, 0.0f, -1.0f, 1.0f }, {  0.0f, 0.0f, -1.0f, 1.0f },
        {  1.0f, 0.0f,  0.0f, 1.0f }, {  1.0f, 0.0f,  0.0f, 1.0f }, {  1.0f, 0.0f,  0.0f, 1.0f }, {  1.0f, 0.0f,  0.0f, 1.0f },
        {  1.0f, 0.0f,  0.0f, 1.0f }, {  1.0f, 0.0f,  0.0f, 1.0f }, {  1.0f, 0.0f,  0.0f, 1.0f }, {  1.0f, 0.0f,  0.0f, 1.0f },
        { -1.0f, 0.0f,  0.0f, 1.0f }, { -1.0f, 0.0f,  0.0f, 1.0f }, { -1.0f, 0.0f,  0.0f, 1.0f }, { -1.0f, 0.0f,  0.0f, 1.0f },
        {  1.0f, 0.0f,  0.0f, 1.0f }, {  1.0f, 0.0f,  0.0f, 1.0f }, {  1.0f, 0.0f,  0.0f, 1.0f }, {  1.0f, 0.0f,  0.0f, 1.0f },
    };

    std::vector<unsigned int> indices = {
        0, 2, 1, 0, 3, 2,
        4, 6, 5, 4, 7, 6,
        8, 10, 9, 8, 11, 10,
        12, 14, 13, 12, 15, 14,
        16, 18, 17, 16, 19, 18,
        20, 22, 21, 20, 23, 22,
    };

    // Default texture
    std::vector<Texture> textures;
    textures.push_back(Texture());

    return Mesh(positions, normals, uvs, tangents, indices, textures);
}

Mesh Geometry::sphereUV(float radius, unsigned int nLatitudes, unsigned int nLongitudes)
{
    if (radius <= 0.f) {
        Console::get().error("[Geometry::sphereUV] Invalid radius " + std::to_string(radius) + ", must be +ve real.");
    }

    nLatitudes = std::max(2u, nLatitudes);
    nLongitudes = std::max(3u, nLongitudes);
    const unsigned int vertexCount = (nLatitudes + 1)*(nLongitudes + 1);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec4> tangents;
    positions.reserve(vertexCount);
    normals.reserve(vertexCount);
    uvs.reserve(vertexCount);
    tangents.reserve(vertexCount);

    for (unsigned int iLatitude = 0; iLatitude <= nLatitudes; iLatitude++) {
        const float v = (float)(iLatitude)/(float)nLatitudes;
        const float phi = PI*v;
        const float sinPhi = std::sin(phi);
        const float cosPhi = std::cos(phi);

        for (unsigned int iLongitude = 0; iLongitude <= nLongitudes; iLongitude++) {
            const float u = (float)(iLongitude)/(float)nLongitudes;
            const float theta = TWO_PI*u;
            const float sinTheta = std::sin(theta);
            const float cosTheta = std::cos(theta);

            const float x = sinPhi*cosTheta;
            const float y = cosPhi;
            const float z = sinPhi*sinTheta;

            glm::vec3 position = {x, y, z};
            position *= radius;
            positions.push_back(position);
            normals.push_back(glm::normalize(position));
            tangents.push_back({1.0f, 0.0f, 0.0f, 1.0f});
            uvs.push_back({u, 1.0f - v});
        }
    }

    std::vector<unsigned int> indices;
        indices.reserve(6*nLatitudes*nLongitudes);
        for (unsigned int iLatitude = 0; iLatitude < nLatitudes; iLatitude++) {
            for (unsigned int iLongitude = 0; iLongitude < nLongitudes; iLongitude++) {
                const unsigned int i0 = iLatitude*(nLongitudes + 1) + iLongitude;
                const unsigned int i1 = i0 + nLongitudes + 1;
                indices.push_back(i0);
                indices.push_back(i0 + 1);
                indices.push_back(i1);
                indices.push_back(i1);
                indices.push_back(i0 + 1);
                indices.push_back(i1 + 1);
            }
        }

    // Default texture
    std::vector<Texture> textures;
    textures.push_back(Texture());

    return Mesh(positions, normals, uvs, tangents, indices, textures);
}

Mesh Geometry::sphereIcosphere(float radius, unsigned int nDivisions)
{
    if (radius < 0.f) {
        Console::get().error("[Geometry::sphereIcosphere] Invalid radius " + std::to_string(radius) + ", must be >= 0.");
    }

    const float t = GOLDEN_RATIO;

    std::vector<glm::vec3> positions = {
        {-1.0f, t, 0.0f}, {1.0f, t, 0.0f}, {-1.0f, -t, 0.0f}, {1.0f, -t, 0.0f},
        {0.0f, -1.0f, t}, {0.0f, 1.0f, t}, { 0.0f, -1.0f, -t}, {0.0f, 1.0f, -t},
        { t, 0.0f, -1.0f}, {t, 0.0f, 1.0f}, {-t, 0.0f, -1.0f}, {-t, 0.0f, 1.0f},
    };

    for (glm::vec3& position : positions) {
        position = glm::normalize(position);
    }

    std::vector<unsigned int> indices = {
        0, 11, 5,   0, 5,  1,   0,  1,  7,   0, 7, 10,   0, 10, 11,
        1,  5, 9,   5, 11, 4,  11, 10,  2,  10, 7,  6,   7,  1,  8,
        3,  9, 4,   3, 4,  2,   3,  2,  6,   3, 6,  8,   3,  8,  9,
        4,  9, 5,   2, 4, 11,   6,  2, 10,   8, 6,  7,   9,  8,  1,
    };

    auto makeKey = [](unsigned int a, unsigned int b) {
        const uint64_t minIndex = std::min(a, b);
        const uint64_t maxIndex = std::max(a, b);
        return (minIndex << 32) | maxIndex;
    };

    for (unsigned int iDivision = 0; iDivision < nDivisions; iDivision++) {
        std::unordered_map<uint64_t, unsigned int> midpointCache;
        std::vector<unsigned int> newIndices;
        newIndices.reserve(indices.size() * 4);

        auto midpoint = [&](unsigned int a, unsigned int b) {
            const uint64_t key = makeKey(a, b);
            const auto found = midpointCache.find(key);
            if (found != midpointCache.end()) {
                return found->second;
            }

            glm::vec3 mid = glm::normalize((positions[a] + positions[b]) * 0.5f);
            const unsigned int index = static_cast<unsigned int>(positions.size());
            positions.push_back(mid);
            midpointCache.emplace(key, index);
            return index;
        };

        for (size_t i = 0; i < indices.size(); i += 3) {
            const unsigned int i0 = indices[i];
            const unsigned int i1 = indices[i + 1];
            const unsigned int i2 = indices[i + 2];

            const unsigned int a = midpoint(i0, i1);
            const unsigned int b = midpoint(i1, i2);
            const unsigned int c = midpoint(i2, i0);

            newIndices.push_back(i0); newIndices.push_back(a); newIndices.push_back(c);
            newIndices.push_back(i1); newIndices.push_back(b); newIndices.push_back(a);
            newIndices.push_back(i2); newIndices.push_back(c); newIndices.push_back(b);
            newIndices.push_back(a);  newIndices.push_back(b); newIndices.push_back(c);
        }

        indices.swap(newIndices);
    }

    for (glm::vec3& position : positions) {
        position *= radius;
    }

    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec4> tangents;
    normals.reserve(positions.size());
    uvs.reserve(positions.size());
    tangents.reserve(positions.size());

    for (const glm::vec3& position : positions) {
        const glm::vec3 normal = glm::normalize(position);
        const float u = 0.5f + std::atan2(normal.z, normal.x) / TWO_PI;
        const float v = 0.5f - std::asin(normal.y) / PI;
        normals.push_back(normal);
        uvs.push_back({u, v});
        tangents.push_back({1.0f, 0.0f, 0.0f, 1.0f});
    }

    // Default texture
    std::vector<Texture> textures;
    textures.push_back(Texture());

    return Mesh(positions, normals, uvs, tangents, indices, textures);
}
