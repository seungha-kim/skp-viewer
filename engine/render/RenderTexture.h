#pragma once

#include <memory>
#include "../reader/AbstractReader.h"
#include <glad/glad.h>

namespace acon {

class RenderTexture {
public:
    explicit RenderTexture(std::unique_ptr<TextureData>&& data);
    ~RenderTexture();
    [[nodiscard]] GLuint textureName() const;
    void prepareToRender();
private:
    unsigned m_textureName{};
    std::unique_ptr<TextureData> m_tempData;
};

}
