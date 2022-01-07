#pragma once
#include <vector>
#include "Scene.h"
#include "../render/SurfaceInfo.h"

namespace acon {

class SceneManager {
public:
    explicit SceneManager(const SurfaceInfo& surfaceInfo);
    [[nodiscard]] const Scene& activeScene() const;
    Scene& activeSceneMut();
    int activeSceneIndex() const;
    bool removeActiveScene();
    void addScene();
    const std::vector<Scene>& scenes();
    bool setActiveScene(int index);
private:
    std::vector<Scene> m_scenes;
    int m_activeSceneIndex = 0;
};

}
