#include "RenderModel.h"

namespace acon {

RenderModel::RenderModel() = default;

RenderModel::~RenderModel() = default;

unsigned RenderModel::getObjectUnitCount(ObjectId id) const {
    return m_objectData.at(id).units.size();
}

UnitId RenderModel::getObjectUnit(ObjectId id, int index) const {
    return m_objectData.at(id).units[index];
}

const std::vector<std::unique_ptr<RenderMesh>> &RenderModel::meshes() const {
    return m_meshes;
}

}
