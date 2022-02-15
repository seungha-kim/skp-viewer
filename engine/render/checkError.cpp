#include "checkError.h"
#include <cstdio>
#include <glad/glad.h>

namespace acon {

void priv_checkError(const char* file, int line) {
    if (auto error = glGetError()) {
        printf("--GL_ERROR(%d, %#06x): \"%s:%d\"\n", error, error, file, line);
    }
}

}
