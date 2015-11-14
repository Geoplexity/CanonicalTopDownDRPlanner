#include "Graph/Graph.hpp"
#include "Graph/Pebbled_Graph.hpp"
#include "Graph/Isostatic_Graph_Realizer.hpp"

#include "Application/Graph_Display_Window.hpp"
#include "Application/Animated_Graph_Realization_Window.hpp"

#include <iostream>
using namespace std;




enum runtime_options {
    ro_drp_2d = 0,
    ro_display_linkage = 1,
    ro_display_framework = 2,
    ro_omd = 3,
    ro_test = 4
};

void drp_2d(const string& dot_file);
void display_linkage(const string& dot_file);
void display_framework(const string& dot_file);
void omd(const string& dot_file);
void test(const string& dot_file);




namespace global {
    // Program *myProg;
    string root_dir_relative_to_exe_dir = "../";

    namespace dflt {
        string dot_file = root_dir_relative_to_exe_dir + "test_files/test.dot";
        runtime_options runtime_option = ro_drp_2d;
    }

    namespace window {
        const int width_screen_coords = 1000, height_screen_coords = width_screen_coords/1.618;
        // const int width_screen_coords = 1000, height_screen_coords = 1000;
        // int width_px = 1000, height_px = 1000;
        // const char *title = "Demo";
        string vertex_shader   = root_dir_relative_to_exe_dir + "src/GUI/shaders/vs.glsl";
        string fragment_shader = root_dir_relative_to_exe_dir + "src/GUI/shaders/fs.glsl";
    }
}



int main(int argc, char **argv) {
    string dot_file = global::dflt::dot_file;
    runtime_options runtime_option = global::dflt::runtime_option;

    // handle command-line arguments
    for (int i = 1; i < argc; i++) {
        string inp = argv[i];
        string flag, val;

        int eqsign = inp.find("=");
        if (eqsign+1 == inp.length()) {
            cerr << "No value for flag " << inp.substr(0, eqsign) << "." << endl;
            return 0;
        } else if (eqsign == -1) {
            flag = inp;
            val = "";
        } else {
            flag = inp.substr(0, eqsign);
            val  = inp.substr(eqsign+1, inp.length());
        }

        // compare returns 0 if the value is the same, so not ! determines if they are the same
        if (!flag.compare("-dot")) {
            if (!val.compare("")) {
                i++;
                if (i == argc) {
                    cerr << "No filename provided for flag " << flag << "." << endl;
                    return 0;
                }
                dot_file = argv[i];
            } else {
                cerr << "Invalid value for flag " << flag << "." << endl;
                return 0;
            }
        }

        else if (!flag.compare("-opt")) {
            if (!val.compare("0")) {
                runtime_option = ro_drp_2d;
            } else if (!val.compare("1")) {
                runtime_option = ro_display_linkage;
            } else if (!val.compare("2")) {
                runtime_option = ro_display_framework;
            } else if (!val.compare("3")) {
                runtime_option = ro_omd;
            } else if (!val.compare("4")) {
                runtime_option = ro_test;
            } else {
                cerr << "Invalid value for flag " << flag << "." << endl;
                return 0;
            }
        }

        // else if (!flag.compare("-nogui")) {
        //     if (!val.compare("")) {
        //         nogui = true;
        //     } else {
        //         cerr << "Invalid value for flag " << flag << "." << endl;
        //         return 0;
        //     }
        // }

        else {
            cerr << "The flag " << flag << " is not recognized." << endl;
            return 0;
        }
    }

    switch (runtime_option) {
        case ro_drp_2d: drp_2d(dot_file); break;
        case ro_display_linkage: display_linkage(dot_file); break;
        case ro_display_framework: display_framework(dot_file); break;
        case ro_omd: omd(dot_file); break;
        case ro_test: test(dot_file); break;
    }


    return 0;
}

void drp_2d(const string& dot_file) {
    cout << "Running DRP_2D on \"" << dot_file << "\"." << endl;

    // read in
    Graph g;
    g.read_from_file(dot_file.c_str());

    g.set_layout();
    g.get_graph_in_range(-0.87, 0.87, -0.87, 0.87);

    Main_GUI_Manager mgm;
    Graph_Display_Window myWindow(&mgm, &g);

    mgm.create_window(
        &myWindow,
        global::window::width_screen_coords,
        global::window::height_screen_coords,
        "Graph Drawer",
        3,
        2,
        true);
    myWindow.init_drawer(
        global::window::vertex_shader.c_str(),
        global::window::fragment_shader.c_str());

    myWindow.update_display();

    myWindow.get_drp();

    while (!myWindow.should_close()) {
        // mgm.poll_for_events();
        // cout << "About to wait." << endl;
        mgm.wait_for_events();
        // cout << "Handled." << endl;
    }
}

void display_linkage(const string& dot_file) {
    cout << "Displaying linkage \"" << dot_file << "\"." << endl;

    // read in
    Graph g;
    g.read_from_file(dot_file.c_str());

    g.set_layout();
    g.get_graph_in_range(-0.87, 0.87, -0.87, 0.87);

    Main_GUI_Manager mgm;
    Graph_Display_Window myWindow(&mgm, &g);

    mgm.create_window(
        &myWindow,
        global::window::width_screen_coords,
        global::window::height_screen_coords,
        "Graph Drawer",
        3,
        2,
        true);
    myWindow.init_drawer(
        global::window::vertex_shader.c_str(),
        global::window::fragment_shader.c_str());

    myWindow.update_display();


    while (!myWindow.should_close()) {
        mgm.wait_for_events();
    }
}

void display_framework(const string& dot_file) {
    cout << "Displaying framework \"" << dot_file << "\"." << endl;

    // read in
    Graph g;
    g.read_from_file(dot_file.c_str());

    g.get_graph_in_range(-0.87, 0.87, -0.87, 0.87);

    Main_GUI_Manager mgm;
    Graph_Display_Window myWindow(&mgm, &g);

    mgm.create_window(
        &myWindow,
        global::window::width_screen_coords,
        global::window::height_screen_coords,
        "Graph Drawer",
        3,
        2,
        true);
    myWindow.init_drawer(
        global::window::vertex_shader.c_str(),
        global::window::fragment_shader.c_str());

    myWindow.update_display();

    while (!myWindow.should_close()) {
        // mgm.poll_for_events();
        // cout << "About to wait." << endl;
        mgm.wait_for_events();
        // cout << "Handled." << endl;
    }
}

void omd(const string& dot_file) {
    cout << "Testing OMD on \"" << dot_file << "\"" << endl;

    // read in
    Graph g;
    g.read_from_file(dot_file.c_str());

    // get layout... x/y coords
    g.set_layout();
    g.get_graph_in_range(-0.87, 0.87, -0.87, 0.87);


    Main_GUI_Manager mgm;
    Animated_Graph_Realization_Window myWindow(&mgm, &g);

    mgm.create_window(
        &myWindow,
        global::window::width_screen_coords,
        global::window::height_screen_coords,
        "Graph Drawer",
        3,
        2,
        true);
    myWindow.init_drawer(
        global::window::vertex_shader.c_str(),
        global::window::fragment_shader.c_str());

    myWindow.update_display();

    while (!myWindow.should_close()) {
        mgm.wait_for_events();
    }
}

void test(const string& dot_file) {
    cout << "Testing code." << endl;

    // read in
    Graph g;
    g.read_from_file(dot_file.c_str());

    // // test output
    // Graph copy(g);
    // copy.write_to_file("test_files/out.dot");

    // get layout... x/y coords
    g.set_layout();
    g.get_graph_in_range(-0.87, 0.87, -0.87, 0.87);


    Main_GUI_Manager mgm;
    Graph_Display_Window myWindow(&mgm, &g);

    mgm.create_window(
        &myWindow,
        global::window::width_screen_coords,
        global::window::height_screen_coords,
        "Graph Drawer",
        3,
        2,
        true);
    myWindow.init_drawer(
        global::window::vertex_shader.c_str(),
        global::window::fragment_shader.c_str());

    myWindow.update_display();


    while (!myWindow.should_close()) {
        // mgm.poll_for_events();
        // cout << "About to wait." << endl;
        mgm.wait_for_events();
        // cout << "Handled." << endl;
    }
}
