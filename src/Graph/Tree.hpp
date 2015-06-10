#ifndef TREE_HPP
#define TREE_HPP

#include <set>
#include <vector>


// A simple tree, where each node stores a cluster and a flag as to whether or not it is finished
template<typename T>
class Tree {
public:
    // input graph must be independent
    Tree(T load) :
        _parent(NULL),
        _first_child(NULL), _last_child(NULL),
        _next(NULL), _prev(NULL),
        _num_children(0),
        load(load)
        {}
    ~Tree();

    T load;

    Tree* parent() const      {return _parent;}
    Tree* first_child() const {return _first_child;}
    Tree* last_child() const  {return _last_child;}
    Tree* next() const        {return _next;}
    Tree* prev() const        {return _prev;}

    Tree* first_sibling();
    Tree* last_sibling();

    unsigned int num_children() const {return _num_children;}

    void add_child(Tree* node);
    void add_children(std::set<Tree*> nodes);

    // removes children from node, and appends them to this
    void move_children_from(Tree* node);

    unsigned int height() const;
    unsigned int width() const; // of the largest level
    std::vector<unsigned int> width_per_level() const;

    std::set<Tree*> children() const;
    std::set<Tree*> ancestors() const;

    // A simple insertion sort. Need overloaded < operator
    void sort_children_descending_recursively();

    // void print_tree(const Graph *g, unsigned int tabs=0);

    // static void print_verts(const Graph &g, Cluster &c);
private:
    Tree
        *_parent,
        *_first_child,
        *_last_child,
        *_next,
        *_prev;

    unsigned int _num_children;

    void swap_children(Tree* n0, Tree* n1);

    void _sort_children_descending_recursively_aux(Tree* start_node, Tree* end_node);
};

template<typename T>
Tree<T>::~Tree() {
    Tree<T> *node, *next_node;
    for (Tree<T>* node = first_child(); node != NULL; node = next_node) {
        next_node = node->next();
        delete(node);
    }
}

template<typename T>
Tree<T>* Tree<T>::first_sibling() {
    if (parent()) return parent()->first_child();

    Tree<T>* ret;
    for (ret = this; ret->prev() != NULL; ret = ret->prev()) {}
    return ret;
}

template<typename T>
Tree<T>* Tree<T>::last_sibling() {
    if (parent()) return parent()->last_child();

    Tree<T>* ret;
    for (ret = this; ret->next() != NULL; ret = ret->next()) {}
    return ret;
}

template<typename T>
void Tree<T>::add_child(Tree<T>* node) {
    node->_parent = this;
    node->_next = NULL;

    if (!first_child()) {
        node->_prev = NULL;
        _first_child = node;
    } else {
        node->_prev = _last_child;
        last_child()->_next = node;
    }

    _last_child = node;

    _num_children++;
}


template<typename T>
void Tree<T>::add_children(std::set<Tree<T>*> nodes) {
    for (typename std::set<Tree<T>*>::iterator n_it = nodes.begin(); n_it != nodes.end(); n_it++) {
        add_child(*n_it);
        _num_children++;
    }
}

// removes children from node, and appends them to this
template<typename T>
void Tree<T>::move_children_from(Tree<T>* node) {
    if (!node->first_child())
        return;

    // append
    if (this->first_child()) {
        this->last_child()->_next = node->first_child();
        node->first_child()->_prev = this->last_child();
    } else {
        this->_first_child = node->first_child();
    }

    // change parents
    for (Tree<T>* n = node->first_child(); n != NULL; n = n->next())
        n->_parent = this;

    // change this and nodes child pointers
    this->_last_child  = node->last_child();
    node->_first_child = NULL;
    node->_last_child  = NULL;

    // change children count
    this->_num_children += node->num_children();
    node->_num_children = 0;
}

template<typename T>
unsigned int Tree<T>::height() const {
    unsigned int max = 1;
    for (Tree<T> *child = first_child(); child != NULL; child = child->next()) {
        unsigned int child_depth = child->height() + 1;
        if (child_depth > max) max = child_depth;
    }
    return max;
}

// of the largest level
template<typename T>
unsigned int Tree<T>::width() const {
    std::vector<Tree<T>*> this_level, next_level;
    unsigned int
        // this_width = 0,
        next_width = 0,
        max_width  = 0;

    this_level.push_back(this);
    max_width = 1;


    while (this_level.size() != 0) {
        for (typename std::vector<Tree<T>*>::iterator c = this_level.begin(); c != this_level.end(); c++) {
            for (Tree<T> *child = (*c)->first_child(); child != NULL; child = child->next()) {
                next_level.push_back(child);
                next_width++;
            }
        }

        if (next_width > max_width) max_width = next_width;

        this_level = next_level;

        next_level.clear();
        next_width = 0;
    }

    return max_width;
}

template<typename T>
std::vector<unsigned int> Tree<T>::width_per_level() const {
    std::vector<unsigned int> ret(height());
    unsigned int i = 0;
    ret[i++] = 1;

    std::vector<Tree<T>*> this_level, next_level;
    for (Tree<T> *child = first_child(); child != NULL; child = child->next()) {
        this_level.push_back(child);
    }
    ret[i++] = this_level.size();

    while (this_level.size() != 0) {
        unsigned int next_width = 0;
        for (typename std::vector<Tree<T>*>::iterator c = this_level.begin(); c != this_level.end(); c++) {
            for (Tree<T> *child = (*c)->first_child(); child != NULL; child = child->next()) {
                next_level.push_back(child);
                next_width++;
            }
        }

        ret[i++] = next_width;
        this_level = next_level;
        next_level.clear();
    }

    return ret;
}

template<typename T>
std::set<Tree<T>*> Tree<T>::children() const {
    std::set<Tree*> ret;
    for (Tree* node = _first_child; node != NULL; node = node->next())
        ret.insert(node);
    return ret;
}

#include <iostream>
template<typename T>
std::set<Tree<T>*> Tree<T>::ancestors() const {
    std::set<Tree<T>*> ret;
    for (Tree<T>* node = parent(); node != NULL; node = node->parent()) {
        ret.insert(node);
    }
    return ret;
}

template<typename T>
void Tree<T>::swap_children(Tree<T>* n0, Tree<T>* n1) {
    assert(n0 != NULL);
    assert(n1 != NULL);

    if (n0 == n1)
        return;

    // swap them so it actually becomes the next case
    if (n1->next() == n0) {
        Tree<T> *temp = n0;
        n0 = n1;
        n1 = temp;
    }

    // n1 follows n0
    if (n0->next() == n1) {
        Tree<T> *n0_prev = n0->prev();

        n0->_prev = n1;
        n0->_next = n1->next();
        if (n0->next()) n0->next()->_prev = n0;
        else _last_child = n0;

        n1->_next = n0;
        n1->_prev = n0_prev;
        if (n1->prev()) n1->prev()->_next = n1;
        else _first_child = n1;

        return;
    }

    // else they are not the same or adjacent
    Tree<T> *n0_prev = n0->prev();
    Tree<T> *n0_next = n0->next();

    n0->_prev = n1->prev();
    n0->_next = n1->next();
    if (n0->prev()) n0->prev()->_next = n0;
    else _first_child = n0;
    if (n0->next()) n0->next()->_prev = n0;
    else _last_child = n0;

    n1->_prev = n0_prev;
    n1->_next = n0_next;
    if (n1->prev()) n1->prev()->_next = n1;
    else _first_child = n1;
    if (n1->next()) n1->next()->_prev = n1;
    else _last_child = n1;
}


template<typename T>
void Tree<T>::sort_children_descending_recursively() {
    _sort_children_descending_recursively_aux(first_child(), last_child());
    for (Tree<T>* i = first_child(); i != NULL; i = i->next()) {
        i->sort_children_descending_recursively();
    }
}

// #include <iostream>

// template<typename T>
// void Tree<T>::_sort_children_descending_recursively_aux(Tree<T>* start_node, Tree<T>* end_node) {
//     std::cout << "aux: Here 0" << std::endl;
//     // base case
//     if (start_node == NULL || end_node == NULL || start_node == end_node){
//         return;
//     }
//     // else if (start_node->next() == end_node) {
//     //     if (start_node->load < end_node->load) {
//     //         swap_children(start_node, end_node);
//     //     }
//     //     return;
//     // }
//     std::cout << "aux: Here 1" << std::endl;

//     Tree<T> *pivot = end_node;
//     Tree<T> *store_index = start_node;
//     for (Tree<T>* i = start_node; i != end_node; i = i->next()) {
//         std::cout << "aux: Here 1.1" << std::endl;
//         for (Tree<T>* j = start_node; j != end_node; j = j->next()) {
//             std::cout << j->load.size() << " ";
//         }
//         std::cout << end_node->load.size() << std::endl;
//         std::cout << "aux: Here 1.2" << std::endl;

//         if (pivot->load < i->load) {
//             // occurs on the first swap, just need to keep track for recursive calls
//             if (store_index == start_node) {
//                 start_node = i;
//             }

//             swap_children(i, store_index);
//             // swap back pointers
//             Tree<T> *temp = store_index;
//             store_index = i;
//             i = temp;

//             store_index = store_index->next();
//         }
//         std::cout << "aux: Here 1.3" << std::endl;
//     }

//     std::cout << "aux: Here 2" << std::endl;
//     swap_children(store_index, pivot);
//     std::cout << "aux: Here 3" << std::endl;

//     _sort_children_descending_recursively_aux(start_node, pivot->prev());
//     _sort_children_descending_recursively_aux(pivot->next(), store_index);
//     std::cout << "aux: Here 4" << std::endl;
// }


// TODO: uses .size() and compares ints... doesn't work with the overloaded operator for clusters for some reason...
template<typename T>
void Tree<T>::_sort_children_descending_recursively_aux(Tree<T>* start_node, Tree<T>* end_node) {
    if (start_node == NULL || end_node == NULL || start_node == end_node){
        return;
    }

    for (Tree<T>* i = start_node->next(); i != NULL; i = i->next()) {
        Tree<T>* j = i;
        while (j->prev() != NULL && j->prev()->load.size() < j->load.size()) {
            if (j == i) i = j->prev();
            swap_children(j, j->prev());
        }
    }
}


#endif
