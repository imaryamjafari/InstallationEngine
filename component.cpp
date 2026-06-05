#include "component.hpp"
#include <algorithm>
#include <string>
#include <sstream>
#include <utility>
#include <iostream>

using namespace std;

string trim(const string& s){
    size_t b = s.find_first_not_of(" \t\r\n");
    if(b == string::npos) return "";
    size_t e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

string joinTitle(const vector<string>& tokens, size_t from){
    if(from >= tokens.size()) return "";
    string res = tokens[from];
    for(size_t i = from + 1; i < tokens.size(); ++i){
        res += ' ';
        res += tokens[i];
    }
    return res;
}

string stateToString(ComponentState s){
    switch(s){
        case ComponentState::PENDING: return "PENDING";
        case ComponentState::INSTALLED: return "INSTALLED";
        case ComponentState::FAILED: return "FAILED";
    }
    return "PENDING";
}

Installable::Installable(string id_, string title_)
    : id(std::move(id_)), title(std::move(title_)), state(ComponentState::PENDING),
      mockFail(false), installedParentsCount(0), explicitlyInstalled(false) {}

void Installable::notify(ComponentState oldState, ComponentState newState){
    for(Observer* obs : observers){
        if(obs)
            obs->onStateChanged(this, oldState, newState);
    }
}

void Installable::setState(ComponentState newState){
    if(state == newState) return;
    ComponentState old = state;
    state = newState;
    notify(old, newState);
}

void Installable::forcePending(){
    setState(ComponentState::PENDING);
    installedParentsCount = 0;
    mockFail = false;
    explicitlyInstalled = false;
}
