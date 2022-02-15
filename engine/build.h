#pragma once

#include "reader/AbstractReader.h"
#include <memory>
#include <tuple>

namespace acon {
class RuntimeModel;
class RenderModel;
class RenderUnit;
using ModelPair = std::pair<std::unique_ptr<RuntimeModel>, std::unique_ptr<RenderModel>>;

ModelPair buildModel(const AbstractReader& reader);
}
