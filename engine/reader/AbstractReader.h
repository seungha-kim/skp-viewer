#pragma once

#include <glm/glm.hpp>

using ObjectId = unsigned;
using UnitId = unsigned;
using MaterialId = unsigned;
using TextureId = unsigned;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

struct Face {
    Vertex vertices[3];
};

struct TextureData {
    const char* ptr;
    int size;
};

class AbstractReader {
public:
    virtual ~AbstractReader() = default;

    static const ObjectId ROOT_OBJECT_ID = 0;

    // Object
    [[nodiscard]] virtual bool hasObject(ObjectId id) const = 0;
    [[nodiscard]] virtual std::string getObjectName(ObjectId id) const = 0;
    [[nodiscard]] virtual glm::mat4 getObjectTransform(ObjectId id) const = 0;
    [[nodiscard]] virtual unsigned getObjectUnitCount(ObjectId id) const = 0;
    [[nodiscard]] virtual UnitId getObjectUnit(ObjectId id, int index) const = 0;
    [[nodiscard]] virtual bool getObjectVisibility(ObjectId id) const = 0;
    [[nodiscard]] virtual unsigned getObjectChildrenCount(ObjectId id) const = 0;
    [[nodiscard]] virtual ObjectId getObjectChild(ObjectId id, int index) const = 0;

    // Unit
    [[nodiscard]] virtual bool hasUnit(UnitId unitId) const = 0;
    [[nodiscard]] virtual MaterialId getUnitFrontMaterial(UnitId id) const = 0;
    [[nodiscard]] virtual MaterialId getUnitBackMaterial(UnitId id) const = 0;
    [[nodiscard]] virtual unsigned getUnitFaceCount(UnitId id) const = 0;
    [[nodiscard]] virtual Face getUnitFace(UnitId id, int index) const = 0;

    // TODO: Material - 아직 texture, color 등을 어떻게 해야 할 지 잘 모르겠다
    [[nodiscard]] virtual bool hasMaterial(MaterialId materialId) const = 0;
    [[nodiscard]] virtual glm::vec3 getMaterialDiffuseRGB(MaterialId id) const = 0;
    [[nodiscard]] virtual TextureData getMaterialTextureData(MaterialId id) const = 0;
};
