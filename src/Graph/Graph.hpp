#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/undirected_graph.hpp>
// #include <boost/graph/adjacency_list.hpp>

// for layout
#include <boost/graph/topology.hpp>
typedef boost::square_topology<>::point_type Point;




struct Graph_Properties { };


struct Vertex_Properties {
    // unsigned int pebbles;
    double x, y;
    unsigned int color;

    std::string name;

    // for layout
    Point point;

    Vertex_Properties() {
        // pebbles = 2;
        x = 0;
        y = 0;
        color = 0x000000;
    }
};


struct Edge_Properties {
    double distance;
    // unsigned int pebbles;

    Edge_Properties() {
        distance = 1;
        // pebbles = 0;
    }

    Edge_Properties(double d) {
        distance = d;
        // pebbles = 0;
    }
};

// typedef boost::adjacency_list<boost::listS,
//                 boost::listS,
//                 boost::undirectedS,
//                 Vertex_Properties,
//                 Edge_Properties,
//                 Graph_Properties,
//                 boost::listS> Graph;


// undirected is simplified adjacency list graph
// don't want adjacency matrix as these graphs have |E|=O(|V|)
typedef boost::undirected_graph<Vertex_Properties, Edge_Properties> gtype;
typedef boost::graph_traits<gtype>::vertex_descriptor Vertex;
typedef boost::graph_traits<gtype>::edge_descriptor Edge;

typedef boost::graph_traits<gtype>::vertex_iterator Vertex_iter;
typedef boost::graph_traits<gtype>::edge_iterator Edge_iter;





class Graph : public gtype
{
public:
    Vertex add_vertex();
    Edge add_edge(Vertex v0, Vertex v1, double distance);

    std::pair<Vertex_iter, Vertex_iter> vertices();
    std::pair<Edge_iter, Edge_iter> edges();

    unsigned int num_vertices();

    std::pair<Vertex, Vertex> verts_on_edge(Edge_iter e);

    void set_layout();

    void write_to_file(const char* filename);
    void read_from_file(const char* filename);
};


#endif
