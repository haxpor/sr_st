#pragma once

#include "Platform.h"
#include "Types.h"

#include <vector>
#include <algorithm>

SR_NAMESPACE_START

///
/// ObjData holding data of .obj file as loaded.
/// It supports both copy and move operation.
struct ObjData
{
    std::vector<sr::Vec3f> vertices;
    std::vector<std::vector<unsigned int>> faces;
    /// ... will be more data to load i.e. normals, texture-coord

    ObjData() = default;
    ObjData(const ObjData& other);
    ObjData(ObjData&& other);
    ~ObjData() = default;
    ObjData& operator=(const ObjData& other);
    ObjData& operator=(ObjData&& other);
    friend void swap(ObjData& first, ObjData& second) noexcept;
};

class ObjLoader
{
public:
    ///
    /// Load .obj file then return result of formed vertices.
    /// Load vertex, texture coordinate and normals.
    ///
    /// \param filepath file path of .obj file to parse
    /// \param dataOut Data output to be set when it successfully loaded.
    /// \return Return true if successfully loaded, otherwise return false.
    ///
    static bool loadObjFile(const char* filepath, ObjData& dataOut);
};

SR_NAMESPACE_END
