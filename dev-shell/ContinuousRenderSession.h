#pragma once

struct ContinuousRenderSession {
    float lastTime = 0.0f;
    float deltaTime = 0.0f;

    void updateTime(float currentTime) {
        if (lastTime != 0.0f) {
            deltaTime = currentTime - lastTime;
        }
        lastTime = currentTime;
    }
};
