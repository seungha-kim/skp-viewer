#pragma once

#include "ContinuousRenderSession.h"
#include <optional>

struct PlaybackState {
    bool forceContinuous = false;
    bool manual = true;
    float playback = 0.0f;
    std::optional<ContinuousRenderSession> continuousRenderSession;
};