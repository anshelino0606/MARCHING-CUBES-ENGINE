//
// Created by Anhelina Modenko on 06.10.2023.
//

#include "States.h"

template<typename T>
bool States::indexActive(T* states, int target)  {
    return (*states & (1 << target));
}

template<typename T>
void States::activateIndex(T *states, int target) {
    *states |= 1 << target;
}

template<typename T>
void States::uniquelyActivateIndex(T *states, int target) {
    activateIndex<T>(states, target); // activate state first
    *states &= 1 << target; // deactivate others
}

template<typename T>
void States::deactivateIndex(T *states, int target) {
    *states &= ~(1 << target);
}

template<typename T>
void States::toggleIndex(T *states, int target) {
    *states ^= 1 << target;
}

template<typename T>
bool States::active(T* states, T state) {
    return (*states & state);
}

template<typename T>
void States::activate(T* states, T state) {
    *states |= state;
}

template<typename T>
void States::uniquelyActivate(T* states, T state) {
    *states &= state;
}

template<typename T>
void States::deactivate(T* states, T state) {
    *states &= ~state;
}

template<typename T>
void States::toggle(T* states, T state) {
    *states ^= state;
}