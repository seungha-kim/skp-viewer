# Target - Binding
pybind11_add_module(binding_test
        bindings.cpp)
target_link_libraries(binding_test
        PRIVATE
        engine
        pybind11_headers
        python)

foreach(DLL ${SKETCHUP_DLL_LIST})
    add_custom_command(
        TARGET binding_test
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DLL} $<TARGET_FILE_DIR:binding_test>
    )
endforeach()
