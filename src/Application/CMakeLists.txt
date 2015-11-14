add_library(Application
    Animated_Graph_Realization_Window.cpp
    App_Window_2D.cpp
    DRP_Display_Window.cpp
    Graph_Display_Window.cpp)
target_link_libraries(Application Graph GUI)
