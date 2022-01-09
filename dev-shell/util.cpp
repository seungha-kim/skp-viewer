#include "util.h"
#include "../engine/reader/SketchupReader.h"
#include "../engine/reader/AssimpReader.h"

namespace acon {

static inline bool ends_with(std::string_view value, std::string_view ending) {
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

std::unique_ptr<AbstractReader> createReaderByPath(std::string_view path) {
    if (ends_with(path, ".skp")) {
        return std::make_unique<SketchupReader>(path);
    } else if (ends_with(path, ".obj")) {
        return std::make_unique<AssimpReader>(path);
    } else {
        throw std::runtime_error("Not supported file format");
    }
}

}
