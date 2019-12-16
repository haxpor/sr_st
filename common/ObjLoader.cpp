#include "ObjLoader.h"

#include "Logger.h"

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <sstream>
#include <fstream>

SR_NAMESPACE_START

ObjData::ObjData(const ObjData& other)
{
    // make a copy from other
    vertices = other.vertices;
    faces = other.faces;
}

ObjData::ObjData(ObjData&& other)
{
    // swap the data content pointed to by vector
    vertices.swap(other.vertices);
    faces.swap(other.faces);
}

ObjData& ObjData::operator=(const ObjData& other)
{
    if (this == &other)
        return *this;

    // make a copy
    vertices = other.vertices;
    faces = other.faces;

    return *this;
}

ObjData& ObjData::operator=(ObjData&& other)
{
    if (this == &other)
        return *this;

    vertices.swap(other.vertices);
    faces.swap(other.faces);

    return *this;
}

void swap(ObjData& first, ObjData& second) noexcept
{
    first.vertices.swap(second.vertices);
    first.faces.swap(second.faces);
}

bool ObjLoader::loadObjFile(const char* filepath, ObjData& dataOut)
{
    std::ifstream filein;
    filein.open(filepath, std::ifstream::in);
    if (filein.fail())
    {
        LOGE("failed to read %s\n", filepath);
        return false;
    }

    std::string line;
    std::vector<unsigned int> tmpFaceIndicesList;
    std::vector<sr::Vec3f>& vertices = dataOut.vertices;
    std::vector<std::vector<unsigned int>>& faces = dataOut.faces;
    while (!filein.eof())
    {
        std::getline(filein, line);
        std::istringstream iss(line);
        char cdump;

        if (!line.compare(0, 2, "v "))
        {
            iss >> cdump;        // dump "v", then separated white-space will automatically ignored
            sr::Vec3f vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            vertices.emplace_back(vertex);
        }
        else if (!line.compare(0, 2, "f "))
        {
            tmpFaceIndicesList.clear();

            iss >> cdump;        // dump "f", then separated whie-space will automatically ignored

            unsigned int uidump;
            for (int i=0; i<3; ++i)
            {
                unsigned int idx;
                iss >> idx >> cdump >> uidump >> cdump >> uidump;
                tmpFaceIndicesList.emplace_back(idx);
            }

            faces.push_back(tmpFaceIndicesList);
        }
    }

    filein.close();
    return true;
}

SR_NAMESPACE_END
