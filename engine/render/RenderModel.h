#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include "RenderUnit.h"
#include "RenderTexture.h"
#include "../primitives.h"
#include "../build.h"

namespace acon {

struct RenderObjectData {
    std::vector<UnitId> units;
};

class RenderModel {
    friend ModelPair buildModel(const AbstractReader& reader);
public:
    RenderModel();
    ~RenderModel();
    [[nodiscard]] unsigned getObjectUnitCount(ObjectId id) const;
    [[nodiscard]] UnitId getObjectUnit(ObjectId id, int index) const;
    [[nodiscard]] const std::vector<std::unique_ptr<RenderUnit>>& units() const;
private:
    std::unordered_map<ObjectId, RenderObjectData> m_objectData{};
    std::vector<std::unique_ptr<RenderUnit>> m_units{};
    std::unordered_map<TextureId, std::unique_ptr<RenderTexture>> m_textures{};
};

}
