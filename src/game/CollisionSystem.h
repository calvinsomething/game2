#pragma once

#include "../util.h"

struct CollisionTriangle
{
    DirectX::XMFLOAT3 position[3];
    DirectX::XMFLOAT3 normal;
};

struct CollisionFloor
{
    bool hit = false;
    float y;
    DirectX::XMFLOAT3 normal;
};

class CollisionSystem
{
  public:
    CollisionSystem() = default;

    void reserve_triangles(size_t n);

    float get_xz_determinant(const DirectX::XMFLOAT3 &origin, const DirectX::XMFLOAT3 &a, const DirectX::XMFLOAT3 &b);

    CollisionFloor get_floor(const DirectX::XMFLOAT3 &position);

    void add_triangle(const CollisionTriangle &triangle);
    // int get_lateral_intersected_grid_cells_count(DirectX::XMFLOAT2 &p1, DirectX::XMFLOAT2 p2, float slope_inv);
    bool get_first_intersected_cell_in_pos_x(DirectX::XMFLOAT2 &p1, DirectX::XMFLOAT2 p2, float slope_inv,
                                             DirectX::XMINT2 &cell);

    template <typename T>
    void load_floor_triangles(T *vertices, uint32_t *indices, size_t n, const DirectX::XMMATRIX &xform)
    {
        reserve_triangles(n / 4);

        float slope_tolerance = std::cos(DirectX::XM_PI / 8);

        for (size_t i = 0; i < n; i += 3)
        {
            DirectX::XMFLOAT3 normal = normalize(transform(vertices[indices[i]].normal, xform));

            if (normal.y > slope_tolerance)
            {
                CollisionTriangle t = {};
                t.normal = normal;

                for (size_t j = 0; j < 3; ++j)
                {
                    t.position[j] = transform(vertices[indices[i + j]].position, xform);
                }

                add_triangle(t);
            }
        }
    }

  private:
    int get_grid_coord(float c);

    uint64_t get_key(uint32_t x, uint32_t z);

    StdUnorderedMap<uint64_t, StdVector<uint32_t>> grid;

    StdVector<CollisionTriangle> triangles;
};
