#include "Graph/Graph.hpp"

#include "GUI/MainGuiManager.hpp"
#include "GUI/program.hpp"

#include "GUI/Circle.hpp"

#include <iostream>
#include <vector>
using namespace std;



namespace global {
    Program *myProg;

    namespace window {
        const int width_screen_coords = 1000, height_screen_coords = 1000;
        // int width_px = 1000, height_px = 1000;
        // const char *title = "Demo";
    }
}


class Demowindow : public Window {
    void key_callback(int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            cout << "HERE!" << endl;
            // glfwSetWindowShouldClose(window, GL_TRUE);
    };
};




Graph set_up_graph() {
    Graph g;

    vector<Vertex> v;
    for (int i = 0; i < 10; i++) {
        v.push_back(g.add_vertex());
    }

    g.add_edge(v[0], v[4], 1);
    g.add_edge(v[1], v[4], 1);
    g.add_edge(v[2], v[4], 1);
    g.add_edge(v[3], v[4], 1);
    g.add_edge(v[0], v[1], 1);
    g.add_edge(v[1], v[3], 1);
    g.add_edge(v[2], v[3], 1);
    g.add_edge(v[4], v[5], 1);
    g.add_edge(v[5], v[6], 1);
    g.add_edge(v[6], v[7], 1);
    g.add_edge(v[7], v[8], 1);
    g.add_edge(v[8], v[9], 1);
    g.add_edge(v[9], v[3], 1);
    g.add_edge(v[6], v[2], 1);
    g.add_edge(v[5], v[8], 1);


    // // Perform a topological sort.
    // std::deque<int> topo_order;
    // boost::topological_sort(g, std::front_inserter(topo_order));

    // // Print the results.
    // for(std::deque<int>::const_iterator i = topo_order.begin();
    //     i != topo_order.end();
    //     ++i)
    // {
    //     cout << tasks[v] << endl;
    // }

    return g;
}



vector<gl_obj::pos_vec> verts_from_graph(Graph &g) {
    vector<gl_obj::pos_vec> v;

    for (std::pair<Vertex_iter, Vertex_iter> verts = g.vertices();
        verts.first != verts.second;
        verts.first++)
    {
        v.push_back(gl_obj::pos_vec(g[*verts.first].x, g[*verts.first].y));
    }

    return v;
}


vector<gl_obj::pos_vec> edges_from_graph(Graph &g) {
    vector<gl_obj::pos_vec> v;

    for (std::pair<Edge_iter, Edge_iter> edges = g.edges();
        edges.first != edges.second;
        edges.first++)
    {
        std::pair<Vertex, Vertex> uv = g.verts_on_edge(edges.first);
        v.push_back(gl_obj::pos_vec(g[uv.first].x, g[uv.first].y));
        v.push_back(gl_obj::pos_vec(g[uv.second].x, g[uv.second].y));
    }


    return v;
}


void display_graph(Window &wind, Graph &g) {
    std::vector<gl_obj::pos_vec> vertices = verts_from_graph(g);
    std::vector<gl_obj::pos_vec> edges = edges_from_graph(g);


    wind.set_as_context();


    // Clip the view port to match our ratio
    int width, height;
    wind.get_window_size_in_pixels(&width, &height);
    glViewport(0, 0, width, height);


    global::myProg->clearViewport();
    global::myProg->draw_graph_edges(edges);
    global::myProg->draw_graph_vertices(vertices);
    // DRAW EDGES

    // myProg->draw_colorpicking_scene();
    global::myProg->flush();

    wind.swap_buffers();
}


int main()
{
    Graph g = set_up_graph();

    g.set_layout();


    MainGuiManager mgm;

    Demowindow myWindow;
    mgm.create_window(&myWindow, global::window::width_screen_coords, global::window::height_screen_coords, "Graph Drawer", 3, 2);
    myWindow.set_as_context();


    // my stuff
    global::myProg = new Program("src/GUI/shaders/vs.glsl", "src/GUI/shaders/fs.glsl");


    display_graph(myWindow, g);


    while (!myWindow.should_close()) {
        // mgm.poll_for_events();
        mgm.wait_for_events();
    }




    return 0;
}
