#include "package.hpp"
#include <utility>

using namespace std;

Package::Package(string id, string title) : Installable(std::move(id), std::move(title)) {}

void Package::addChild(Installable* child) {
    children.push_back(child);
}

bool Package::hasChild(const string& childId) const {
    for (auto* child : children) {
        if (child->getId() == childId) return true;
    }
    return false;
}

void Package::rollbackInstalledChildren(const vector<Installable*>& installedChildren,
                                        const vector<Installable*>& countedChildren){
    for(auto it = countedChildren.rbegin(); it != countedChildren.rend(); ++it)
        (*it)->decParents();
    
    for(auto it = installedChildren.rbegin(); it != installedChildren.rend(); ++it){
        Installable* child = *it;
        if(child->getState() == ComponentState::INSTALLED &&
           child->getInstalledParentsCount() == 0 &&
           !child->isExplicit()){
            child->uninstall();
        }
    }
}

bool Package::install(TransactionContext& tx){
    if(state == ComponentState::INSTALLED) return true;
    if(mockFail){
        setState(ComponentState::FAILED);
        return false;
    }

    vector<Installable*> installedChildren;
    vector<Installable*> countedChildren;

    for(auto* child : children){
        if (child->getState() == ComponentState::INSTALLED){
            child->incParents();
            countedChildren.push_back(child);
            continue;
        }

        if(!child->install(tx)){
            rollbackInstalledChildren(installedChildren, countedChildren);
            setState(ComponentState::FAILED);
            return false;
        }

        child->incParents();
        countedChildren.push_back(child);
        installedChildren.push_back(child);
    }

    setState(ComponentState::INSTALLED);
    return true;
}

void Package::uninstall(){
    if(state != ComponentState::INSTALLED) return;
    setState(ComponentState::PENDING);
    explicitlyInstalled = false;

    for(auto it = children.rbegin(); it != children.rend(); ++it){
        Installable* child = *it;
        child->decParents();
        if(child->getState() == ComponentState::INSTALLED &&
           child->getInstalledParentsCount() == 0 &&
           !child->isExplicit()) {
           child->uninstall();
        }
    }
}
