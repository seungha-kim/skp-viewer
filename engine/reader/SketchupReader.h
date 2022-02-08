#pragma once
#include <string>
#include <unordered_map>
#include <stack>
#include <SketchUpAPI/sketchup.h>
#include "AbstractReader.h"
#include "sketchup-specialization.h"

namespace acon {

struct SketchupUnitHolder;
struct SketchupObjectHolder;
struct SketchupTextureMeta;
struct SketchupTextureMetaHolder;

struct SketchupObjectDescription {
    SUEntitiesRef entities;
    std::optional<ObjectId> parentObjectId;
    std::string name;
    glm::mat4 transform;
    std::optional<MaterialId> inheritedMaterialOpt;
    std::optional<TagId> tagIdOpt;
};

class SketchupReader: public AbstractReader {
public:
    explicit SketchupReader(std::string_view path);

    ~SketchupReader() override;

    [[nodiscard]] bool hasObject(ObjectId id) const override;

    [[nodiscard]] std::string getObjectName(ObjectId id) const override;

    [[nodiscard]] glm::mat4 getObjectTransform(ObjectId id) const override;

    [[nodiscard]] unsigned int getObjectUnitCount(ObjectId id) const override;

    [[nodiscard]] UnitId getObjectUnit(ObjectId id, int index) const override;

    [[nodiscard]] bool getObjectVisibility(ObjectId id) const override;

    [[nodiscard]] unsigned int getObjectChildrenCount(ObjectId id) const override;

    [[nodiscard]] ObjectId getObjectChild(ObjectId id, int index) const override;

    [[nodiscard]] bool hasUnit(UnitId unitId) const override;

    [[nodiscard]] std::optional<MaterialId> getUnitFrontMaterial(UnitId id) const override;

    [[nodiscard]] std::optional<MaterialId> getUnitBackMaterial(UnitId id) const override;

    [[nodiscard]] unsigned int getUnitTriangleCount(UnitId id) const override;

    [[nodiscard]] Triangle getUnitTriangle(UnitId id, int index) const override;

    [[nodiscard]] bool hasMaterial(MaterialId materialId) const override;

    [[nodiscard]] bool getMaterialHasColor(MaterialId materialId) const override;

    [[nodiscard]] glm::vec4 getMaterialColor(MaterialId id) const override;

    [[nodiscard]] bool getMaterialHasTexture(MaterialId materialId) const override;

    [[nodiscard]] TextureId getMaterialTexture(MaterialId materialId) const override;

    [[nodiscard]] std::unique_ptr<TextureData> copyTextureData(TextureId textureId) const override;

    [[nodiscard]] int getTextureWidth(TextureId textureId) const override;

    [[nodiscard]] int getTextureHeight(TextureId textureId) const override;

    [[nodiscard]] unsigned getTagCount() const override;

    [[nodiscard]] TagId getTag(int index) const override;

    [[nodiscard]] std::string getTagName(TagId id) const override;

    [[nodiscard]] unsigned getTagObjectCount(TagId id) const override;

    [[nodiscard]] ObjectId getTagObject(TagId id, int index) const override;

private:
    SUModelRef m_model;

    ObjectId m_objectCount = ROOT_OBJECT_ID;
    MaterialId m_materialCount = 0;
    UnitId m_unitCount = 0;
    TextureId m_textureCount = 0;
    TagId m_tagCount = 0;
    SULayerRef m_defaultLayerRef{};

    using GroupMap = std::unordered_map<ObjectId, SUGroupRef>;
    using GroupInverse = std::unordered_map<SUGroupRef, ObjectId>;
    using UnitMap = std::unordered_map<UnitId, int>; // TODO
    using MaterialMap = std::unordered_map<MaterialId, SUMaterialRef>;
    using MaterialInverse = std::unordered_map<SUMaterialRef, MaterialId>;
    using TextureMap = std::unordered_map<TextureId, SUTextureRef>;
    using TextureInverse = std::unordered_map<SUTextureRef, TextureId>;
    using TagList = std::vector<TagId>;
    using TagMap = std::unordered_map<TagId, SULayerRef>;
    using TagInverse = std::unordered_map<SULayerRef, TagId>;
    using TagObjects = std::unordered_map<TagId, std::vector<ObjectId>>;
    // TODO: face, edge 에도 layer 할당될 수 있음. 추후 unit 쪼개는 단위에 layer 추가해서 해결 예정.
    std::unique_ptr<SketchupUnitHolder> m_unitHolder;
    std::unique_ptr<SketchupObjectHolder> m_objectHolder;
    std::unique_ptr<SketchupTextureMetaHolder> m_textureMetaHolder;

    GroupMap m_groupMap;
    GroupInverse m_groupInverse;
    UnitMap m_unitMap;
    MaterialMap m_materialMap;
    MaterialInverse m_materialInverse;
    std::unordered_map<MaterialId, TextureId> m_materialTextures;
    std::unordered_map<MaterialId, glm::vec4> m_materialColors;
    TextureMap m_textureMap;
    TextureInverse m_textureInverse;
    TagList m_tagList;
    TagMap m_tagMap;
    TagInverse m_tagInverse;
    TagObjects m_tagObjects;
    std::stack<SketchupObjectDescription> m_dfsStack;

    ObjectId processObject(const SketchupObjectDescription& desc);
    void pushChildren(ObjectId id, SUComponentInstanceRef instance);
    [[nodiscard]] bool isValidLayer(SULayerRef layerRef) const;
};

}
