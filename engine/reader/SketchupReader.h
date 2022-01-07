#pragma once
#include <string>
#include <unordered_map>
#include <SketchUpAPI/sketchup.h>
#include "AbstractReader.h"
#include "sketchup-specialization.h"

namespace acon {

struct SketchupUnitHolder;
struct SketchupObjectHolder;
struct SketchupObjectDescription;
class SketchupReader: public AbstractReader {
public:
    explicit SketchupReader(std::string_view path);

    ~SketchupReader() override;

    [[nodiscard]] bool hasObject(ObjectId id) const override;

    [[nodiscard]] std::string getObjectName(ObjectId id) const override;

    [[nodiscard]] glm::mat4 getObjectTransform(ObjectId id) const override;

    [[nodiscard]] unsigned int getObjectUnitCount(ObjectId id) const override;

    // TODO: 리더니까 이거 그냥 vector 반환하게 바꿔도 되지 않나?
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

    [[nodiscard]] glm::vec3 getMaterialDiffuseRGB(MaterialId id) const override;

    [[nodiscard]] TextureData getMaterialTextureData(MaterialId id) const override;

private:
    SUModelRef m_model;

    ObjectId m_objectCount = ROOT_OBJECT_ID;
    MaterialId m_materialCount = 0;
    UnitId m_unitCount = 0;
    TextureId m_textureCount = 0;

    using GroupMap = std::unordered_map<ObjectId, SUGroupRef>;
    using GroupInverse = std::unordered_map<SUGroupRef, ObjectId>;
    using UnitMap = std::unordered_map<UnitId, int>; // TODO
    using MaterialMap = std::unordered_map<MaterialId, SUMaterialRef>;
    using MaterialInverse = std::unordered_map<SUMaterialRef, MaterialId>;
    using TextureMap = std::unordered_map<TextureId, SUTextureRef>;
    using TextureInverse = std::unordered_map<SUTextureRef, TextureId>;
    std::unique_ptr<SketchupUnitHolder> m_unitHolder;
    std::unique_ptr<SketchupObjectHolder> m_objectHolder;

    GroupMap m_groupMap;
    GroupInverse m_groupInverse;
    UnitMap m_unitMap;
    MaterialMap m_materialMap;
    MaterialInverse m_materialInverse;
    TextureMap m_textureMap;
    TextureInverse m_textureInverse;

    ObjectId processObject(const SketchupObjectDescription& desc);
};

}
