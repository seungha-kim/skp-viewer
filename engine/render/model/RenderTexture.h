#pragma once

#include "../../reader/AbstractReader.h"
#include <glad/glad.h>

namespace acon {

class RenderTexture {
public:
    explicit RenderTexture(const TextureData& data);
    ~RenderTexture();
    [[nodiscard]] GLuint textureName() const;
private:
    unsigned m_textureName{};
};

}
