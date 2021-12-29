#pragma once
#include "SurfaceInfo.h"

class BasePass {
    virtual void resizeResources(const SurfaceInfo& surfaceInfo) = 0;
};


