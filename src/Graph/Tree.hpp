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
        load(load)
        {}

    T load;

    Tree* parent() const      {return _parent;}
    Tree* first_child() const {return _first_child;}
    Tree* last_child() const  {return _last_child;}
    Tree* next() const        {return _next;}
    Tree* prev() const        {return _prev;}

    std::set<Tree*> make_children_set();

    void add_child(Tree* node);
    void add_children(std::set<Tree*> nodes);

    unsigned int height();
    unsigned int width(); // of the largest level
    std::vector<unsigned int> width_per_level();

    // void print_tree(const Graph *g, unsigned int tabs=0);

    // static void print_verts(const Graph &g, Cluster &c);
private:
    Tree
        *_parent,
        *_first_child,
        *_last_child,
        *_next,
        *_prev;
};



template<typename T>
std::set<Tree<T>*> Tree<T>::make_children_set() {
    std::set<Tree*> ret;
    for (Tree* node = _first_child; node != NULL; node = node->next())
        ret.insert(node);
    return ret;
}

template<typename T>
void Tree<T>::add_child(Tree<T>* node) {
    node->_parent = this;
    node->_next = NULL; node->_prev = NULL;

    if (_first_child == NULL) {
        _first_child = node;
    } else {
        _last_child->_next = node;
        node->_prev = _last_child;
    }

    _last_child = node;
}


template<typename T>
void Tree<T>::add_children(std::set<Tree<T>*> nodes) {
    for (typename std::set<Tree<T>*>::iterator n_it = nodes.begin(); n_it != nodes.end(); n_it++) {
        add_child(*n_it);
    }
}

template<typename T>
unsigned int Tree<T>::height() {
    unsigned int max = 1;
    for (Tree<T> *child = first_child(); child != NULL; child = child->next()) {
        unsigned int child_depth = child->height() + 1;
        if (child_depth > max) max = child_depth;
    }
    return max;
}

// of the largest level
template<typename T>
unsigned int Tree<T>::width() {
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
std::vector<unsigned int> Tree<T>::width_per_level() {
    std::vector<Tree<T>*> this_level, next_level;
    this_level.push_back(this);

    std::vector<unsigned int> ret(height());
    unsigned int i = 0;
    ret[i++] = 1;

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


#endif
