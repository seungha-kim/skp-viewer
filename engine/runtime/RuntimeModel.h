#pragma once

#include <string_view>
#include <memory>
#include <unordered_map>
#include "../primitives.h"
#include "../build.h"

namespace acon {

struct RuntimeObjectData {
    std::string name;
    glm::mat4 transform;
    bool visibility;
    std::vector<ObjectId> children;
};

class RuntimeModel {
    friend ModelPair buildModel(const AbstractReader& reader);
public:
    RuntimeModel() = default;
    ~RuntimeModel() = default;
    [[nodiscard]] bool hasObject(ObjectId id) const;
    [[nodiscard]] std::string_view getObjectName(ObjectId id) const;
    [[nodiscard]] glm::mat4 getObjectTransform(ObjectId id) const;
    [[nodiscard]] bool getObjectVisibility(ObjectId id) const;
    [[nodiscard]] unsigned getObjectChildrenCount(ObjectId id) const;
    [[nodiscard]] ObjectId getObjectChild(ObjectId id, int index) const;
private:
    std::unordered_map<ObjectId, RuntimeObjectData> m_objectData;
};

}
