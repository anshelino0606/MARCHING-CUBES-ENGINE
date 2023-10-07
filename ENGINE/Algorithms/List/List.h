//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_LIST_H
#define MARCHING_CUBES_LIST_H

#include <vector>

namespace List {
    template<typename T>
    int getIndexOf(std::vector<T> v, T x) {
        return std::find(v.begin(), v.end(), x) - v.begin();
    }

    template<typename T>
    bool contains(std::vector<T> v, T x) {
        return std::find(v.begin(), v.end(), x) != v.end();
    }
}


#endif //MARCHING_CUBES_LIST_H
