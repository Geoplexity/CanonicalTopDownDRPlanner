#include <iostream>
#include <deque>
#include <iterator>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

// #include <boost/graph/graph_traits.hpp>
#include <boost/graph/undirected_graph.hpp>

using namespace std;

struct vertex {
    int weight;

    vertex(int w) {
        weight = w;
    }
};

struct edge {
    int weight;
};

int main()
{
    // simplified adjacency list graph
    // don't want adjacency matrix as these graphs have |E|=O(|V|)
    boost::undirected_graph<vertex, edge> g;

    vertex v0 = vertex(0);
    vertex v1 = vertex(3);

    // boost::graph_traits::vertex_descriptor v0 = g.add_vertex();
    // boost::graph_traits::vertex_descriptor v1 = g.add_vertex();

    g.add_edge(v0, v1);



    // // Create a n adjacency list, add some vertices.
    // boost::adjacency_list<void> g(num tasks);
    // boost::add_vertex(0, g);
    // boost::add_vertex(1, g);
    // boost::add_vertex(2, g);
    // boost::add_vertex(3, g);
    // boost::add_vertex(4, g);
    // boost::add_vertex(5, g);
    // boost::add_vertex(6, g);

    // // Add edges between vertices.
    // boost::add_edge(0, 3, g);
    // boost::add_edge(1, 3, g);
    // boost::add_edge(1, 4, g);
    // boost::add_edge(2, 1, g);
    // boost::add_edge(3, 5, g);
    // boost::add_edge(4, 6, g);
    // boost::add_edge(5, 6, g);

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

    return 0;
}
