#include "checkError.h"
#include <glad/glad.h>
#include <cstdio>

namespace acon {

void priv_checkError(const char* file, int line) {
    if (auto error = glGetError()) {
        printf("--GL_ERROR(%d, %#06x): \"%s:%d\"\n", error, error, file, line);
    }
}

}
