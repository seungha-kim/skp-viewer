#pragma once
#include "../render/SurfaceInfo.h"
#include "Scene.h"
#include <vector>

namespace acon {

class SceneManager {
public:
    explicit SceneManager();
    [[nodiscard]] const Scene& activeScene() const;
    Scene& activeSceneMut();
    int activeSceneIndex() const;
    bool removeActiveScene();
    void addScene();
    const std::vector<Scene>& scenes();
    bool setActiveScene(int index);
    void updateAspectRatio(const SurfaceInfo& surfaceInfo);

private:
    std::vector<Scene> m_scenes;
    int m_activeSceneIndex = 0;
};

}
