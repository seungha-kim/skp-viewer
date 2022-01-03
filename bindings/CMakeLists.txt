# Target - Binding
pybind11_add_module(binding_test
        bindings.cpp)
target_link_libraries(binding_test
        PRIVATE
        renderer
        pybind11_headers
        python)
