#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/subgraph.hpp>
// #include <boost/graph/adjacency_list.hpp>

// for layout
#include <boost/graph/topology.hpp>
typedef boost::square_topology<>::point_type Point;



#include <set>
#include <string>



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

    void set_position(double x, double y) {
        this->x = x; this->y = y;
        point[0] = x; point[1] = y;
    }
};

// struct vertex_copier {
//   template <typename S, typename D>
//   void operator()(const S& /*src*/, D& /*dest*/) {
//     // Nothing for now, deduced types to try and just make it compile
//     // TODO: set values for pos to reflect position on grid.
//   }
// };
// void vertex_copier(Vertex_Properties v0, Vertex_Properties v1);



struct Edge_Properties {
    double length;
    // unsigned int pebbles;

    Edge_Properties() {
        length = 1;
        // pebbles = 0;
    }

    Edge_Properties(double d) {
        length = d;
        // pebbles = 0;
    }
};

// void edge_copier(Edge_Properties e0, Edge_Properties e1);


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

typedef boost::graph_traits<Graph_Type>::adjacency_iterator Graph_Adj_Iterator;



class Graph : public Graph_Type
{
public:
    Graph() : Graph_Type() {}
    // Graph(const Graph& g);

    Vertex_ID add_vertex(Vertex_Properties vp);
    Vertex_ID add_vertex();

    Edge_ID add_edge(Vertex_ID v0, Vertex_ID v1, Edge_Properties ep);
    Edge_ID add_edge(Vertex_ID v0, Vertex_ID v1, double length);

    void remove_vertex(Vertex_ID v);
    void remove_edge(Edge_ID e);
    void remove_edge(Vertex_ID v0, Vertex_ID v1);

    bool has_edge(Vertex_ID v0, Vertex_ID v1);
    Edge_ID edge(Vertex_ID v0, Vertex_ID v1); // can be unsafe... edge may not exist

    // v0 disappears, v1 remains
    void contract_edge(Vertex_ID v0, Vertex_ID v1);
    // bool is_series_parallel();

    std::pair<Vertex_Iterator, Vertex_Iterator> vertices() const;
    std::pair<Edge_Iterator, Edge_Iterator> edges() const;

    unsigned int degree_of_vertex(Vertex_ID v);
    std::set<Vertex_ID> vertices_adjacent(Vertex_ID v);
    std::set<Vertex_ID> vertices_adjacent(std::set<Vertex_ID> &v_set);

    Vertex_Iterator find_vertex(const char *name) const;

    unsigned int num_vertices() const;
    unsigned int num_edges() const;

    std::pair<Vertex_ID, Vertex_ID> verts_on_edge(Edge_ID e) const;

    void set_layout();
    void get_graph_in_range(float x_min, float x_max, float y_min, float y_max);

    void write_to_file(const char* filename);
    void read_from_file(const char* filename);

    void print_vertices(unsigned int indent = 0);
    void print_edges(unsigned int indent = 0);
private:
    // bool _is_series_parallel_aux();
};



// typedef boost::subgraph<Graph> Subgraph_Type;



typedef std::set<Vertex_ID>::iterator Sg_Vertex_Iterator;
typedef std::set<Edge_ID>::iterator Sg_Edge_Iterator;

// An induced subgraph of the input graph. Initially empty.
class Subgraph {
public:
    Subgraph(const Graph *g);

    void induce(Vertex_Iterator begin, Vertex_Iterator end);
    void induce(std::set<Vertex_ID> *vertices);

    void add_vertex(Vertex_ID vertex);
    void remove_vertex(Vertex_ID vertex);

    const Graph* graph();
    std::pair<Sg_Vertex_Iterator, Sg_Vertex_Iterator> vertices();
    std::pair<Sg_Edge_Iterator, Sg_Edge_Iterator> edges();

    std::set<Vertex_ID> vertices_adjacent(std::set<Vertex_ID> &v_set);

    // assumes they're disjoint
    std::vector<std::set<Vertex_ID> > edges_between(
        std::set<Vertex_ID> &v_set_1,
        std::set<Vertex_ID> &v_set_2);

    std::pair<Vertex_ID, Vertex_ID> verts_on_edge(Edge_ID e);

    unsigned int num_vertices();
    unsigned int num_edges();

    const Vertex_Properties& operator[](Vertex_ID vertex);
private:
    const Graph *_graph;

    std::set<Vertex_ID> _vertices;
    std::set<Edge_ID> _edges;
};


#endif
