#pragma once

#include <glm/glm.hpp>
#include <optional>

using ObjectId = unsigned;
using UnitId = unsigned;
using MaterialId = unsigned;
using TextureId = unsigned;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

struct Triangle {
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
    [[nodiscard]] virtual std::optional<MaterialId> getUnitFrontMaterial(UnitId id) const = 0;
    [[nodiscard]] virtual std::optional<MaterialId> getUnitBackMaterial(UnitId id) const = 0;
    [[nodiscard]] virtual unsigned getUnitTriangleCount(UnitId id) const = 0;
    [[nodiscard]] virtual Triangle getUnitTriangle(UnitId id, int index) const = 0;

    // TODO: Material - 아직 texture, color 등을 어떻게 해야 할 지 잘 모르겠다
    // TODO: multiple texture, multiple material
    [[nodiscard]] virtual bool hasMaterial(MaterialId materialId) const = 0;
    [[nodiscard]] virtual glm::vec3 getMaterialDiffuseRGB(MaterialId id) const = 0;
    [[nodiscard]] virtual TextureData getMaterialTextureData(MaterialId id) const = 0;

    // TODO: Scene, Object Visibility. Scene ㅇㅔ서 Camera 는 안 가져도라더라도 visibility 는 가져와야..
    // TODO: Layer (or Tag. Tag 가 최신 용어임)
    // TODO: Component -> 아 이거 하려면 MeshData 라는 계층을 하나 더 둬야? OK component 란 것은 없고 mesh 만 있다
    // TODO: Blender 는 mesh data 를 공유하는 정도의 component 기능이 있는데, SketchUp 은 object 까지 공유한다.
    //  -> 이 mismatch 를 어떻게 할 것인가? -> SketchUp 읽어올 때 mesh data 공유하는 정도로만 하는게 어떨까 - 이렇게 되면 이름이 다 같아질듯.
    // TODO: Component override? X 이런거 없다. Object 가 서로 다르기 때문에 Object 속성을 바꾸는게 곧 override 라고 봐도 됨
};
