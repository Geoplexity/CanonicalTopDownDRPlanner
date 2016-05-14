#ifndef MATROID_HPP
#define MATROID_HPP


#include <set>



template<typename T>
class Matroid {
public:
    Matroid(const std::set<T>& ground_set);

    bool is_independent(const std::set<T>& subset) const = 0;
    bool is_independent(const std::set<T>& subset, const T& element) const = 0;

    // a basis is an independent set that would become dependent were anything added
    bool is_basis() const;
    bool is_basis(std::set<T>& subset) const;

    unsigned int rank() const;
    unsigned int rank(std::set<T>& subset) const;
private:
    const std::set<T> &_ground_set;
    bool _ground_set_is_indpendent;
    unsigned int _ground_set_rank;

};


template<typename T>
Matroid<T>::Matroid(const std::set<T>& ground_set) :
    _ground_set(ground_set)
{ }


template<typename T>
bool Matroid<T>::is_basis() const {
    if (_ground_set.size() == rank())
        return true;
    return false;
}












#endif
