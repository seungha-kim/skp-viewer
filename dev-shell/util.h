#pragma once

#include "../engine/reader/AbstractReader.h"
#include <memory>
#include <string_view>

namespace acon {

std::unique_ptr<AbstractReader> createReaderByPath(std::string_view path);

}
