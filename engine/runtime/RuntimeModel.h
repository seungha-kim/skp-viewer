#pragma once

#include "../build.h"
#include "../primitives.h"
#include <memory>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace acon {

struct RuntimeObjectData {
    std::string name;
    glm::mat4 transform;
    bool visibility;
    std::vector<ObjectId> children;
    std::optional<ObjectId> parentIdOpt;
    std::optional<TagId> tagIdOpt; // TODO: multiple tags for one object
};

struct RuntimeTagData {
    TagId id;
    std::string name;
    bool visibility;
    std::unordered_set<ObjectId> objects;
};

class RuntimeModel {
    friend ModelPair buildModel(const AbstractReader& reader);

public:
    RuntimeModel() = default;
    ~RuntimeModel() = default;
    [[nodiscard]] ObjectId rootObjectId() const;
    [[nodiscard]] bool hasObject(ObjectId id) const;
    [[nodiscard]] std::string_view getObjectName(ObjectId id) const;
    [[nodiscard]] glm::mat4 getObjectTransform(ObjectId id) const;
    [[nodiscard]] bool getObjectVisibility(ObjectId id) const;
    [[nodiscard]] unsigned getObjectChildrenCount(ObjectId id) const;
    [[nodiscard]] ObjectId getObjectChild(ObjectId id, int index) const;
    [[nodiscard]] std::optional<ObjectId> getObjectParent(ObjectId id) const;

    [[nodiscard]] unsigned getTagCount() const;
    [[nodiscard]] TagId getTag(int index) const;
    [[nodiscard]] std::string_view getTagName(TagId id) const;
    [[nodiscard]] bool getTagVisibility(TagId id) const;
    void setTagVisibility(TagId id, bool visibility);
    [[nodiscard]] bool getObjectVisibilityUpdated() const;
    void clearFrameFlags();

    [[nodiscard]] std::optional<ObjectId> selectedObjectIdOpt() const;
    void updateObjectSelectionById(std::optional<ObjectId> objectIdOpt);

private:
    // TODO: object order?
    std::unordered_map<ObjectId, RuntimeObjectData> m_objectData;
    std::vector<TagId> m_tagList;
    std::unordered_map<TagId, RuntimeTagData> m_tagData;
    std::optional<ObjectId> m_selectedObjectIdOpt {};

    // TODO: 렌더링 고려사항은 여기에 없는게 맞다 - 삭제 예정
    bool m_objectVisibilityUpdated = true;
};

}
