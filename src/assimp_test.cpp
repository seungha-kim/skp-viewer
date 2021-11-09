#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

int main() {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile( "resources/monkey.obj",
                                              aiProcess_CalcTangentSpace       |
                                              aiProcess_Triangulate            |
                                              aiProcess_JoinIdenticalVertices  |
                                              aiProcess_SortByPType);
    std::cout << "Success" << std::endl;
}