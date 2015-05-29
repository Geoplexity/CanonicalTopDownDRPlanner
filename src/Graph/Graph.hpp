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

    bool has_edge(Vertex_ID v0, Vertex_ID v1) const;
    Edge_ID edge(Vertex_ID v0, Vertex_ID v1) const; // can be unsafe... edge may not exist

    // v0 disappears, v1 remains
    void contract_edge(Vertex_ID v0, Vertex_ID v1);
    // bool is_series_parallel();

    std::pair<Vertex_Iterator, Vertex_Iterator> vertices() const;
    std::pair<Edge_Iterator, Edge_Iterator> edges() const;

    unsigned int degree_of_vertex(Vertex_ID v) const;
    std::set<Vertex_ID> vertices_adjacent(Vertex_ID v) const;
    std::set<Vertex_ID> vertices_adjacent(std::set<Vertex_ID> &v_set) const;

    std::pair<Vertex_ID, Vertex_ID> vertices_incident(Edge_ID e) const;

    std::set<Edge_ID> edges_incident(Vertex_ID v) const;
    std::set<Edge_ID> edges_incident(std::set<Vertex_ID> vs) const;

    std::set<Edge_ID> edges_in_induced_subgraph(std::set<Vertex_ID> vs) const;

    // assumes they're disjoint
    std::set<Edge_ID> edges_between(
        std::set<Vertex_ID> &v_set_1,
        std::set<Vertex_ID> &v_set_2) const;

    Vertex_Iterator find_vertex(const char *name) const;

    unsigned int num_vertices() const;
    unsigned int num_edges() const;

    void set_layout();
    void get_graph_in_range(float x_min, float x_max, float y_min, float y_max);

    void write_to_file(const char* filename);
    void read_from_file(const char* filename);

    void print_vertices(unsigned int indent = 0) const;
    void print_edges(unsigned int indent = 0) const;
private:
    // bool _is_series_parallel_aux();
};





class Mapped_Graph_Copy : public Graph {
public:
    Mapped_Graph_Copy(const Graph *g);

    // This "copies through"... Makes a copy of the input mapped graph copy 'g'
    // but instead of mapping to 'g' it maps to 'g.orig'
    Mapped_Graph_Copy(const Mapped_Graph_Copy &g);

    // Creates an induced subgraph on g
    Mapped_Graph_Copy(const Graph *g, std::set<Vertex_ID> &vertices);


    void add_original_vertex(Vertex_ID orig_v);
    void add_original_edge(Edge_ID orig_e);


    // input is a copy Vertex_ID
    Vertex_ID original_vertex(Vertex_ID v) const;
    std::set<Vertex_ID> original_vertices(const std::set<Vertex_ID> &vs) const;
    // input is an input Vertex_ID
    Vertex_ID copy_vertex(Vertex_ID v) const;

    // Adds every edge incident to the current subgraph, and the associated vertices.
    // Not really used anywhere, remade into grow_into, but left b/c it might
    // be useful in some other future context.
    void expand();

    // Input is a set of original vertices. If they are adjacent to any vertices
    // in the copy, the vertex and the incident edges will be added into the copy.
    // Behaves the same as expand if the input is a super set of the vertex set
    // of the orig graph diff the vertex set of the copy vertex set.
    // Of particular use in DR-planning.
    void grow_into(std::set<Vertex_ID> orig_vs);

    void print_copy_to_orig(unsigned int indent = 0);
private:
    const Graph *orig;

    std::map<Vertex_ID, Vertex_ID> orig_to_copy, copy_to_orig;
};



// typedef boost::subgraph<Graph> Subgraph_Type;



typedef std::set<Vertex_ID>::iterator Sg_Vertex_Iterator;
typedef std::set<Edge_ID>::iterator Sg_Edge_Iterator;

// An induced subgraph of the input graph. Initially empty.
class Subgraph {
public:
    Subgraph(const Graph *g);

    void induce(Vertex_Iterator begin, Vertex_Iterator end);
    void induce(const std::set<Vertex_ID> &vertices);

    void add_vertex(Vertex_ID vertex);
    void remove_vertex(Vertex_ID vertex);

    const Graph* graph() const;
    std::pair<Sg_Vertex_Iterator, Sg_Vertex_Iterator> vertices() const;
    std::pair<Sg_Edge_Iterator, Sg_Edge_Iterator> edges() const;

    std::set<Vertex_ID> vertices_adjacent(std::set<Vertex_ID> &v_set) const;

    std::pair<Vertex_ID, Vertex_ID> vertices_incident(Edge_ID e) const;

    // assumes they're disjoint
    std::vector<std::set<Vertex_ID> > edges_between(
        std::set<Vertex_ID> &v_set_1,
        std::set<Vertex_ID> &v_set_2) const;

    unsigned int num_vertices() const;
    unsigned int num_edges() const;

    const Vertex_Properties& operator[](Vertex_ID vertex) const;
private:
    const Graph *_graph;

    std::set<Vertex_ID> _vertices;
    std::set<Edge_ID> _edges;
};


#endif
