//
// Created by Anhelina Modenko on 06.10.2023.
//

#ifndef MARCHING_CUBES_STATES_H
#define MARCHING_CUBES_STATES_H


class States {
    template<typename T>
    static bool indexActive(T* states, int target);

    template<typename T>
    static void activateIndex(T* states, int target);

    template<typename T>
    static void uniquelyActivateIndex(T* states, int target);

    template<typename T>
    void deactivateIndex(T* states, int target);

    template<typename T>
    static void toggleIndex(T* states, int target);

    template<typename T>
    static bool active(T* states, T state);

    template<typename T>
    static void activate(T* states, T state);

    template<typename T>
    static void uniquelyActivate(T* states, T state);

    template<typename T>
    static void deactivate(T* states, T state);

    template<typename T>
    static void toggle(T* states, T state);
};


#endif //MARCHING_CUBES_STATES_H
