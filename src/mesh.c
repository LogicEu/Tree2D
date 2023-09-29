#include "Tree.h"

Mesh meshCreate()
{
    Mesh mesh;
    mesh.vbo = 0;
    mesh.id = glee_buffer_id();
    mesh.vertices = array_new(32, sizeof(Vertex));
    return mesh;
}

void meshDestroy(Mesh* mesh)
{
    if (mesh->vertices)
        array_destroy(mesh->vertices);
}

void meshUpdate(const Mesh* mesh)
{
    glBindVertexArray(mesh->id);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices->used * mesh->vertices->bytes, mesh->vertices->data, GL_STATIC_DRAW);
}

void meshBind(Mesh* mesh)
{
    if (mesh->vertices) {
        glBindVertexArray(mesh->id);
        glGenBuffers(1, &mesh->vbo);
        meshUpdate(mesh);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, mesh->vertices->bytes, (void*)0);
        glEnableVertexAttribArray(5);
    }
}
#include <stdio.h>
void meshAddQuad(Mesh* mesh, float x, float y, unsigned int atlasSize, unsigned int atlasIndex)
{
    vec2 pos[] = {
        {x + 32.0f, y + 32.0f},
        {x + 32.0f, y        },
        {x,         y + 32.0f},
        {x + 32.0f, y        },
        {x,         y        },
        {x,         y + 32.0f},
    };

    float size = 1.0f / (float)atlasSize;
    float uvx = (float)atlasIndex * size;

    vec2 uvs[] = {
        {uvx,           0.0f},
        {uvx,           1.0f},
        {uvx + size,    0.0f},
        {uvx,           1.0f},
        {uvx + size,    1.0f},
        {uvx + size,    0.0f},
    };

    for (unsigned int i = 0; i < 6; i++) {
        Vertex v = {pos[i], uvs[i]};
        array_push(mesh->vertices, &v);
    }
}

Mesh meshFromMap(map_t map)
{
    const unsigned int atlasSize = 2;
    Mesh mesh = meshCreate();

    for (unsigned int y = 0; y < map.height; y++) {
        for (unsigned int x = 0; x < map.width; x++) {
            uint8_t tile = *map_tile(map, x, y);
            if (tile == MAP_TILE_BLOCK || tile == MAP_TILE_STATIC) {
                unsigned int index = (y + 1 < map.height && (*map_tile(map, x, y + 1) == MAP_TILE_BLOCK || *map_tile(map, x, y + 1) == MAP_TILE_STATIC));
                meshAddQuad(&mesh, (float)(x * 32), (float)(y * 32), atlasSize, index);
            }
        }
    }

    meshBind(&mesh);
    return mesh;
}