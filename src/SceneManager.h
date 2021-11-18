#pragma once
#include <vector>
#include "Scene.h"

class SceneManager {
public:
    [[nodiscard]] const Scene& activeScene() const;
    Scene& activeSceneMut();
    int activeSceneIndex() const;
    bool removeActiveScene();
    void addScene();
    const std::vector<Scene>& scenes();
    bool setActiveScene(int index);
private:
    std::vector<Scene> m_scenes = {Scene()};
    int m_activeSceneIndex = 0;
};
