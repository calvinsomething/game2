#include "CollisionSystem.h"

constexpr int grid_cell_size = 4;

void CollisionSystem::reserve_triangles(size_t n)
{
    triangles.reserve(triangles.size() + n);
}

float CollisionSystem::get_xz_determinant(const DirectX::XMFLOAT3 &origin, const DirectX::XMFLOAT3 &a,
                                          const DirectX::XMFLOAT3 &b)
{
    float a_x = a.x - origin.x, b_x = b.x - origin.x, a_z = a.z - origin.z, b_z = b.z - origin.z;

    return b_x * a_z - a_x * b_z;
}

CollisionFloor CollisionSystem::get_floor(const DirectX::XMFLOAT3 &position)
{
    CollisionFloor floor = {};
    floor.y = -1e10f;

    auto cell = grid.find(get_key(get_grid_coord(position.x), get_grid_coord(position.z)));
    if (cell != grid.end())
    {
        for (uint32_t i : cell->second)
        {
            float total_area =
                get_xz_determinant(triangles[i].position[0], triangles[i].position[1], triangles[i].position[2]);

            float weight_0 =
                get_xz_determinant(position, triangles[i].position[1], triangles[i].position[2]) / total_area;

            if (weight_0 >= 0.0f && weight_0 <= 1.0f)
            {
                float weight_1 =
                    get_xz_determinant(position, triangles[i].position[2], triangles[i].position[0]) / total_area;

                if (weight_1 >= 0.0f && weight_1 <= 1.0f)
                {
                    float weight_2 = 1.0f - weight_0 - weight_1;

                    if (weight_2 >= 0.0f && weight_2 <= 1.0f)
                    {
                        float y = triangles[i].position[0].y * weight_0 + triangles[i].position[1].y * weight_1 +
                                  triangles[i].position[2].y * weight_2;

                        bool hit = std::abs(position.y - y) < 0.1f;

                        if (y > floor.y && (hit || y < position.y))
                        {
                            floor.y = y;
                            floor.normal = triangles[i].normal;
                            floor.hit = hit;
                        }
                    }
                }
            }
        }
    }

    return floor;
}

bool CollisionSystem::get_first_intersected_cell_in_pos_x(DirectX::XMFLOAT2 &p1, DirectX::XMFLOAT2 p2, float slope,
                                                          DirectX::XMINT2 &cell)
{
    bool in_pos_y = p2.y >= p1.y;

    float x_boundary = (cell.x + 1) * grid_cell_size;

    float y_boundary;
    bool p2_inside_y_boundary;

    if (in_pos_y)
    {
        y_boundary = (cell.y + 1) * grid_cell_size;

        p2_inside_y_boundary = p2.y < y_boundary;
    }
    else
    {
        y_boundary = cell.y * grid_cell_size;

        p2_inside_y_boundary = p2.y > y_boundary;
    }

    if (p2.x < x_boundary && p2_inside_y_boundary)
    {
        // if line ends before crossing either boundary
        return false;
    }

    float x_at_y = p1.x + (y_boundary - p1.y) / slope;
    float y_at_x = p1.y + (x_boundary - p1.x) * slope;

    if (x_at_y < x_boundary)
    {
        cell.y += in_pos_y ? 1 : -1;
        p1.x = x_at_y;
        p1.y = y_boundary;
    }
    else
    {
        ++cell.x;
        p1.x = x_boundary;
        p1.y = y_at_x;
    }

    return true;
}

void CollisionSystem::add_triangle(const CollisionTriangle &triangle)
{
    StdVector<DirectX::XMINT2> intersected_grid_cells;
    intersected_grid_cells.reserve(10);

    for (size_t i = 0; i < 3; ++i)
    {
        size_t j = (i + 1) % 3;

        size_t a, b;
        if (triangle.position[i].x <= triangle.position[j].x)
        {
            a = i;
            b = j;
        }
        else
        {
            a = j;
            b = i;
        }

        DirectX::XMFLOAT2 p1 = {triangle.position[a].x, triangle.position[a].z};
        DirectX::XMFLOAT2 p2 = {triangle.position[b].x, triangle.position[b].z};

        float slope = (p2.y - p1.y) / (p2.x - p1.x);

        DirectX::XMINT2 cell{get_grid_coord(p1.x), get_grid_coord(p1.y)};

        bool ok = true;
        while (ok)
        {
            intersected_grid_cells.push_back(cell);
            ok = get_first_intersected_cell_in_pos_x(p1, p2, slope, cell);
        }
    }

    StdUnorderedMap<int, std::pair<int, int>> row_end_cells;
    for (auto &c : intersected_grid_cells)
    {
        auto row = row_end_cells.find(c.y);
        if (row == row_end_cells.end())
        {
            row_end_cells.insert({c.y, {c.x, c.x}});
        }
        else
        {
            if (c.x < row->second.first)
            {
                row->second.first = c.x;
            }
            else if (c.x > row->second.second)
            {
                row->second.second = c.x;
            }
        }
    }

    for (auto &row : row_end_cells)
    {
        for (int x = row.second.first; x <= row.second.second; ++x)
        {
            auto key = get_key(x, row.first);

            auto cell = grid.find(key);
            if (cell == grid.end())
            {
                grid.insert({key, StdVector<uint32_t>(1, uint32_t(triangles.size()))});
            }
            else
            {
                cell->second.push_back(triangles.size());
            }
        }
    }

    triangles.push_back(triangle);
}

int CollisionSystem::get_grid_coord(float c)
{
    int floored = c / grid_cell_size;
    if (c >= 0)
    {
        return floored;
    }
    else
    {
        // floor negative coords in negative direction rather than mirroring positive flooring
        // e.g. -4.01f -> -2
        return floored - int(floored * grid_cell_size != c);
    }
}

uint64_t CollisionSystem::get_key(uint32_t x, uint32_t z)
{
    return (uint64_t(x) << 32) | z;
}
