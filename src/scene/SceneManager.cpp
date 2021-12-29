#include "SceneManager.h"

SceneManager::SceneManager(const SurfaceInfo &surfaceInfo): m_scenes{Scene(surfaceInfo)} {}

const Scene &SceneManager::activeScene() const {
    return m_scenes[m_activeSceneIndex];
}

Scene &SceneManager::activeSceneMut() {
    return m_scenes[m_activeSceneIndex];
}

bool SceneManager::removeActiveScene() {
    if (m_scenes.size() <= 1) {
        return false;
    }
    m_scenes.erase(m_scenes.begin() + m_activeSceneIndex);
    if (m_scenes.size() == m_activeSceneIndex) {
        m_activeSceneIndex--;
    }
    return true;
}

void SceneManager::addScene() {
    m_scenes.push_back(m_scenes.back());
}

const std::vector<Scene> &SceneManager::scenes() {
    return m_scenes;
}

bool SceneManager::setActiveScene(int index) {
    if (0 <= index && index < m_scenes.size()) {
        m_activeSceneIndex = index;
        return true;
    }
    return false;
}

int SceneManager::activeSceneIndex() const {
    return m_activeSceneIndex;
}
