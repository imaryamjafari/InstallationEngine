#include "logger.hpp"
#include <iostream>
using namespace std;

void SystemLogger::onStateChanged(const Installable* comp,
                                  ComponentState oldState,
                                  ComponentState newState){
    cout << "[OBSERVER] Component " << comp->getId()
         << " changed from " << stateToString(oldState)
         << " to " << stateToString(newState) << '\n';
}
