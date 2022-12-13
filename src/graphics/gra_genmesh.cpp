#include "air_graphics.h"

Mesh AirSoft::GenMeshCapsule(float radius, float height, uint32_t slices, uint32_t rings)
{
    Mesh mesh{};

    const uint32_t CAPSULE_TOP_VERT_COUNT = (slices * 2 * rings) + 1;
    const uint32_t CAPSULE_TOP_TRIANGLE_COUNT = 2 * slices * (2 * rings - 1);
    const uint32_t CAPSULE_TOP_INDEX_COUNT = CAPSULE_TOP_TRIANGLE_COUNT * 3;

    const uint32_t TRIANGLE_COUNT = CAPSULE_TOP_TRIANGLE_COUNT;// + (4 * slices);
    const uint32_t INDEX_COUNT = TRIANGLE_COUNT * 3;

    const float PITCH_MAX = (PI / 2.0f);
    const float PITCH_STEP = PITCH_MAX / rings;
    const float YAW_STEP = PI / slices;

    mesh.vertexCount = CAPSULE_TOP_VERT_COUNT;
    mesh.vertices = (float*) RL_MALLOC(sizeof(Vec3) * CAPSULE_TOP_VERT_COUNT); //(float*) new Vec3[CAPSULE_TOP_VERT_COUNT];
    memset(mesh.normals, 0, sizeof(Vec3) * CAPSULE_TOP_VERT_COUNT);
    mesh.normals = (float*) RL_MALLOC(sizeof(Vec3) * CAPSULE_TOP_VERT_COUNT);
    memset(mesh.normals, 0, sizeof(Vec3) * CAPSULE_TOP_VERT_COUNT);
    mesh.texcoords = (float*) RL_MALLOC(sizeof(Vec2) * CAPSULE_TOP_VERT_COUNT);
    memset(mesh.normals, 0, sizeof(Vec2) * CAPSULE_TOP_VERT_COUNT);

    float currentYaw = 0.0f;
    float currentPitch = PITCH_MAX;


    Vec3* vertexPtr = (Vec3*)mesh.vertices;


    Vec3 currentPos{};

    // Top and bottom half
    for (int i = 0; i < 1; i++)
    {
        *vertexPtr = {0.0f, -radius, 0.0f};
        vertexPtr++;

        for (int s = 0; s < slices * 2; s++)
        {
            
            for (int r = 0; r < rings; r++)
            {
                currentPitch -= PITCH_STEP;
                Vec3 forward = GetCameraForward(currentYaw, currentPitch);
                if (i == 0)
                {
                    forward = -forward;
                }

                *vertexPtr = currentPos + (forward * radius);
                vertexPtr++;
            }
            currentPitch = PITCH_MAX;
            currentYaw += YAW_STEP;
        }
        currentPos += {0.0f, height, 0.0f};
    }


    // Generate indices 
    // Generate first layer

    mesh.triangleCount = TRIANGLE_COUNT;
    mesh.indices = new unsigned short[INDEX_COUNT];

    unsigned short* indexPtr = mesh.indices;

    for (int c = 0; c < 1; c++)
    {
        for (uint32_t i = 0; i < slices * 2; i++)
        {
            uint32_t next = (i + 1) % (slices * 2);

            *indexPtr = 0;
            indexPtr++;

            *indexPtr = i + 1;
            indexPtr++;

            *indexPtr = next + 1;
            indexPtr++;
        }

        uint16_t lastIndex = 1;
        uint16_t lowIndex = 1;
        for (int r = 0; r < rings - 1; r++)
        {
            for (int s = 0; s < slices * 2; s++)
            {
                uint16_t nextLow = lastIndex + (((lowIndex + 1) - lastIndex) % (slices * 2));
                uint16_t next = nextLow + (slices * 2);

                *indexPtr = lowIndex;
                indexPtr++;

                *indexPtr = lowIndex + slices * 2;
                indexPtr++;

                *indexPtr = lowIndex + 1 + (slices * 2);
                indexPtr++;


                *indexPtr = lowIndex;
                indexPtr++;

                *indexPtr = lowIndex + (slices * 2) + 1;
                indexPtr++;

                *indexPtr = lowIndex + 1;
                indexPtr++;

                lowIndex++;
            }
        }
        /*if (c == 0)
        {
            uint16_t currentOffset = CAPSULE_TOP_VERT_COUNT - slices * 2;
            // Connect top with bottom
            for (int s = 0; s < slices * 2; s++)
            {
                *indexPtr = currentOffset;
                indexPtr++;

                *indexPtr = CAPSULE_TOP_VERT_COUNT + currentOffset;
                indexPtr++;
                
                *indexPtr = CAPSULE_TOP_VERT_COUNT + currentOffset + 1;
                indexPtr++;



                *indexPtr = currentOffset;
                indexPtr++;

                *indexPtr = CAPSULE_TOP_VERT_COUNT + currentOffset + 1;
                indexPtr++;
                
                *indexPtr = currentOffset + 1;
                indexPtr++;


                currentOffset++;
            }
        }*/
    }

    // Upload vertex data to GPU (static mesh)
    UploadMesh(&mesh, false);

    return mesh;
}