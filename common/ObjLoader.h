#pragma once

#include "Platform.h"
#include "Types.h"

#include <algorithm>

SR_NAMESPACE_START

///
/// ObjData holding data of .obj file as loaded.
/// It supports both copy and move operation.
struct ObjData
{
    Vertex_v1* verticesPtr;
    unsigned int* indicesPtr;

    int verticesCount;
    int indicesCount;

    ObjData();
    ObjData(const ObjData& other);
    ObjData(ObjData&& other);
    ~ObjData();
    ObjData& operator=(const ObjData& other);
    ObjData& operator=(ObjData&& other);
    friend void swap(ObjData& first, ObjData& second) noexcept;
};

///
/// Load .obj file then return result of formed vertices.
/// Load vertex, texture coordinate and normals.
///
/// \param filepath file path of .obj file to parse
/// \param dataOut Data output to be set when it successfully loaded.
/// \return Return true if successfully loaded, otherwise return false.
///
bool LoadObjFile(const char* filepath, ObjData& dataOut);

SR_NAMESPACE_END
