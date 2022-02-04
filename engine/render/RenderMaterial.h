#pragma once
#include <glm/glm.hpp>
#include <variant>
#include "../primitives.h"

namespace acon {

using RenderColor = glm::vec3;
using RenderMaterial = std::variant<glm::vec3, TextureId>;

}
