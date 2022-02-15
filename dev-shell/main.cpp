#include "Window.h"
#include "nfd.h"
#include "util.h"
#include <filesystem>

const unsigned int SCR_WIDTH = 1366;
const unsigned int SCR_HEIGHT = 768;

int main(int argc, char* argv[]) {
    NFD_Init();

    std::unique_ptr<acon::AbstractReader> model;

    if (argc > 1) {
        if (std::filesystem::is_regular_file(argv[1])) {
            model = acon::createReaderByPath(argv[1]);
        } else {
            puts("Error: Given path is not a regular file");
            return 1;
        }
    } else {
        nfdchar_t* outPath;
        nfdfilteritem_t filterItem[2] = {
            {"SketchUp", "skp"},
            {"Wavefront OBJ", "obj"},
        };
        nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 2, NULL);
        if (result == NFD_OKAY) {
            model = acon::createReaderByPath(outPath);
            NFD_FreePath(outPath);
        } else if (result == NFD_CANCEL) {
            puts("Error: User pressed cancel.");
            return 1;
        } else {
            printf("Error: %s\n", NFD_GetError());
            return 1;
        }
    }
    Window window {SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", *model};
    window.mainLoop();
    NFD_Quit();
    return 0;
}
