//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_STATES_H
#define MARCHING_CUBES_STATES_H


namespace States {
    template<typename T>
    bool indexActive(T* states, int target)  {
        return (*states & (1 << target));
    }

    template<typename T>
    void activateIndex(T *states, int target) {
        *states |= 1 << target;
    }

    template<typename T>
    void uniquelyActivateIndex(T *states, int target) {
        activateIndex<T>(states, target); // activate state first
        *states &= 1 << target; // deactivate others
    }

    template<typename T>
    void deactivateIndex(T *states, int target) {
        *states &= ~(1 << target);
    }

    template<typename T>
    void toggleIndex(T *states, int target) {
        *states ^= 1 << target;
    }

    template<typename T>
    bool active(T* states, T state) {
        return (*states & state);
    }

    template<typename T>
    void activate(T* states, T state) {
        *states |= state;
    }

    template<typename T>
    void uniquelyActivate(T* states, T state) {
        *states &= state;
    }

    template<typename T>
    void deactivate(T* states, T state) {
        *states &= ~state;
    }

    template<typename T>
    void toggle(T* states, T state) {
        *states ^= state;
    }
};


#endif //MARCHING_CUBES_STATES_H
