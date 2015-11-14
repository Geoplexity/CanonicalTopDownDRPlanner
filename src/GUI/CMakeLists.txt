add_library(GUI
    Camera_2D.cpp
    Drawer.cpp
    Main_GUI_Manager.cpp
    Shader_Program.cpp)
target_link_libraries(GUI ${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
