#pragma once
#include "../build.h"
#include "../primitives.h"
#include "RenderTexture.h"
#include "RenderUnit.h"
#include <memory>
#include <unordered_map>
#include <vector>

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
    void prepareToRender();

private:
    std::unordered_map<ObjectId, RenderObjectData> m_objectData {};
    std::vector<std::unique_ptr<RenderUnit>> m_units {};
    std::unordered_map<TextureId, std::unique_ptr<RenderTexture>> m_textures {};
};

}
