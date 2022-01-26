#pragma once

#include <string>
#include <unordered_map>
#include "AbstractReader.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;
struct aiTexture;

namespace acon {

class AssimpReader: public AbstractReader {
public:
    explicit AssimpReader(std::string_view path);
    ~AssimpReader() override;

    [[nodiscard]] bool hasObject(ObjectId id) const override;

    [[nodiscard]] std::string getObjectName(ObjectId id) const override;

    [[nodiscard]] glm::mat4 getObjectTransform(ObjectId id) const override;

    [[nodiscard]] unsigned getObjectUnitCount(ObjectId id) const override;

    [[nodiscard]] UnitId getObjectUnit(ObjectId id, int index) const override;

    [[nodiscard]] bool getObjectVisibility(ObjectId id) const override;

    [[nodiscard]] unsigned getObjectChildrenCount(ObjectId id) const override;

    [[nodiscard]] ObjectId getObjectChild(ObjectId id, int index) const override;

    [[nodiscard]] bool hasUnit(UnitId meshId) const override;

    [[nodiscard]] std::optional<MaterialId> getUnitFrontMaterial(UnitId id) const override;

    [[nodiscard]] std::optional<MaterialId> getUnitBackMaterial(UnitId id) const override;

    [[nodiscard]] unsigned getUnitTriangleCount(UnitId id) const override;

    [[nodiscard]] Triangle getUnitTriangle(UnitId id, int index) const override;

    [[nodiscard]] bool hasMaterial(MaterialId materialId) const override;

    [[nodiscard]] bool getMaterialHasColor(MaterialId materialId) const override;

    [[nodiscard]] glm::vec4 getMaterialColor(MaterialId id) const override;

    [[nodiscard]] bool getMaterialHasTexture(MaterialId materialId) const override;

    [[nodiscard]] TextureId getMaterialTexture(MaterialId materialId) const override;

    [[nodiscard]] std::unique_ptr<TextureData> copyTextureData(TextureId textureId) const override;

    [[nodiscard]] int getTextureWidth(TextureId textureId) const override;

    [[nodiscard]] int getTextureHeight(TextureId textureId) const override;

private:
    // TODO: assimp 가이드에 웬만하면 이렇게 들고 있지 말라고 해놓았음.
    // 읽기는 이렇게 읽되, 자체 자료구조로 변환하는 방식이 더 좋을듯.
    const aiScene* m_scene;

    using NodeMap = std::unordered_map<ObjectId, const aiNode*>;
    using NodeInverse = std::unordered_map<const aiNode*, ObjectId>;
    using MeshMap = std::unordered_map<UnitId, const aiMesh*>;
    using MeshInverse = std::unordered_map<const aiMesh*, UnitId>;
    using MaterialMap = std::unordered_map<MaterialId , const aiMaterial*>;
    using MaterialInverse = std::unordered_map<const aiMaterial*, MaterialId>;
    using TextureMap = std::unordered_map<TextureId, const aiTexture*>;
    using TextureInverse = std::unordered_map<const aiTexture*, TextureId>;

    NodeMap m_nodeMap;
    NodeInverse m_nodeInverse;
    MeshMap m_meshMap;
    MeshInverse m_meshInverse;
    MaterialMap m_materialMap;
    MaterialInverse m_materialInverse;
    TextureMap m_textureMap;
    TextureInverse m_textureInverse;

    unsigned m_nodeCount = ROOT_OBJECT_ID;
    unsigned m_meshCount = 0;
    unsigned m_materialCount = 0;
    unsigned m_textureCount = 0;

    void constructMap();
};

}