#include "RuntimeModel.h"

namespace acon {

bool RuntimeModel::hasObject(ObjectId id) const {
    return m_objectData.contains(id);
}

std::string_view RuntimeModel::getObjectName(ObjectId id) const {
    return m_objectData.at(id).name;
}

glm::mat4 RuntimeModel::getObjectTransform(ObjectId id) const {
    return m_objectData.at(id).transform;
}

bool RuntimeModel::getObjectVisibility(ObjectId id) const {
    if (!m_objectData.at(id).visibility) {
        return false;
    }

    std::optional<ObjectId> currentIdOpt = id;
    while (currentIdOpt) {
        auto currentId = currentIdOpt.value();
        if (m_objectData.at(currentId).tagIdOpt) {
            const auto tagId = m_objectData.at(currentId).tagIdOpt.value();
            if (!m_tagData.at(tagId).visibility) {
                return false;
            }
        }
        currentIdOpt = m_objectData.at(currentId).parentIdOpt;
    }
    return true;
}

unsigned RuntimeModel::getObjectChildrenCount(ObjectId id) const {
    return m_objectData.at(id).children.size();
}

ObjectId RuntimeModel::getObjectChild(ObjectId id, int index) const {
    return m_objectData.at(id).children[index];
}

unsigned RuntimeModel::getTagCount() const {
    return m_tagList.size();
}

TagId RuntimeModel::getTag(int index) const {
    return m_tagList[index];
}

std::string_view RuntimeModel::getTagName(TagId id) const {
    return m_tagData.at(id).name;
}

bool RuntimeModel::getTagVisibility(TagId id) const {
    return m_tagData.at(id).visibility;
}

void RuntimeModel::setTagVisibility(TagId id, bool visibility) {
    m_tagData.at(id).visibility = visibility;
}

}
