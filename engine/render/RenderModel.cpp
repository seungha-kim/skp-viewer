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

const std::vector<std::unique_ptr<RenderUnit>>& RenderModel::units() const {
    return m_units;
}

void RenderModel::prepareToRender() {
    for (auto& [textureId, texture]: m_textures) {
        texture->prepareToRender();
    }
    for (auto& unit: m_units) {
        unit->prepareToRender();
    }
}

}
