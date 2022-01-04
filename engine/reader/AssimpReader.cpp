#include "AssimpReader.h"
#include <stack>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>

static glm::mat4 convertMatrix4(const aiMatrix4x4& mat);
static glm::vec3 convertVec3(const aiVector3D& vec);
static glm::vec2 convertVec3to2(const aiVector3D& vec);
static glm::vec2 convertVec2(const aiVector2D& vec);

AssimpReader::AssimpReader(std::string_view path) {

    Assimp::Importer importer;
    importer.ReadFile(path.data(),
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);
    // to take ownership
    m_scene = importer.GetOrphanedScene();
    constructMap();
}

AssimpReader::~AssimpReader() {
    delete m_scene;
}

bool AssimpReader::hasObject(ObjectId id) const {
    return m_nodeMap.count(id) > 0;
}

std::string AssimpReader::getObjectName(ObjectId id) const {
    const auto* node = m_nodeMap.at(id);
    return {node->mName.data, node->mName.length};
}

glm::mat4 AssimpReader::getObjectTransform(ObjectId id) const {
    const auto* node = m_nodeMap.at(id);
    return convertMatrix4(node->mTransformation);
}

unsigned AssimpReader::getObjectUnitCount(ObjectId id) const {
    const auto* node = m_nodeMap.at(id);
    return node->mNumMeshes;
}

UnitId AssimpReader::getObjectUnit(ObjectId id, int index) const {
    const auto* node = m_nodeMap.at(id);
    const auto* mesh = m_scene->mMeshes[node->mMeshes[index]];
    return m_meshInverse.at(mesh);
}

bool AssimpReader::getObjectVisibility(ObjectId id) const {
    return true;
}

unsigned AssimpReader::getObjectChildrenCount(ObjectId id) const {
    const auto* node = m_nodeMap.at(id);
    return node->mNumChildren;
}

ObjectId AssimpReader::getObjectChild(ObjectId id, int index) const {
    const auto* node = m_nodeMap.at(id);
    const auto* child = node->mChildren[index];
    return m_nodeInverse.at(child);
}

bool AssimpReader::hasUnit(UnitId meshId) const {
    return m_meshMap.count(meshId) > 0;
}

MaterialId AssimpReader::getUnitFrontMaterial(UnitId id) const {
    const auto* mesh = m_meshMap.at(id);
    const auto* material = m_scene->mMaterials[mesh->mMaterialIndex];
    return m_materialInverse.at(material);
}

MaterialId AssimpReader::getUnitBackMaterial(UnitId id) const {
    return getUnitFrontMaterial(id);
}

bool AssimpReader::hasMaterial(MaterialId materialId) const {
    // TODO: Material 좀 더 알아보기: https://assimp-docs.readthedocs.io/en/master/usage/use_the_lib.html#material-system
    return false;
}

glm::vec3 AssimpReader::getMaterialDiffuseRGB(MaterialId id) const {
    // TODO
    return glm::vec3();
}

TextureData AssimpReader::getMaterialTextureData(MaterialId id) const {
    // TODO
    return TextureData();
}

void AssimpReader::constructMap() {
    for (int i = 0; i < m_scene->mNumMaterials; i++) {
        m_materialMap[m_materialCount] = m_scene->mMaterials[i];
        m_materialInverse[m_scene->mMaterials[i]] = m_materialCount;
        m_materialCount += 1;
    }

    for (int i = 0; i < m_scene->mNumTextures; i++) {
        m_textureMap[m_textureCount] = m_scene->mTextures[i];
        m_textureInverse[m_scene->mTextures[i]] = m_textureCount;
        m_textureCount += 1;
    }

    for (int i = 0; i < m_scene->mNumMeshes; i++) {
        m_meshMap[m_meshCount] = m_scene->mMeshes[i];
        m_meshInverse[m_scene->mMeshes[i]] = m_meshCount;
        m_meshCount += 1;
    }

    std::stack<aiNode*> dfsStack({m_scene->mRootNode});

    while (!dfsStack.empty()) {
        const auto* node = dfsStack.top();
        dfsStack.pop();
        m_nodeMap[m_nodeCount] = node;
        m_nodeInverse[node] = m_nodeCount;
        m_nodeCount += 1;
        for (int i = 0; i < node->mNumChildren; i++) {
            dfsStack.push(node->mChildren[i]);
        }
    }
}

unsigned AssimpReader::getUnitFaceCount(UnitId id) const {
    const auto* mesh = m_meshMap.at(id);
    return mesh->mNumFaces;
}

Face AssimpReader::getUnitFace(UnitId id, int index) const {
    const auto* mesh = m_meshMap.at(id);
    const auto face = mesh->mFaces[index];

    Face result { .vertices = {} };

    assert(face.mNumIndices == 3);
    for (int i = 0; i < face.mNumIndices; i++) {
        result.vertices[i].position = convertVec3(mesh->mVertices[face.mIndices[i]]);
    }

    if (mesh->mNormals != nullptr) {
        for (int i = 0; i < face.mNumIndices; i++) {
            result.vertices[i].normal = convertVec3(mesh->mNormals[face.mIndices[i]]);
        }
    } else {
        for (int i = 0; i < face.mNumIndices; i++) {
            result.vertices[i].normal = glm::vec3();
        }
    }

    // TODO: multiple texture
    if (mesh->mTextureCoords[0]) {
        for (int i = 0; i < face.mNumIndices; i++) {
            result.vertices[i].texCoord = convertVec3to2(mesh->mTextureCoords[0][face.mIndices[i]]);
        }
    } else {
        for (int i = 0; i < face.mNumIndices; i++) {
            result.vertices[i].texCoord = convertVec3to2(mesh->mTextureCoords[0][face.mIndices[i]]);
        }
    }

    return result;
}

glm::mat4 convertMatrix4(const aiMatrix4x4 &mat) {
    float arr[] {
        mat.a1, mat.b1, mat.c1, mat.d1,
        mat.a2, mat.b2, mat.c2, mat.d2,
        mat.a3, mat.b3, mat.c3, mat.d3,
        mat.a4, mat.b4, mat.c4, mat.d4,
    };
    return glm::make_mat4(arr);
}

static glm::vec3 convertVec3(const aiVector3D& vec) {
    return {vec.x, vec.y, vec.z};
}

static glm::vec2 convertVec3to2(const aiVector3D& vec) {
    return {vec.x, vec.y};
}

static glm::vec2 convertVec2(const aiVector2D& vec) {
    return {vec.x, vec.y};
}
