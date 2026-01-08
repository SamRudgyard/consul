#include "graphics/geometry/geometry.hpp"

#include <algorithm>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <cmath>

#include "core/console/console.hpp"
#include "maths/constants.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/geometric.hpp"

Mesh Geometry::capsule(float radius, float height, unsigned int nLatitudes, unsigned int nLongitudes)
{
    // Capsule = cylinder of height (height - 2r) + two hemispheres of radius r.
    // Here, "height" is total end-to-end height of the capsule (including hemispheres).
    // If height <= 2r, it degenerates to a sphere of radius r.

    constexpr float eps = 1e-6f;

    if (radius <= 0.f) {
        Console::get().error("[Geometry::capsule] Invalid radius " + std::to_string(radius) + ", must be +ve real.");
    }
    if (height <= 0.f) {
        Console::get().error("[Geometry::capsule] Invalid height " + std::to_string(height) + ", must be +ve real.");
    }

    nLatitudes  = std::max(2u, nLatitudes);   // hemispheres will use at least 2 stacks
    nLongitudes = std::max(3u, nLongitudes);  // at least a triangle around

    const float halfHeight = 0.5f * height;
    const float halfCyl = std::max(0.0f, halfHeight - radius); // half of cylinder section height
    const bool hasCylinder = halfCyl > eps;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec4> tangents;
    std::vector<unsigned int> indices;

    // Precompute
    std::vector<float> cosThetas(nLongitudes + 1), sinThetas(nLongitudes + 1);
    for (unsigned int i = 0; i <= nLongitudes; ++i) {
        float theta = TWO_PI * float(i) / float(nLongitudes);
        cosThetas[i] = std::cos(theta);
        sinThetas[i] = std::sin(theta);
    }

    auto pushVertex = [&](const glm::vec3& p, const glm::vec3& n, const glm::vec2& uv, const glm::vec4& t) {
        positions.push_back(p);
        normals.push_back(n);
        uvs.push_back(uv);
        tangents.push_back(t);
    };

    auto addQuad = [&](unsigned int i0, unsigned int i1, unsigned int i2, unsigned int i3) {
        indices.push_back(i0);
        indices.push_back(i1);
        indices.push_back(i2);
        indices.push_back(i2);
        indices.push_back(i1);
        indices.push_back(i3);
    };

    auto vFromY = [&](float y) -> float {
        float v = (y + halfHeight) / std::max(height, eps); // 0..1
        return 1.0f - v;
    };

    // Helper to emit a ring at given y with a given ring radius and normal function
    auto emitRing = [&](float y, float ringRadius, const glm::vec3& centerOffset, bool normalsFromSphereCenter) {
        const unsigned int ringStart = (unsigned int)positions.size();
        for (unsigned int iLon = 0; iLon <= nLongitudes; ++iLon) {
            float u = float(iLon) / float(nLongitudes);
            float c = cosThetas[iLon];
            float s = sinThetas[iLon];

            glm::vec3 pos(ringRadius * c, y, ringRadius * s);

            glm::vec3 n;
            if (normalsFromSphereCenter) {
                // For hemispheres: normal points away from the sphere center
                glm::vec3 fromCenter = pos - centerOffset;
                n = glm::normalize(fromCenter);
            } else {
                // For cylinder: purely radial
                n = glm::normalize(glm::vec3(c, 0.f, s));
            }

            glm::vec3 tangent3 = glm::normalize(glm::vec3(-s, 0.f, c));
            glm::vec4 tangent(tangent3, 1.f);

            pushVertex(pos, n, glm::vec2(u, vFromY(y)), tangent);
        }
        return ringStart;
    };

    // ==================================================
    // 1) Top hemisphere (including equator ring at y=+halfCyl)
    // ==================================================
    // Parameterize hemisphere using phi from 0 (north pole) to PI/2 (equator).
    const unsigned int hemiStacks = nLatitudes;
    const unsigned int topStacks = hemiStacks / 2; // number of "cells" in top hemi
    const glm::vec3 topCenter(0.f, +halfCyl, 0.f);

    std::vector<unsigned int> ringStarts;

    // rings: i = 0..topStacks (inclusive)
    for (unsigned int i = 0; i <= topStacks; ++i) {
        float t = float(i) / float(topStacks); // 0..1
        float phi = (PI * 0.5f) * t;           // 0..PI/2
        float sinPhi = std::sin(phi);
        float cosPhi = std::cos(phi);

        float y = topCenter.y + radius * cosPhi;
        float ringR = radius * sinPhi;

        ringStarts.push_back(emitRing(y, ringR, topCenter, true));
    }

    // indices for top hemisphere
    for (unsigned int i = 0; i < topStacks; ++i) {
        unsigned int aStart = ringStarts[i];
        unsigned int bStart = ringStarts[i + 1];
        for (unsigned int j = 0; j < nLongitudes; ++j) {
            unsigned int i0 = aStart + j;
            unsigned int i1 = aStart + j + 1;
            unsigned int i2 = bStart + j;
            unsigned int i3 = bStart + j + 1;
            addQuad(i0, i1, i2, i3);
        }
    }

    // ==================================================
    // 2) Cylinder section (optional): connect equator rings with radial normals
    // ==================================================
    unsigned int cylTopRingStart = 0;
    unsigned int cylBotRingStart = 0;

    if (hasCylinder) {
        // Duplicate the equator rings for the cylinder so the normals are radial (hard edge).
        cylTopRingStart = emitRing(+halfCyl, radius, glm::vec3(0.f), false);
        cylBotRingStart = emitRing(-halfCyl, radius, glm::vec3(0.f), false);

        for (unsigned int j = 0; j < nLongitudes; ++j) {
            unsigned int i0 = cylTopRingStart + j;
            unsigned int i1 = cylTopRingStart + j + 1;
            unsigned int i2 = cylBotRingStart + j;
            unsigned int i3 = cylBotRingStart + j + 1;
            addQuad(i0, i1, i2, i3);
        }
    } else {
        // No cylinder: the capsule is a sphere; the "equator" is at y=0 and already present.
        // We'll just treat the bottom hemisphere as continuing from the last top ring.
    }

    // ==================================================
    // 3) Bottom hemisphere (including equator ring at y=-halfCyl)
    // ==================================================
    const unsigned int bottomStacks = hemiStacks - topStacks; // remaining cells
    const glm::vec3 botCenter(0.f, -halfCyl, 0.f);

    std::vector<unsigned int> botRingStarts;
    // rings: i = 0..bottomStacks (inclusive)
    // Parameterize phi from PI/2 (equator) to PI (south pole).
    for (unsigned int i = 0; i <= bottomStacks; ++i) {
        float t = float(i) / float(bottomStacks);       // 0..1
        float phi = (PI * 0.5f) + (PI * 0.5f) * t;      // PI/2..PI
        float sinPhi = std::sin(phi);
        float cosPhi = std::cos(phi);

        float y = botCenter.y + radius * cosPhi;
        float ringR = radius * sinPhi;

        botRingStarts.push_back(emitRing(y, ringR, botCenter, true));
    }

    // indices for bottom hemisphere
    for (unsigned int i = 0; i < bottomStacks; ++i) {
        unsigned int aStart = botRingStarts[i];
        unsigned int bStart = botRingStarts[i + 1];
        for (unsigned int j = 0; j < nLongitudes; ++j) {
            unsigned int i0 = aStart + j;
            unsigned int i1 = aStart + j + 1;
            unsigned int i2 = bStart + j;
            unsigned int i3 = bStart + j + 1;
            addQuad(i0, i1, i2, i3);
        }
    }

    // ==================================================
    // 4) Stitching between parts
    // ==================================================
    // We intentionally duplicated the equator rings when hasCylinder==true to get correct normals:
    // - Top hemisphere equator ring (sphere normals) is already connected to top hemi.
    // - Cylinder rings (radial normals) are connected to each other.
    // - Bottom hemisphere equator ring (sphere normals) is connected to bottom hemi.
    // There is no need to stitch by indices because they are separate vertex rings with different normals.

    // Default texture
    std::vector<Texture> textures;
    textures.emplace_back(Texture());

    return Mesh(positions, normals, uvs, tangents, indices, textures);
}

Mesh Geometry::cube(float width)
{
    return cuboid(width, width, width);
}

Mesh Geometry::cone(float radius, float height, unsigned int sides)
{
    return cylinder(0.f, radius, height, sides);
}

Mesh Geometry::cylinder(float radiusTop, float radiusBottom, float height, unsigned int sides)
{
    constexpr float eps = 1e-6f;

    if (radiusTop < 0.f) {
        Console::get().error("[Geometry::cylinder] Invalid radiusTop " + std::to_string(radiusTop) + ", must be >= 0.");
    }
    if (radiusBottom <= 0.f) {
        Console::get().error("[Geometry::cylinder] Invalid radiusBottom " + std::to_string(radiusBottom) + ", must be +ve real.");
    }
    if (height <= 0.f) {
        Console::get().error("[Geometry::cylinder] Invalid height " + std::to_string(height) + ", must be +ve real.");
    }

    sides = std::max(3u, sides); // We require at least 3 sides
    const float halfHeight = 0.5f*height;
    const float yTop = halfHeight;
    const float yBottom = -halfHeight;
    const float slope = (radiusBottom - radiusTop)/height;
    const bool hasTopSurface = radiusTop > eps;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec4> tangents;
    std::vector<unsigned int> indices;

    // Precompute angles
    std::vector<float> cosThetas(sides + 1), sinThetas(sides + 1);
    for (unsigned int iSide = 0; iSide <= sides; iSide++) {
        float theta = TWO_PI*float(iSide)/float(sides);
        cosThetas[iSide] = std::cos(theta);
        sinThetas[iSide] = std::sin(theta);
    }

    // ==================================================
    // 1) Side surfaces
    // ==================================================
    const unsigned int sideStart = (unsigned int)positions.size();

    for (unsigned int iSide = 0; iSide <= sides; iSide++) {
        const float c = cosThetas[iSide];
        const float s = sinThetas[iSide];

        const glm::vec3 normal = glm::normalize(glm::vec3(c, slope, s));
        const glm::vec3 tangent3 = glm::normalize(glm::vec3(-s, 0.f, c));
        const glm::vec4 tangent(tangent3, 1.f);

        const float u = float(iSide)/float(sides);

        // top (or apex if radiusTop == 0)
        positions.emplace_back(radiusTop*c, yTop, radiusTop*s);
        normals.push_back(normal);
        uvs.emplace_back(u, 1.f);
        tangents.push_back(tangent);

        // bottom
        positions.emplace_back(radiusBottom*c, yBottom, radiusBottom*s);
        normals.push_back(normal);
        uvs.emplace_back(u, 0.f);
        tangents.push_back(tangent);
    }

    // Side indices (CCW from outside)
    for (unsigned int iSide = 0; iSide < sides; iSide++) {
        // Note that we construct the sides from "inside" its surface,
        // hence indices are CW here so they are CCW from "outside".
        const unsigned int topL = sideStart + 2u*iSide;
        const unsigned int botL = topL + 1u;
        const unsigned int topR = sideStart + 2u*(iSide + 1u);
        const unsigned int botR = topR + 1u;

        indices.push_back(topL);
        indices.push_back(topR);
        indices.push_back(botL);

        indices.push_back(topR);
        indices.push_back(botR);
        indices.push_back(botL);
    }

    // ==================================================
    // 2) Top Surface
    // ==================================================
    if (hasTopSurface) {
        const unsigned int capStart = (unsigned int)positions.size();

        // center
        positions.emplace_back(0.f, yTop, 0.f);
        normals.emplace_back(0.f, 1.f, 0.f);
        uvs.emplace_back(0.5f, 0.5f);
        tangents.emplace_back(1.f, 0.f, 0.f, 1.f);

        // ring
        for (unsigned int iSide = 0; iSide <= sides; iSide++) {
            const float c = cosThetas[iSide];
            const float s = sinThetas[iSide];

            const float x = radiusTop*c;
            const float z = radiusTop*s;

            positions.emplace_back(x, yTop, z);
            normals.emplace_back(0.f, 1.f, 0.f);
            uvs.emplace_back(0.5f + 0.5f*c, 0.5f + 0.5f*s);
            tangents.emplace_back(1.f, 0.f, 0.f, 1.f);
        }

        for (unsigned int i = 0; i < sides; ++i) {
            const unsigned int center = capStart;
            const unsigned int a = capStart + 1u + i;
            const unsigned int b = capStart + 1u + (i + 1u);

            indices.push_back(center);
            indices.push_back(b);
            indices.push_back(a);
        }
    }

    // ==================================================
    // 3) Bottom Surface
    // ==================================================
    const unsigned int capStart = (unsigned int)positions.size();

    // center
    positions.emplace_back(0.f, yBottom, 0.f);
    normals.emplace_back(0.f, -1.f, 0.f);
    uvs.emplace_back(0.5f, 0.5f);
    tangents.emplace_back(1.f, 0.f, 0.f, 1.f);

    // ring
    for (unsigned int iSide = 0; iSide <= sides; iSide++) {
        const float c = cosThetas[iSide];
        const float s = sinThetas[iSide];

        const float x = radiusBottom*c;
        const float z = radiusBottom*s;

        positions.emplace_back(x, yBottom, z);
        normals.emplace_back(0.f, -1.f, 0.f);
        uvs.emplace_back(0.5f + 0.5f*c, 0.5f + 0.5f*s);
        tangents.emplace_back(1.f, 0.f, 0.f, 1.f);
    }

    for (unsigned int iSide = 0; iSide < sides; iSide++) {
        const unsigned int center = capStart;
        const unsigned int a = capStart + 1u + iSide;
        const unsigned int b = capStart + 1u + (iSide + 1u);

        indices.push_back(center);
        indices.push_back(a);
        indices.push_back(b);
    }

    std::vector<Texture> textures;
    textures.emplace_back(Texture());

    return Mesh(positions, normals, uvs, tangents, indices, textures);
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

Mesh Geometry::pyramidSquare(float baseSize, float height)
{
    const float eps = 1e-6f;

    if (baseSize <= eps) {
        throw std::invalid_argument("[Geometry::pyramidSquare] baseSize must be > 0.");
    }
    if (height <= eps) {
        throw std::invalid_argument("[Geometry::pyramidSquare] height must be > 0.");
    }

    const float halfHeight = 0.5f * height;
    const float yTop    =  halfHeight;
    const float yBottom = -halfHeight;

    const float a = 0.5f * baseSize;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec4> tangents;
    std::vector<unsigned int> indices;

    // Hard edges => no vertex sharing between side faces.
    // Build 4 side triangles * 3 vertices = 12 verts
    // plus base cap 4 verts = 16 verts total.
    positions.reserve(16);
    normals.reserve(16);
    uvs.reserve(16);
    tangents.reserve(16);
    indices.reserve(4 * 3 + 2 * 3);

    const glm::vec3 apex(0.f, yTop, 0.f);
    const glm::vec3 c0(-a, yBottom, -a);
    const glm::vec3 c1( a, yBottom, -a);
    const glm::vec3 c2( a, yBottom,  a);
    const glm::vec3 c3(-a, yBottom,  a);
    const glm::vec3 corners[4] = { c0, c1, c2, c3 };

    // -------------
    // Side faces
    // -------------
    // For each face i, use corners[i] -> corners[i+1] plus apex.
    // We emit triangle vertices as (apex, next, curr) to match GL_CCW from outside.
    for (unsigned int i = 0; i < 4; ++i) {
        const glm::vec3& curr = corners[i];
        const glm::vec3& next = corners[(i + 1u) % 4u];

        // Face normal from winding (apex, next, curr)
        const glm::vec3 faceN = glm::normalize(glm::cross(next - apex, curr - apex));

        // Per-face tangent: along edge apex->next (any consistent tangent works for hard edges)
        glm::vec3 t3 = next - apex;
        if (glm::length2(t3) < eps) t3 = glm::vec3(1.f, 0.f, 0.f);
        t3 = glm::normalize(t3);
        const glm::vec4 faceT(t3, 1.f);

        // Simple per-face UVs: map each triangular face to [0,1]
        // (apex at top center, base edge spans U 0..1)
        const glm::vec2 uvApex(0.5f, 1.f);
        const glm::vec2 uvNext(1.f, 0.f);
        const glm::vec2 uvCurr(0.f, 0.f);

        const unsigned int baseIdx = (unsigned int)positions.size();

        positions.push_back(apex);
        normals.push_back(faceN);
        uvs.push_back(uvApex);
        tangents.push_back(faceT);

        positions.push_back(next);
        normals.push_back(faceN);
        uvs.push_back(uvNext);
        tangents.push_back(faceT);

        positions.push_back(curr);
        normals.push_back(faceN);
        uvs.push_back(uvCurr);
        tangents.push_back(faceT);

        indices.push_back(baseIdx + 0u);
        indices.push_back(baseIdx + 1u);
        indices.push_back(baseIdx + 2u);
    }

    // -------------
    // Base cap (flat)
    // -------------
    const unsigned int baseStart = (unsigned int)positions.size();
    const glm::vec3 down(0.f, -1.f, 0.f);
    const glm::vec4 baseTangent(1.f, 0.f, 0.f, 1.f);

    auto baseUV = [&](const glm::vec3& p) -> glm::vec2 {
        // Map x,z from [-a,a] to [0,1]
        return glm::vec2((p.x / (2.f * a)) + 0.5f, (p.z / (2.f * a)) + 0.5f);
    };

    for (unsigned int i = 0; i < 4; ++i) {
        const glm::vec3& p = corners[i];
        positions.push_back(p);
        normals.push_back(down);
        uvs.push_back(baseUV(p));
        tangents.push_back(baseTangent);
    }

    // Keep base winding so normal points -Y (outside)
    indices.push_back(baseStart + 0u);
    indices.push_back(baseStart + 1u);
    indices.push_back(baseStart + 2u);

    indices.push_back(baseStart + 0u);
    indices.push_back(baseStart + 2u);
    indices.push_back(baseStart + 3u);

    std::vector<Texture> textures;
    textures.emplace_back(Texture());

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
