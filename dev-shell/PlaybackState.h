#pragma once

#include <optional>
#include "ContinuousRenderSession.h"

struct PlaybackState {
    bool forceContinuous = false;
    bool manual = true;
    float playback = 0.0f;
    std::optional<ContinuousRenderSession> continuousRenderSession;
};