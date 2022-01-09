#pragma once

#include <memory>
#include <string_view>
#include "../engine/reader/AbstractReader.h"

namespace acon {

std::unique_ptr<AbstractReader> createReaderByPath(std::string_view path);

}
