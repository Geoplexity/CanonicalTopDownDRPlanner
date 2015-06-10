#ifndef MIN_PRIORITY_QUEUE_HPP
#define MIN_PRIORITY_QUEUE_HPP

#include <vector>
#include <map>
// #include <iostream>

template<typename ID, typename Key>
class Min_Priority_Queue {
public:
    Min_Priority_Queue() {}

    bool empty() const;
    unsigned int size() const;

    void insert(ID id, Key key);

    void pop();
    std::pair<ID, Key> top() const;
    std::pair<ID, Key> pop_top();

    void change_key(ID id, Key new_key);

    // void print();
private:
    std::vector< std::pair<ID, Key> > heap;
    std::map<ID, unsigned int> map;

    unsigned int parent_idx(unsigned int idx);
    unsigned int left_child_idx(unsigned int idx);
    unsigned int right_child_idx(unsigned int idx);

    void swap(unsigned int a, unsigned int b);

    void erase_last();

    // assumes left and right children are a heap, but from the parent it’s not a heap
    void heapify_down(unsigned int idx);

    void heapify_up(unsigned int idx);
};



template<typename ID, typename Key>
bool Min_Priority_Queue<ID, Key>::empty() const {
    return heap.size() == 0? true: false;
}

template<typename ID, typename Key>
unsigned int Min_Priority_Queue<ID, Key>::size() const {
    return heap.size();
}

template<typename ID, typename Key>
void Min_Priority_Queue<ID, Key>::insert(ID id, Key key) {
    if (map.find(id) != map.end()) // TODO: error
        return;

    unsigned int inserted = heap.size();
    heap.push_back(std::make_pair(id, key));
    map[id] = inserted;

    heapify_up(inserted);
}

template<typename ID, typename Key>
void Min_Priority_Queue<ID, Key>::pop() {
    swap(0, heap.size()-1);
    erase_last();
    heapify_down(0);
}

template<typename ID, typename Key>
std::pair<ID, Key> Min_Priority_Queue<ID, Key>::top() const {
    return heap[0];
}

template<typename ID, typename Key>
std::pair<ID, Key> Min_Priority_Queue<ID, Key>::pop_top() {
    std::pair<ID, Key> ret = top();
    pop();
    return ret;
}

template<typename ID, typename Key>
void Min_Priority_Queue<ID, Key>::change_key(ID id, Key new_key) {
    if (map.find(id) == map.end()) // TODO: error
        return;

    unsigned int idx = map[id];

    if (new_key > heap[idx].second) {
        heap[idx].second = new_key;
        heapify_down(idx);
    } else {
        heap[idx].second = new_key;
        heapify_up(idx);
    }
}

// template<typename ID, typename Key>
// void Min_Priority_Queue<ID, Key>::print() {
//     unsigned int idx = 0;
//     for (unsigned int i = 0; i < heap.size(); i++) {
//         std::cout << "(" << i << ", " << heap[i].second << ") ";
//     }
//     std::cout << std::endl;
// }

template<typename ID, typename Key>
unsigned int Min_Priority_Queue<ID, Key>::parent_idx(unsigned int idx) {
    return ((int) (idx+1)/2) - 1;
}

template<typename ID, typename Key>
unsigned int Min_Priority_Queue<ID, Key>::left_child_idx(unsigned int idx) {
    return (int) idx*2+1;
}

template<typename ID, typename Key>
unsigned int Min_Priority_Queue<ID, Key>::right_child_idx(unsigned int idx) {
    return (int) idx*2+2;
}

template<typename ID, typename Key>
void Min_Priority_Queue<ID, Key>::swap(unsigned int a, unsigned int b) {
    std::pair<ID, Key> temp = heap[a];
    heap[a] = heap[b];
    heap[b] = temp;

    map[heap[a].first] = a;
    map[heap[b].first] = b;
}

template<typename ID, typename Key>
void Min_Priority_Queue<ID, Key>::erase_last() {
    map.erase(heap.back().first);
    heap.pop_back();
}

// assumes left and right children are a heap, but from the parent it’s not a heap
template<typename ID, typename Key>
void Min_Priority_Queue<ID, Key>::heapify_down(unsigned int idx) {
    while (true) {
        unsigned int lc = left_child_idx(idx);
        unsigned int rc = right_child_idx(idx);

        // determine which is the smallest: parent, left or right child
        unsigned int smallest = idx;
        if ( lc < heap.size()  &&  heap[lc].second <= heap[idx].second )
            smallest = lc;
        if ( rc < heap.size()  &&  heap[rc].second <= heap[smallest].second )
            smallest = rc;

        if (smallest != idx) {
            swap(idx, smallest);
            idx = smallest;
        } else {
            break;
        }
    }


    // if (smallest != idx) {
    //     swap(idx, smallest);
    //     heapify_down(smallest); // Now the child that swapped may not be a heap
    // }
}

template<typename ID, typename Key>
void Min_Priority_Queue<ID, Key>::heapify_up(unsigned int idx) {
    unsigned int parent = parent_idx(idx);
    while (idx != 0 && heap[idx].second < heap[parent].second) {
        swap(parent, idx);
        idx = parent;
        parent = parent_idx(idx);
    }
}



#endif
