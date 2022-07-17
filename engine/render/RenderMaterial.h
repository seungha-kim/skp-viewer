#pragma once
#include "../primitives.h"
#include <glm/glm.hpp>
#include <optional>

namespace acon {

struct RenderMaterial {
    glm::vec3 color = glm::vec3(0.7f, 0.7f, 0.7f);
    std::optional<TextureId> textureOpt;
    float opacity = 1.0f;
    bool use_opacity = false;
};

}
