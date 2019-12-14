#include "ObjLoader.h"

#include "Logger.h"

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <vector>

SR_NAMESPACE_START

ObjData::ObjData()
    : verticesPtr(nullptr)
    , indicesPtr(nullptr)
    , verticesCount(0)
    , indicesCount(0)
{
}

ObjData::ObjData(const ObjData& other)
{
    verticesPtr = new Vertex_v1[other.verticesCount];
    verticesCount = other.verticesCount;

    indicesPtr = new unsigned int[other.indicesCount];
    indicesCount = other.indicesCount;
}

ObjData::ObjData(ObjData&& other)
{
    using std::swap;
    swap(*this, other);
    other.verticesPtr = nullptr;
    other.indicesPtr = nullptr;
}

ObjData::~ObjData()
{
    if (verticesPtr)
    {
        free(verticesPtr);
        verticesPtr = nullptr;
    }
    if (indicesPtr)
    {
        free(indicesPtr);
        indicesPtr = nullptr;
    }
    verticesCount = 0;
    indicesCount = 0;
}

ObjData& ObjData::operator=(const ObjData& other)
{
    if (&other == this)
        return *this;

    // release the current resource we're holding first
    // note: we don't call destructor() direcly in this case, mostly only case to use it only when
    // use it with placement new, not this one.
    if (verticesPtr)
        free(verticesPtr);
    if (indicesPtr)
        free(indicesPtr);

    // make a copy
    verticesPtr = new Vertex_v1[other.verticesCount];
    *verticesPtr = *other.verticesPtr;
    verticesCount = other.verticesCount;

    indicesPtr = new unsigned int[other.indicesCount];
    *indicesPtr = *other.indicesPtr;
    indicesCount = other.indicesCount;

    return *this;
}

ObjData& ObjData::operator=(ObjData&& other)
{
    // check if it's not pointing to the same object, we want to avoid that
    if (&other == this)
        return *this;

    // release the current resource we're holding first
    // note: we don't call destructor() direcly in this case, mostly only case to use it only when
    // use it with placement new, not this one.
    if (verticesPtr)
        free(verticesPtr);
    if (indicesPtr)
        free(indicesPtr);

    // just transfer the ownership
    verticesPtr = other.verticesPtr;
    indicesPtr = other.indicesPtr;
    verticesCount = other.verticesCount;
    indicesCount = other.indicesCount;

    // set null to other's pointers to prevent it to get deleted when out of scope
    other.verticesPtr = nullptr;
    other.indicesPtr = nullptr;
    other.verticesCount = 0;
    other.indicesCount = 0;

    return *this;
}

void swap(ObjData& first, ObjData& second) noexcept
{
    using std::swap;
    swap(first.verticesPtr, second.verticesPtr);
    swap(first.indicesPtr, second.indicesPtr);
    swap(first.verticesCount, second.verticesCount);
    swap(first.indicesCount, second.indicesCount);
}

// start with 100 empty space to hold elements
#define INITIAL_ELEM_COUNT 100

// use the same factor as C++ stl uses in vector
#define INCREASE_ELEM_FACTOR 1.5

typedef struct DUP DUP;
struct DUP
{
    Vertex_v1 v;
    int idx;
    DUP* next;
};

typedef struct
{
    bool set;
    DUP* dup;
} DUPS;

/// handle vertex for 'f '
static void handle_f_v(int v_index, int vt_index, int vn_index, int* final_vertices_count, int* final_indices_count, Vertex_v1** out_vertices, unsigned int** out_indices, int* latest_used_vertices_index, int* indices_index, Vec3* vertices, Vec2* texcoords, Vec3* normals, DUPS* dup_table);

static void free_dup_recur(DUP* dup_ptr)
{
    if (dup_ptr == nullptr)
    {
        return;
    }
    else
    {
        free_dup_recur(dup_ptr->next);
        dup_ptr->next = nullptr;

        // free this node
        free(dup_ptr);
    }
}

static void free_dups(DUPS** dups_table, int count)
{
    // get value pointed to by double pointer
    DUPS* dups_table_v = *dups_table;

    // free each element of dups table
    for (int i=0; i<count; i++)
    {
        free_dup_recur(dups_table_v[i].dup);
        dups_table_v[i].dup = nullptr;
    }

    // free dups table itself
    free(dups_table_v);
    *dups_table = nullptr;
}

bool LoadObjFile(const char* filepath, ObjData& dataOut)
{
    std::FILE* file = std::fopen(filepath, "rb");
    if (file == nullptr)
    {
        LOGE("Cannot read .obj file");
        return false;
    }

    // define line buffer to read from file
    char line[256];

    // collect all meta vertex array info before we finally form final vertices and indices
    Vec3* vertices = nullptr; 
    Vec2* texcoords = nullptr;
    Vec3* normals = nullptr;

    // keep track of current allocated count for all buffer
    int v_alloc_count = INITIAL_ELEM_COUNT;
    int vt_alloc_count = INITIAL_ELEM_COUNT;
    int vn_alloc_count = INITIAL_ELEM_COUNT;
    // running indexes for all data
    int vertices_i = 0;
    int texcoords_i = 0;
    int normals_i = 0;

    // start with lowest enough space to hold elements
    // the true number of element is tracked along the way, and
    // we will adjust the number of space later at the end to reduce un-needed memory usage
    vertices = static_cast<Vec3*>(std::calloc(1, sizeof(Vec3) * v_alloc_count));
    texcoords = static_cast<Vec2*>(std::calloc(1, sizeof(Vec2) * vt_alloc_count));
    normals = static_cast<Vec3*>(std::calloc(1, sizeof(Vec3) * vn_alloc_count));

    // flag checking whether it has been entered into 'f ' case
    bool entered_f = false;

    // variables related to 'f ' section
    // this will be defered in allocation later after we entered into 'f ' section after we know exact number of elements to allocate
    // from those vertex data
    Vertex_v1* out_vertices = nullptr;
    int final_vertices_count = 0;
    int static_vertices_count = 0;

    // same for indices
    unsigned int* out_indices = nullptr; 
    int final_indices_count = 0;

    // latest used vertices index, default to -1
    int latest_used_vertices_index = -1;
    // index for indices, support only for triangle-face for now
    int i_i = 0;

    // duplicate table
    // when such vertex is found, then set this table for such element
    // as well as add duplicated element in the list for future checking
    // in order to give us a correct indice value
    DUPS* v_set_table = nullptr;

    // read file line by line
    float temp_v[3];
    int idx[9];
    while (std::fgets(line, sizeof(line), file))
    {
        if (std::strncmp(line, "v ", strlen("v ")) == 0)
        {
            // start at offset 2 as we don't need to pay attention to prefix "v "
            // the same for others
            if (std::sscanf(line + 2, "%f %f %f", &temp_v[0], &temp_v[1], &temp_v[2]) == 3)
            {
                // check if need to expand the memory space 
                if (vertices_i >= v_alloc_count)
                {
                    // expand with pre-set value
                    v_alloc_count += v_alloc_count * INCREASE_ELEM_FACTOR;
                    vertices = static_cast<Vec3*>(std::realloc(vertices, sizeof(Vec3) * v_alloc_count));
                }

                // copy by values
                vertices[vertices_i].x = temp_v[0];
                vertices[vertices_i].y = temp_v[1];
                vertices[vertices_i++].z = temp_v[2];
            }
            else
            {
                LOGE("Warning: Failed attempt to read vertex data for 'v ' format");
            }
        }
        else if (std::strncmp(line, "vt ", strlen("vt ")) == 0)
        {
            if (std::sscanf(line + 3, "%f %f", &temp_v[0], &temp_v[1]) == 2)
            {
                // check if need to expand the memory space 
                if (texcoords_i >= vt_alloc_count)
                {
                    // expand with pre-set value
                    vt_alloc_count += vt_alloc_count * INCREASE_ELEM_FACTOR;
                    texcoords = static_cast<Vec2*>(std::realloc(texcoords, sizeof(Vec2) * vt_alloc_count));
                }

                texcoords[texcoords_i].s = temp_v[0];
                texcoords[texcoords_i++].t = temp_v[1];
            }
            else
            {
                LOGE("Warning: Failed attempt to read vertex data for 'vt ' format");
            }
        }
        else if (std::strncmp(line, "vn ", strlen("vn ")) == 0)
        {
            if (std::sscanf(line + 3, "%f %f %f", &temp_v[0], &temp_v[1], &temp_v[2]) == 3)
            {
                // check if need to expand the memory space 
                if (normals_i >= vn_alloc_count)
                {
                    // expand with pre-set value
                    vn_alloc_count += vn_alloc_count * INCREASE_ELEM_FACTOR;
                    normals = static_cast<Vec3*>(std::realloc(normals, sizeof(Vec3) * vn_alloc_count));
                }

                normals[normals_i].x = temp_v[0];
                normals[normals_i].y = temp_v[1];
                normals[normals_i++].z = temp_v[2];
            }
            else
            {
                LOGE("Warning: Failed attempt to read vertex data for 'vn ' format");
            }
        }
        else if (std::strncmp(line, "f ", strlen("f ")) == 0)
        {
            if (!entered_f)
            {
                // set flag
                entered_f = true;

                // shrink memory space previously allocated for all 3 buffers
                vertices = static_cast<Vec3*>(std::realloc(vertices, sizeof(Vec3) * vertices_i));
                texcoords = static_cast<Vec2*>(std::realloc(texcoords, sizeof(Vec2) * texcoords_i));
                normals = static_cast<Vec3*>(std::realloc(normals, sizeof(Vec3) * normals_i));

                // set number of elements for final vertices as well
                // differentiate from v_count to give us info later how many actual 'v ' is
                final_vertices_count = vertices_i;
                // initially set latest vertices used for duplicated vertex item later used in handle_f_v
                latest_used_vertices_index = final_vertices_count;

                // allocate memory space
                // allocate vertices enough space initially
                out_vertices = static_cast<Vertex_v1*>(std::calloc(1, sizeof(Vertex_v1) * final_vertices_count));
                // allocate with initial guess space
                final_indices_count = INITIAL_ELEM_COUNT * 3;
                out_indices = static_cast<unsigned int*>(std::calloc(1, sizeof(unsigned int) * final_indices_count));

                // allocate memory space for dup table
                // number of element for table won't grow as each element might maintain its own duplicate list
                v_set_table = static_cast<DUPS*>(std::calloc(1, sizeof(DUPS) * final_vertices_count));

                // save vertices count for later use
                // this value won't be modified by handle_f_v() function
                static_vertices_count = final_vertices_count;
            }

            if (std::sscanf(line + 2, "%d/%d/%d %d/%d/%d %d/%d/%d", &idx[0], &idx[1], &idx[2], &idx[3], &idx[4], &idx[5], &idx[6], &idx[7], &idx[8]) == 9)
            {
                // vertex 1
                handle_f_v(idx[0]-1, idx[1]-1, idx[2]-1, &final_vertices_count, &final_indices_count, &out_vertices, &out_indices, &latest_used_vertices_index, &i_i, vertices, texcoords, normals, v_set_table);
                // vertex 2
                handle_f_v(idx[3]-1, idx[4]-1, idx[5]-1, &final_vertices_count, &final_indices_count, &out_vertices, &out_indices, &latest_used_vertices_index, &i_i, vertices, texcoords, normals, v_set_table);
                // vertex 3
                handle_f_v(idx[6]-1, idx[7]-1, idx[8]-1, &final_vertices_count, &final_indices_count, &out_vertices, &out_indices, &latest_used_vertices_index, &i_i, vertices, texcoords, normals, v_set_table);
            }
            else
            {
                LOG("Warning: Failed attempt to read vertex data for 'f ' format");
            }
        }
    }

    // close file
    std::fclose(file);
    file = nullptr;

    // free un-needed anymore vertex data
    free(vertices);
    vertices = nullptr;
    free(texcoords);
    texcoords = nullptr;
    free(normals);
    normals = nullptr;

    // free dups table
    free_dups(&v_set_table, static_vertices_count);

    // shrink dowm memory of out_vertices, and out_indices
    int total_real_vertices_count = static_vertices_count + (latest_used_vertices_index - static_vertices_count);
    out_vertices = static_cast<Vertex_v1*>(std::realloc(out_vertices, sizeof(Vertex_v1) * total_real_vertices_count));
    out_indices = static_cast<unsigned int*>(std::realloc(out_indices, sizeof(unsigned int) * (i_i+1)));
    LOGE("final shrink vertices output down to = %d", total_real_vertices_count);

    LOGE("total vertices: %d, texcoords: %d, normals: %d, final vertices: %d, indices count: %d", vertices_i, texcoords_i, normals_i, total_real_vertices_count, i_i+1);
    LOGE("calculated (as expanded) vertices count: %d, calculated indices count: %d", final_vertices_count, final_indices_count);

    // set results
    if (dataOut.verticesPtr != nullptr)     // check pointer itself is declared somewhere externally, check so we can set such pointer to point to our loaded data
        dataOut.verticesPtr = out_vertices;
    dataOut.verticesCount = total_real_vertices_count;

    if (dataOut.indicesPtr != nullptr)
        dataOut.indicesPtr = out_indices;
    dataOut.indicesCount = i_i+1;

    return 0;
}

void handle_f_v(int v_index, int vt_index, int vn_index, int* final_vertices_count, int* final_indices_count, Vertex_v1** out_vertices, unsigned int** out_indices, int* latest_used_vertices_index, int* indices_index, Vec3* vertices, Vec2* texcoords, Vec3* normals, DUPS* dup_table)
{
    // get the pointer to vertices and indices to work with
    Vertex_v1* out_vertices_v = *out_vertices;
    unsigned int* out_indices_v = *out_indices;

    if (!dup_table[v_index].set)
    {
        out_vertices_v[v_index].pos = vertices[v_index];
        out_vertices_v[v_index].texcoord = texcoords[vt_index];
        out_vertices_v[v_index].normal = normals[vn_index];

        // expand memory space for indices (if need)
        if (*indices_index >= *final_indices_count)
        {
            *final_indices_count = *final_indices_count + *final_indices_count * INCREASE_ELEM_FACTOR;
            *out_indices = static_cast<unsigned int*>(std::realloc(out_indices_v, sizeof(unsigned int) * *final_indices_count));
            // update out_indices_v pointer
            out_indices_v = *out_indices;
        }
        // update indices
        out_indices_v[*indices_index] = v_index;
        *indices_index = *indices_index + 1;

        dup_table[v_index].set = true;
    }
    else
    {
        // early check against root if it matches or not
        // so later we can skip checking aginst root
        if (fabs(out_vertices_v[v_index].texcoord.s - texcoords[vt_index].s) < 1.0e-5 &&
                fabs(out_vertices_v[v_index].texcoord.t - texcoords[vt_index].t) < 1.0e-5 &&
                fabs(out_vertices_v[v_index].normal.x - normals[vn_index].x) < 1.0e-5 &&
                fabs(out_vertices_v[v_index].normal.y - normals[vn_index].y) < 1.0e-5 &&
                fabs(out_vertices_v[v_index].normal.z - normals[vn_index].z) < 1.0e-5)
        {
            // expand memory space for indices (if need)
            if (*indices_index >= *final_indices_count)
            {
                *final_indices_count = *final_indices_count + *final_indices_count * INCREASE_ELEM_FACTOR;
                *out_indices = static_cast<unsigned int*>(std::realloc(out_indices_v, sizeof(unsigned int) * *final_indices_count));
                // update out_indices_v pointer
                out_indices_v = *out_indices;
            }
            // update indices
            out_indices_v[*indices_index] = v_index;
            *indices_index = *indices_index + 1;
        }
        else
        {
            // if no duplicate, then add itself into the list
            if (dup_table[v_index].dup == nullptr)
            {
                // expand memory space for vertices (if need)
                // note: always expand for the first time check
                if (*latest_used_vertices_index >= *final_vertices_count)
                {
                    // update vertices count
                    *final_vertices_count = *final_vertices_count + *final_vertices_count * INCREASE_ELEM_FACTOR;
                    *out_vertices = static_cast<Vertex_v1*>(std::realloc(out_vertices_v, sizeof(Vertex_v1) * *final_vertices_count));
                    // update out_vertices_v pointer
                    out_vertices_v = *out_vertices;
                }

                // add into result vertices
                out_vertices_v[*latest_used_vertices_index].pos = vertices[v_index];
                out_vertices_v[*latest_used_vertices_index].texcoord = texcoords[vt_index];
                out_vertices_v[*latest_used_vertices_index].normal = normals[vn_index];

                // expand memory space for indices (if need)
                if (*indices_index >= *final_indices_count)
                {
                    *final_indices_count = *final_indices_count + *final_indices_count * INCREASE_ELEM_FACTOR;
                    *out_indices = static_cast<unsigned int*>(std::realloc(out_indices_v, sizeof(unsigned int) * *final_indices_count));
                    // update out_indices_v pointer
                    out_indices_v = *out_indices;
                }
                // update indices
                out_indices_v[*indices_index] = *latest_used_vertices_index;
                *indices_index = *indices_index + 1;

                // add into duplicate list
                dup_table[v_index].dup = static_cast<DUP*>(std::malloc(sizeof(DUP)));
                dup_table[v_index].dup->v = out_vertices_v[*latest_used_vertices_index];
                dup_table[v_index].dup->idx = *latest_used_vertices_index;
                dup_table[v_index].dup->next = nullptr;

                // update vertices index
                *latest_used_vertices_index = *latest_used_vertices_index + 1;
            }
            // check against duplicate list
            else
            {
                DUP* prev_dup = nullptr;
                DUP* dup = dup_table[v_index].dup;
                bool unique = true;

                while ( dup != nullptr)
                {
                    Vertex_v1 v = dup->v;
                    // check for duplicate to finish the job earlier
                    if (fabs(v.texcoord.s - texcoords[vt_index].s) < 1.0e-5 &&
                            fabs(v.texcoord.t - texcoords[vt_index].t) < 1.0e-5 &&
                            fabs(v.normal.x - normals[vn_index].x) < 1.0e-5 &&
                            fabs(v.normal.y - normals[vn_index].y) < 1.0e-5 &&
                            fabs(v.normal.z - normals[vn_index].z) < 1.0e-5)
                    {
                        unique = false;

                        // expand memory space for indices (if need)
                        if (*indices_index >= *final_indices_count)
                        {
                            *final_indices_count = *final_indices_count + *final_indices_count * INCREASE_ELEM_FACTOR;
                            *out_indices = static_cast<unsigned int*>(std::realloc(out_indices_v, sizeof(unsigned int) * *final_indices_count));
                            // update out_indices_v pointer
                            out_indices_v = *out_indices;
                        }
                        // update indices
                        out_indices_v[*indices_index] = dup->idx;
                        *indices_index = *indices_index + 1;
                        break;
                    }

                    // update previous pointer
                    prev_dup = dup;

                    // update dup pointer
                    dup = dup->next;
                }

                // if still unique, then add into final vertices & duplicate list
                if (unique)
                {
                    // expand memory space for vertices (if need)
                    // note: always expand for the first time check
                    if (*latest_used_vertices_index >= *final_vertices_count)
                    {
                        // update vertices count
                        *final_vertices_count = *final_vertices_count + *final_vertices_count * INCREASE_ELEM_FACTOR;
                        *out_vertices = static_cast<Vertex_v1*>(std::realloc(out_vertices_v, sizeof(Vertex_v1) * *final_vertices_count));
                        // update out_vertices_v pointer
                        out_vertices_v = *out_vertices;
                    }

                    // add into result vertices
                    out_vertices_v[*latest_used_vertices_index].pos = vertices[v_index];
                    out_vertices_v[*latest_used_vertices_index].texcoord = texcoords[vt_index];
                    out_vertices_v[*latest_used_vertices_index].normal = normals[vn_index];

                    // expand memory space for indices (if need)
                    if (*indices_index >= *final_indices_count)
                    {
                        *final_indices_count = *final_indices_count + *final_indices_count * INCREASE_ELEM_FACTOR;
                        *out_indices = static_cast<unsigned int*>(std::realloc(out_indices_v, sizeof(unsigned int) * *final_indices_count)); 
                        // update out_indices_v pointer
                        out_indices_v = *out_indices;
                    }
                    // update indices
                    out_indices_v[*indices_index] = *latest_used_vertices_index;
                    *indices_index = *indices_index + 1;

                    // add into duplicate list
                    prev_dup->next = static_cast<DUP*>(std::malloc(sizeof(DUP)));
                    prev_dup->next->v = out_vertices_v[*latest_used_vertices_index];
                    prev_dup->next->idx = *latest_used_vertices_index;
                    prev_dup->next->next = nullptr;

                    // update vertices index
                    *latest_used_vertices_index = *latest_used_vertices_index + 1;
                }
            }
        }
    }
}

SR_NAMESPACE_END
