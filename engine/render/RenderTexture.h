#pragma once

#include "../reader/AbstractReader.h"
#include <glad/glad.h>
#include <memory>

namespace acon {

class RenderTexture {
public:
    explicit RenderTexture(std::unique_ptr<TextureData>&& data);
    ~RenderTexture();
    [[nodiscard]] GLuint textureName() const;
    void prepareToRender();

private:
    unsigned m_textureName {};
    std::unique_ptr<TextureData> m_tempData;
};

}
