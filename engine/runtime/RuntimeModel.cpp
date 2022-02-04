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
    return m_objectData.at(id).visibility;
}

unsigned RuntimeModel::getObjectChildrenCount(ObjectId id) const {
    return m_objectData.at(id).children.size();
}

ObjectId RuntimeModel::getObjectChild(ObjectId id, int index) const {
    return m_objectData.at(id).children[index];
}

}
