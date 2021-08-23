#include "Tree.h"

Mesh meshCreate()
{
    Mesh mesh;
    mesh.id = glee_buffer_id();
    mesh.vertices = NULL;
    mesh.indices = NULL;
    return mesh;
}

void meshDestroy(Mesh* mesh)
{
    if (mesh->vertices) array_destroy(mesh->vertices);
    if (mesh->indices) array_destroy(mesh->indices);
}

void meshBind(Mesh* mesh)
{
    if (!mesh->vertices || !mesh->indices) return;

    unsigned int VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(mesh->id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices->used * mesh->vertices->bytes, mesh->vertices->data, GL_STATIC_DRAW);  
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices->used * mesh->indices->bytes, mesh->indices->data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(5);	
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, mesh->vertices->bytes, (void*)0);
}

static void meshAddQuad(array_t* vertices, unsigned int x, unsigned int y, unsigned int atlasSize, unsigned int atlasIndex)
{
    x *= 32;
    y *= 32;
    vec2 pos[] = {
        {(float)(x + 32),    (float)(y + 32)},
        {(float)(x + 32),    (float)y       },
        {(float)x,          (float)(y + 32) },
        {(float)(x + 32),    (float)y       },
        {(float)x,          (float)y        },
        {(float)x,          (float)(y + 32) },
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
        array_push(vertices, &v);
    }
}

Mesh meshFromMap(map_t map)
{
    const unsigned int atlasSize = 2;
    array_t* vertices = array_new(32, sizeof(Vertex));

    for (unsigned int y = 0; y < map.height; y++) {
        for (unsigned int x = 0; x < map.width; x++) {
            uint8_t tile = *map_tile(map, x, y);
            if (tile == MAP_TILE_BLOCK || tile == MAP_TILE_STATIC) {
                unsigned int index;
                if (y + 1 < map.height && (*map_tile(map, x, y + 1) == MAP_TILE_BLOCK || *map_tile(map, x, y + 1) == MAP_TILE_STATIC)) {
                    index = 1;
                } else index = 0;
                meshAddQuad(vertices, x, y, atlasSize, index);
            }
        }
    }

    Mesh mesh = meshCreate();
    array_map_indexed(vertices, &mesh.vertices, &mesh.indices);
    array_destroy(vertices);
    meshBind(&mesh);
    return mesh;
}