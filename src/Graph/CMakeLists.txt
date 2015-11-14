add_library(Graph
    Cluster.cpp
    DR_Plan.cpp
    Graph.cpp
    Isostatic_Graph_Realizer.cpp
    Pebbled_Graph.cpp)
target_link_libraries(Graph ${Boost_LIBRARIES})
