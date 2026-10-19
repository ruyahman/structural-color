#pragma once
#include <DirectXMath.h>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <vector>

// CPU-side surface generation, independent of a D3D device so it can be tested.
namespace Surface
{
    constexpr unsigned kGridDivisions = 150;
    constexpr float kMaxPatternFrequency = 20.0f;
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT3 tangent;
    };
    static_assert(sizeof(Vertex) == 36, "Vertex layout must match light.vs");
    struct Mesh
    {
        std::vector<Vertex> vertices;
        std::vector<std::uint32_t> indices;
    };

    inline bool IsValidFrequency(float frequency)
    {
        return std::isfinite(frequency) && frequency >= 0.0f &&
               frequency <= kMaxPatternFrequency;
    }

    inline DirectX::XMFLOAT3 EvaluateTangent(float x, float y, float frequency)
    {
        // Original research demo: rotate the tangent within the flat XY surface.
        // Frequency changes the direction field, not the geometry or its normal.
        const float angle = DirectX::XM_PI *
            std::cos(DirectX::XM_2PI * x * frequency) *
            std::cos(DirectX::XM_2PI * y * frequency);
        return {std::cos(angle), std::sin(angle), 0.0f};
    }

    inline void UpdateTangents(std::vector<Vertex>& vertices, float frequency)
    {
        if (!IsValidFrequency(frequency))
            throw std::invalid_argument("Pattern frequency is outside the supported range");
        for (auto& vertex : vertices)
            vertex.tangent = EvaluateTangent(vertex.position.x, vertex.position.y, frequency);
    }

    inline Mesh BuildGrid(unsigned divisions = kGridDivisions)
    {
        if (divisions == 0 || divisions > 512)
            throw std::invalid_argument("Grid divisions must be in [1, 512]");
        Mesh mesh;
        const unsigned rowSize = divisions + 1;
        mesh.vertices.reserve(rowSize * rowSize);
        mesh.indices.reserve(divisions * divisions * 6);
        for (unsigned y = 0; y <= divisions; ++y)
        {
            for (unsigned x = 0; x <= divisions; ++x)
            {
                const float px = 2.0f * x / divisions - 1.0f;
                const float py = 2.0f * y / divisions - 1.0f;
                mesh.vertices.push_back({{px, py, 0.0f}, {0.0f, 0.0f, 1.0f},
                                         EvaluateTangent(px, py, 0.0f)});
            }
        }
        // Independent triangles avoid the unintended connections between rows
        // that occurred when four indices per cell were drawn as one strip.
        for (unsigned y = 0; y < divisions; ++y)
        {
            for (unsigned x = 0; x < divisions; ++x)
            {
                const auto a = y * rowSize + x;
                mesh.indices.insert(mesh.indices.end(),
                    {a, a + 1, a + rowSize, a + rowSize, a + 1, a + rowSize + 1});
            }
        }
        return mesh;
    }
}
