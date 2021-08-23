#include "Tree.h"
#include <string.h>

static Atlas atlasCreate(bmp_t bmp)
{
    Atlas atlas;
    atlas.bmp = bmp;
    atlas.count = 1;
    return atlas;
}

static void atlasPush(Atlas* atlas, bmp_t bmp)
{
    bmp_t tmp = bmp_new(atlas->bmp.width + bmp.width, atlas->bmp.height, atlas->bmp.channels);
    for (unsigned int y = 0; y < tmp.height; y++) {
        for (unsigned int x = 0; x < tmp.width; x++) {
            if (x < atlas->bmp.width) {
                memcpy(px_at(&tmp, x, y), px_at(&atlas->bmp, x, y), tmp.channels);
            } else {
                memcpy(px_at(&tmp, x, y), px_at(&bmp, x - atlas->bmp.width, y), tmp.channels);
            }
        }
    }
    bmp_free(&atlas->bmp);
    atlas->bmp = tmp;
    atlas->count++;
}

static void atlasBind(Atlas* atlas)
{
    atlas->texture = texture_atlas(&atlas->bmp);
}

Atlas atlasLoad(char** paths, unsigned int count)
{
    bmp_t tmp = bmp_load(paths[0]);
    Atlas atlas = atlasCreate(tmp);

    for (unsigned int i = 1; i < count; i++) {
        tmp = bmp_load(paths[i]);
        atlasPush(&atlas, tmp);
        bmp_free(&tmp);
    }
    atlasBind(&atlas);
    return atlas;
}

void atlasFree(Atlas* atlas)
{
    bmp_free(&atlas->bmp);
}