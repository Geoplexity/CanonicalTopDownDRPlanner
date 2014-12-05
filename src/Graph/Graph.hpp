#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/subgraph.hpp>
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
typedef boost::undirected_graph<Vertex_Properties, Edge_Properties> Graph_Type;
typedef boost::graph_traits<Graph_Type>::vertex_descriptor Vertex_ID;
typedef boost::graph_traits<Graph_Type>::edge_descriptor Edge_ID;

typedef boost::graph_traits<Graph_Type>::vertex_iterator Vertex_Iterator;
typedef boost::graph_traits<Graph_Type>::edge_iterator Edge_Iterator;





class Graph : public Graph_Type
{
public:
    Graph() : Graph_Type() {}
    // Graph(const Graph& g);

    Vertex_ID add_vertex(Vertex_Properties vp);
    Vertex_ID add_vertex();

    Edge_ID add_edge(Vertex_ID v0, Vertex_ID v1, Edge_Properties ep);
    Edge_ID add_edge(Vertex_ID v0, Vertex_ID v1, double distance);

    std::pair<Vertex_Iterator, Vertex_Iterator> vertices() const;
    std::pair<Edge_Iterator, Edge_Iterator> edges() const;

    unsigned int num_vertices();
    unsigned int num_edges();

    std::pair<Vertex_ID, Vertex_ID> verts_on_edge(Edge_Iterator e);

    void set_layout();

    void write_to_file(const char* filename);
    void read_from_file(const char* filename);
};



typedef boost::subgraph<Graph> Subgraph_Type;



#endif
