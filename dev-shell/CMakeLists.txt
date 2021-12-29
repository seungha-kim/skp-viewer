# Target - Main
add_executable(dev-shell
        Window.cpp
        Window.h
        main.cpp)
target_link_libraries(dev-shell
        PRIVATE
        renderer
        glfw
        )
