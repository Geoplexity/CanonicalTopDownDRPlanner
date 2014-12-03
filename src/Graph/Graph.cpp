// #include <boost/graph/fruchterman_reingold.hpp>
// #include <boost/graph/random_layout.hpp>
// #include <boost/graph/kamada_kawai_spring_layout.hpp>
// #include <boost/graph/circle_layout.hpp>
// #include <boost/graph/adjacency_list.hpp>
// #include <boost/graph/point_traits.hpp>
// #include <boost/random/linear_congruential.hpp>
// #include <boost/test/minimal.hpp>
// #include <iostream>
// #include <boost/limits.hpp>
// #include <fstream>
// #include <string>
// using namespace boost;

// enum vertex_position_t { vertex_position };
// namespace boost { BOOST_INSTALL_PROPERTY(vertex, position); }

// typedef square_topology<>::point_type point;

// template<typename Graph, typename PositionMap, typename Topology>
// void print_graph_layout(const Graph& g, PositionMap position, const Topology& topology)
// {
//   typedef typename Topology::point_type Point;
//   // Find min/max ranges
//   Point min_point = position[*vertices(g).first], max_point = min_point;
//   BGL_FORALL_VERTICES_T(v, g, Graph) {
//     min_point = topology.pointwise_min(min_point, position[v]);
//     max_point = topology.pointwise_max(max_point, position[v]);
//   }

//   for (int y = (int)min_point[1]; y <= (int)max_point[1]; ++y) {
//     for (int x = (int)min_point[0]; x <= (int)max_point[0]; ++x) {
//       typename graph_traits<Graph>::vertex_Iteratorator vi, vi_end;
//       // Find vertex at this position
//       typename graph_traits<Graph>::vertices_size_type index = 0;
//       for (tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi, ++index) {
//         if ((int)position[*vi][0] == x && (int)position[*vi][1] == y)
//           break;
//       }

//       if (vi == vi_end) std::cout << ' ';
//       else std::cout << (char)(index + 'A');
//     }
//     std::cout << std::endl;
//   }
// }

// template<typename Graph, typename PositionMap>
// void dump_graph_layout(std::string name, const Graph& g, PositionMap position)
// {
//   std::ofstream out((name + ".dot").c_str());
//   out << "graph " << name << " {" << std::endl;

//   typename graph_traits<Graph>::vertex_Iteratorator vi, vi_end;
//   for (tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi) {
//     out << "  n" << get(vertex_index, g, *vi) << "[ pos=\""
//         << (int)position[*vi][0] + 25 << ", " << (int)position[*vi][1] + 25
//         << "\" ];\n";
//   }

//   typename graph_traits<Graph>::edge_Iteratorator ei, ei_end;
//   for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
//     out << "  n" << get(vertex_index, g, source(*ei, g)) << " -- n"
//         << get(vertex_index, g, target(*ei, g)) << ";\n";
//   }
//   out << "}\n";
// }

// template<typename Graph>
// void
// test_circle_layout(Graph*, typename graph_traits<Graph>::vertices_size_type n)
// {
//   typedef typename graph_traits<Graph>::vertex_descriptor vertex;
//   typedef typename graph_traits<Graph>::vertex_Iteratorator vertex_Iteratorator;
//   typedef typename graph_traits<Graph>::vertices_size_type vertices_size_type;
//   typedef typename graph_traits<Graph>::edges_size_type edges_size_type;

//   Graph g(n);

//   // Initialize vertex indices
//   vertex_Iteratorator vi = vertices(g).first;
//   for (vertices_size_type i = 0; i < n; ++i, ++vi)
//     put(vertex_index, g, *vi, i);

//   circle_graph_layout(g, get(vertex_position, g), 10.0);

//   std::cout << "Regular polygon layout with " << n << " points.\n";
//   square_topology<> topology;
//   print_graph_layout(g, get(vertex_position, g), topology);
// }

// struct simple_edge
// {
//   int first, second;
// };

// struct kamada_kawai_done
// {
//   kamada_kawai_done() : last_delta() {}

//   template<typename Graph>
//   bool operator()(double delta_p,
//                   typename boost::graph_traits<Graph>::vertex_descriptor /*p*/,
//                   const Graph& /*g*/,
//                   bool global)
//   {
//     if (global) {
//       double diff = last_delta - delta_p;
//       if (diff < 0) diff = -diff;
//       last_delta = delta_p;
//       return diff < 0.01;
//     } else {
//       return delta_p < 0.01;
//     }
//   }

//   double last_delta;
// };

// template<typename Graph>
// void
// test_triangle(Graph*)
// {
//   typedef typename graph_traits<Graph>::vertex_descriptor vertex_descriptor;
//   typedef typename graph_traits<Graph>::edge_descriptor edge_descriptor;

//   Graph g;

//   vertex_descriptor u = add_vertex(g); put(vertex_index, g, u, 0);
//   vertex_descriptor v = add_vertex(g); put(vertex_index, g, v, 1);
//   vertex_descriptor w = add_vertex(g); put(vertex_index, g, w, 2);

//   edge_descriptor e1 = add_edge(u, v, g).first; put(edge_weight, g, e1, 1.0);
//   edge_descriptor e2 = add_edge(v, w, g).first; put(edge_weight, g, e2, 1.0);
//   edge_descriptor e3 = add_edge(w, u, g).first; put(edge_weight, g, e3, 1.0);

//   circle_graph_layout(g, get(vertex_position, g), 25.0);

//   bool ok = kamada_kawai_spring_layout(g,
//                                        get(vertex_position, g),
//                                        get(edge_weight, g),
//                                        square_topology<>(50.0),
//                                        side_length(50.0));
//   BOOST_CHECK(ok);

//   std::cout << "Triangle layout (Kamada-Kawai).\n";
//   print_graph_layout(g, get(vertex_position, g));
// }













#include "Graph.hpp"



// #include <boost/graph/topological_sort.hpp>


#include <boost/graph/copy.hpp>



// for layout
#include <boost/config.hpp>
// #include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/simple_point.hpp>
#include <boost/property_map/property_map.hpp>
#include <iostream>


#include <boost/graph/circle_layout.hpp>
#include <boost/graph/kamada_kawai_spring_layout.hpp>

#include <boost/graph/random_layout.hpp>
#include <boost/graph/fruchterman_reingold.hpp>



#include "boost/graph/graphviz.hpp"



#include <fstream>
#include <map>



// for layout
// typedef boost::property_map<Graph_Type, std::size_t Vertex_Properties::*>::type VertexIndexPropertyMap;
typedef boost::property_map<Graph_Type, Point Vertex_Properties::*>::type Vertex_Point_Map;
typedef boost::property_map<Graph_Type, double Edge_Properties::*>::type Edge_Distance_Map;

// typedef boost::graph_traits<Graph_Type>::vertex_descriptor VertexDescriptor;



#include <boost/graph/iteration_macros.hpp>
#include <vector>
// Graph::Graph(const Graph& other) {
//     // // boost::copy_graph<Graph, Graph>(other, *this);



//     // typedef std::map<Vertex_ID, size_t> IndexMap;
//     // IndexMap mapIndex;
//     // boost::associative_property_map<IndexMap> propmapIndex(mapIndex);

//     // int i=0;
//     // std::pair<Vertex_Iterator, Vertex_Iterator> vs = other.vertices();
//     // for (Vertex_Iterator it = vs.first; it != vs.second; it++)
//     // {
//     //     put(propmapIndex, *it, i++);
//     // }

//     // this->clear();
//     // boost::copy_graph(other, *this, vertex_index_map( propmapIndex ) );




//     this->clear();

//     std::vector<Vertex_ID> verts;

//     std::pair<Vertex_Iterator, Vertex_Iterator> vs = other.vertices();
//     for (Vertex_Iterator it = vs.first; it != vs.second; it++)
//     {
//         verts.push_back(this->add_vertex(other[*it]));
//     }

//     std::pair<Edge_Iterator, Edge_Iterator> es = other.edges();
//     for (Edge_Iterator it = es.first; it != es.second; it++)
//     {
//         std::pair<Vertex_ID, Vertex_ID> vs_on_e = verts_on_edge(it);
//         this->add_edge(vs_on_e.first, vs_on_e.second, other[*it]);
//     }
// }



Vertex_ID Graph::add_vertex(Vertex_Properties vp) {
    return this->Graph_Type::add_vertex(vp);
}

Vertex_ID Graph::add_vertex() {
    return this->add_vertex(Vertex_Properties());
}

Edge_ID Graph::add_edge(Vertex_ID v0, Vertex_ID v1, Edge_Properties ep) {
    std::pair<Edge_ID, bool> p = this->Graph_Type::add_edge(v0, v1, ep);
    Edge_ID e = p.first;
    bool success = p.second;

    if (success) {
        //
    } else {
        //
    }

    return e;
}

Edge_ID Graph::add_edge(Vertex_ID v0, Vertex_ID v1, double distance) {
    return this->add_edge(v0, v1, Edge_Properties(distance));
}

std::pair<Vertex_Iterator, Vertex_Iterator> Graph::vertices() const {
    return boost::vertices(*this);
}

std::pair<Edge_Iterator, Edge_Iterator> Graph::edges() const {
    return boost::edges(*this);
}

unsigned int Graph::num_vertices() {
    return boost::num_vertices(*this);
}

std::pair<Vertex_ID, Vertex_ID> Graph::verts_on_edge(Edge_Iterator e) {
    return std::make_pair(boost::source(*e, *this), boost::target(*e, *this));
}
// Graph* test() {return this;}



void Graph::set_layout() {
    Vertex_Point_Map vp_map = boost::get(&Vertex_Properties::point, *this);
    Edge_Distance_Map ed_map = boost::get(&Edge_Properties::distance, *this);

    boost::circle_graph_layout(*this, vp_map, 100);
    boost::kamada_kawai_spring_layout(
        *this,
        vp_map,
        ed_map,
        boost::square_topology<>(),
        boost::side_length<double>(0.4)
        // boost::layout_tolerance<>(),
        // 1,
        // vertexIdPropertyMap
        );

    // boost::random_graph_layout(*this, vp_map, boost::square_topology<>());
    // boost::fruchterman_reingold_force_directed_layout(
    //     *this,
    //     vp_map,
    //     boost::square_topology<>());


    Vertex_Iterator i, end;
    boost::tie(i, end) = this->vertices();
    double
        maxx = vp_map[*i][0],
        minx = vp_map[*i][0],
        maxy = vp_map[*i][1],
        miny = vp_map[*i][1];
    double centerx=0, centery=0;

    for (; i != end; i++) {
        double x = vp_map[*i][0];
        double y = vp_map[*i][1];

        centerx += x; centery += y;

        maxx = (x > maxx)? x: maxx;
        maxy = (y > maxy)? y: maxy;

        minx = (x < minx)? x: minx;
        miny = (y < miny)? y: miny;
    }


    centerx /= this->num_vertices();
    centery /= this->num_vertices();

    maxx -= centerx; minx -= centerx;
    maxy -= centery; miny -= centery;

    // std::cout << maxx << minx << std::endl;
    // std::cout << maxy << miny << std::endl;

    double diffx, diffy;
    diffx = (maxx > -minx)? maxx: -minx;
    diffy = (maxy > -miny)? maxy: -miny;
    double diff = (diffx > diffy)? diffx: diffy;

    // double diffx = maxx-minx, diffy = maxy-miny;
    // double diff = (diffx > diffy)? diffx/2.0: diffy/2.0;

    // std::cout << "Diff: " << diff << std::endl;



    // TODO: this is a hueristic to prevent node from hanging off the edge of display
    diff /= .9;


    for (boost::tie(i, end) = this->vertices(); i != end; i++) {
        double x = vp_map[*i][0];
        double y = vp_map[*i][1];

        (*this)[*i].x = (x - centerx)/diff;
        (*this)[*i].y = (y - centery)/diff;

        // std::cout << "X: " << (*this)[*i].x << " Y: " << (*this)[*i].y << std::endl;

    }
}


void Graph::write_to_file(const char* filename) {
    std::ofstream f(filename);
    boost::write_graphviz(f, *this);
}

// #include <boost/graph/property_maps/null_property_map.hpp>
void Graph::read_from_file(const char* filename) {
    boost::dynamic_properties dp;
    boost::property_map<Graph_Type, std::string Vertex_Properties::*>::type name = boost::get(&Vertex_Properties::name, *this);
    // boost::make_null_property<Vertex_ID, std::string>
    dp.property("node_id", name);

    std::ifstream f(filename);
    boost::read_graphviz(f, *this, dp);
}









// template <class WeightMap,class CapacityMap>
// class edge_writer {
// public:
//   edge_writer(WeightMap w, CapacityMap c) : wm(w),cm(c) {}
//   template <class Edge>
//   void operator()(ostream &out, const Edge& e) const {
//     out << "[label=\"" << wm[e] << "\", taillabel=\"" << cm[e] << "\"]";
//   }
// private:
//   WeightMap wm;
//   CapacityMap cm;
// };

// template <class WeightMap, class CapacityMap>
// inline edge_writer<WeightMap,CapacityMap>
// make_edge_writer(WeightMap w,CapacityMap c) {
//   return edge_writer<WeightMap,CapacityMap>(w,c);
// }

// ofstream dot("graph.dot");
// write_graphviz(dot, g,
//   boost::make_label_writer(boost::get(&vert::name, g)),
//   make_edge_writer(boost::get(&edge::weight,g),boost::get(&edge::capacity,g)));

