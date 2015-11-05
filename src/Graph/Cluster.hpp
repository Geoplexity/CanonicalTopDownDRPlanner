#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "Graph.hpp"

#include <set>


// // TODO: Make cluster class, take out static functions of DR_Plan
// // could be template class, with two classes that inherit

// class Cluster_e : public std::set<Edge_ID> {
// public:
//     // Cluster_e& operator=(std::set<Edge_ID> set) {
//     //     // *this = set;
//     //     swap(set);
//     //     return *this;
//     // };
//     // Cluster_e& operator=(const std::set<Edge_ID> &set) : std::set<Edge_ID>(set){
//     //     // std::set<Edge_ID>(set);
//     //     // *this = set;
//     //     // swap(set);
//     //     return *this;
//     // };
//     Cluster_e& operator=(std::set<Edge_ID> &&set) {
//         std::set<Edge_ID>::operator=(set);
//         return *this;
//     };
//     Cluster_e& operator=(const std::set<Edge_ID> &set) {
//         std::set<Edge_ID>::operator=(set);
//         return *this;
//     };
// };





typedef std::set<Vertex_ID> Cluster_v;
typedef std::set<Edge_ID>   Cluster_e;


// Operators
bool operator<(const Cluster_v &c0, const Cluster_v &c1);
bool operator<(const Cluster_e &c0, const Cluster_e &c1);
// template<typename T>
// bool operator<(const std::set<T> &s0, const std::set<T> &s1) {
//     return s0.size() < s1.size();
// }



// Output
void print_cluster(const Graph &g, Cluster_v &c);
void print_cluster(const Graph &g, Cluster_e &c);
// std::ostream& operator<<(std::ostream& os, const Cluster_v &obj);



// Other Functions
bool is_trivial_intersection(const Cluster_e &c1, const Cluster_e &c2);
unsigned int intersection_size(const Cluster_e &c1, const Cluster_e &c2);
bool is_subseteq(const Cluster_e &c1, const Cluster_e &c2); // is c1 subset of c2



#endif
