#ifndef CHUNK_H
#define CHUNK_H

#include "../app.h"

#define CX 16
#define CY 16
#define CZ 16

typedef glm::tvec4<GLbyte> byte4;

struct chunk {
    u8 blk[CX][CY][CZ];
    GLuint vbo;
    int elements;
    bool changed;

    chunk() {
        memset(blk, 0, sizeof(blk));
        elements = 0;
        changed = true;
        glGenBuffers(1, &vbo);
    }

    ~chunk() {
        glDeleteBuffers(1, &vbo);
    }

    u8 get(int x, int y, int z) {
        return blk[x][y][z];
    }

    void set(int x, int y, int z, u8 type) {
        blk[x][y][z] = type;
        changed = true;
    }

    void update() {
        changed = false;

        byte4 vertex[CX * CY * CZ * 6 * 6];
        int i = 0;

        for(int x=0; x<CX; x++) {
            for(int y=0; y<CY; y++) {
                for(int z=0; z<CZ; z++) {
                    u8 type = blk[x][y][z];

                    // Empty block?
                    if(!type) {
                        continue;
                    }

                    // View from negative x
                    vertex[i++] = byte4(x,   y,   z,   type);
                    vertex[i++] = byte4(x,   y,   z+1, type);
                    vertex[i++] = byte4(x,   y+1, z,   type);
                    vertex[i++] = byte4(x,   y+1, z,   type);
                    vertex[i++] = byte4(x,   y,   z+1, type);
                    vertex[i++] = byte4(x,   y+1, z+1, type);

                    // View from positive x
                    vertex[i++] = byte4(x+1, y,   z,   type);
                    vertex[i++] = byte4(x+1, y+1, z,   type);
                    vertex[i++] = byte4(x+1, y,   z+1, type);
                    vertex[i++] = byte4(x+1, y+1, z,   type);
                    vertex[i++] = byte4(x+1, y+1, z+1, type);
                    vertex[i++] = byte4(x+1, y,   z+1, type);

                    // View from negative y
                    vertex[i++] = byte4(x,   y,   z,   type);
                    vertex[i++] = byte4(x+1, y,   z,   type);
                    vertex[i++] = byte4(x,   y,   z+1, type);
                    vertex[i++] = byte4(x+1, y,   z,   type);
                    vertex[i++] = byte4(x+1, y,   z+1, type);
                    vertex[i++] = byte4(x,   y,   z+1, type);

                    // View from positive y
                    vertex[i++] = byte4(x,   y+1, z,   type);
                    vertex[i++] = byte4(x,   y+1, z+1, type);
                    vertex[i++] = byte4(x+1, y+1, z,   type);
                    vertex[i++] = byte4(x+1, y+1, z,   type);
                    vertex[i++] = byte4(x,   y+1, z+1, type);
                    vertex[i++] = byte4(x+1, y+1, z+1, type);

                    // View from negative z
                    vertex[i++] = byte4(x,   y,   z,   type);
                    vertex[i++] = byte4(x,   y+1, z,   type);
                    vertex[i++] = byte4(x+1, y,   z,   type);
                    vertex[i++] = byte4(x,   y+1, z,   type);
                    vertex[i++] = byte4(x+1, y+1, z,   type);
                    vertex[i++] = byte4(x+1, y,   z,   type);

                    // View from positive z
                    vertex[i++] = byte4(x,   y,   z+1, type);
                    vertex[i++] = byte4(x+1, y,   z+1, type);
                    vertex[i++] = byte4(x,   y+1, z+1, type);
                    vertex[i++] = byte4(x,   y+1, z+1, type);
                    vertex[i++] = byte4(x+1, y,   z+1, type);
                    vertex[i++] = byte4(x+1, y+1, z+1, type);
                }
            }
        }

        elements = i;
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, elements * sizeof(*vertex), vertex, GL_STATIC_DRAW);
    }

    void render() {
        if(changed) {
            update();
        }

        // If this chunk is empty, we don't need to draw anything
        if(!elements) {
            return;
        }

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        //6glVertexAttribPointer(attribute_coord, 4, GL_BYTE, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, elements);
    }
};

#endif
