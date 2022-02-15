#pragma once
#include "../primitives.h"
#include <glm/glm.hpp>
#include <variant>

namespace acon {

using RenderColor = glm::vec3;
using RenderMaterial = std::variant<glm::vec3, TextureId>;

}
