#pragma once
#include "SurfaceInfo.h"

namespace acon {

class BasePass {
    virtual void resizeResources(const SurfaceInfo& surfaceInfo) = 0;
};

}
