#include "engine.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

static vector<string> splitTokens(const string& line){
    istringstream iss(line);
    vector<string> tokens;
    string tok;
    while(iss >> tok)
        tokens.push_back(tok);
    return tokens;
}

InstallationEngine::InstallationEngine() = default;

InstallationEngine::~InstallationEngine(){
    for(Installable* comp : allComponents) delete comp;
}

Installable* InstallationEngine::getComponent(const string& id){
    auto it = byId.find(id);
    return it == byId.end() ? nullptr : it->second;
}

void InstallationEngine::addComponent(Installable* comp){
    comp->addObserver(&logger);
    byId[comp->getId()] = comp;
    allComponents.push_back(comp);
}

void InstallationEngine::resetFailedComponents(){
    for(auto it = allComponents.rbegin(); it != allComponents.rend(); ++it){
        Installable* comp = *it;
        if(comp->getState() == ComponentState::FAILED)
            comp->forcePending();
    }
}

bool InstallationEngine::uninstallAllComponents(){
    bool notAllPending = false;
    for(Installable* comp : allComponents)
        if(comp->getState() == ComponentState::INSTALLED || comp->getState() == ComponentState::FAILED) {
            notAllPending = true;
            break;
        }

    if(!notAllPending) return false;

    for(auto it = allComponents.rbegin(); it != allComponents.rend(); ++it){
        Installable* comp = *it;
        if(comp->getState() == ComponentState::INSTALLED || comp->getState() == ComponentState::FAILED)
            comp->forcePending();
    }
    return true;
}


void InstallationEngine::CommandADD(const vector<string> tokens){
    if(tokens.size() < 4){
        cout << "ERROR: Invalid command\n";
        return;
    }
    string type = tokens[1];
    string id = tokens[2];
    string title = joinTitle(tokens, 3);
    if(title.empty()){
        cout << "ERROR: Invalid command\n";
        return;
    }
    if(byId.find(id) != byId.end()){
        cout << "ERROR: Component with ID " << id << " already exists" << '\n';
        return;
    }

    if(type == "MODULE")
        addComponent(new Module(id, title));
    else if(type == "PACKAGE")
        addComponent(new Package(id, title));
    else
        cout << "ERROR: Invalid command\n";
}


void InstallationEngine::CommandATTACH(const vector<string> tokens){
    if(tokens.size() != 3){
        cout << "ERROR: Invalid command\n";
        return;
    }

    string parentId = tokens[1];
    string childId = tokens[2];
    Installable* parent = getComponent(parentId);

    if(!parent){
        cout << "ERROR: Component " << parentId << " does not exist\n";
        return;
    }

    Installable* child = getComponent(childId);

    if(!child){
        cout << "ERROR: Component " << childId << " does not exist\n";
        return;
    }
    if(!parent->isPackage()){
        cout << "ERROR: Cannot attach to a module\n";
        return;
    }

    auto* pkg = static_cast<Package*>(parent);

    if(pkg->hasChild(childId)){
        cout << "ERROR: Component " << childId << " is already attached to " << parentId << '\n';
        return;
    }
    if(parent->getState() == ComponentState::INSTALLED){
        cout << "ERROR: Cannot attach to an already installed package\n";
        return;
    }
    pkg->addChild(child);
}

void InstallationEngine::CommandMOCK_FAIL(const vector<string> tokens){
    if(tokens.size() != 2){
        cout << "ERROR: Invalid command\n";
        return;
    }

    string id = tokens[1];
    Installable* comp = getComponent(id);

    if(!comp){
        cout << "ERROR: Component " << id << " does not exist\n";
        return;
    }
    if(comp->getState() == ComponentState::INSTALLED){
        cout << "ERROR: Component " << id << " is already installed\n";
        return;
    }
    if(comp->isMockFail()){
        cout << "ERROR: Component " << id << " is already set to fail\n";
        return;
    }
    comp->setMockFail(true);
}


void InstallationEngine::CommandRESOLVE(const vector<string> tokens){
    if(tokens.size() != 2){
        cout << "ERROR: Invalid command\n";
        return;
    }

    string id = tokens[1];
    Installable* comp = getComponent(id);

    if(!comp){
        cout << "ERROR: Component " << id << " does not exist\n";
        return;
    }
    if(!comp->isMockFail()){
        cout << "ERROR: Component " << id << " is not in a mock fail state\n";
        return;
    }
    comp->setMockFail(false);
}

void InstallationEngine::CommandINSTALL(const vector<string> tokens){
    if(tokens.size() != 2){
        cout << "ERROR: Invalid command\n";
        return;
    }

    string id = tokens[1];
    Installable* comp = getComponent(id);

    if(!comp){
        cout << "ERROR: Component " << id << " does not exist\n";
        return;
    }
    if(comp->getState() == ComponentState::INSTALLED){
        cout << "ERROR: Component " << id << " is already installed\n";
        return;
    }
    TransactionContext tx;
    if (comp->install(tx)) {
        comp->setExplicit(true);
    }
}

void InstallationEngine::CommandUNINSTALL(const vector<string> tokens){
    if(tokens.size() == 2 && tokens[1] == "-A"){
        if(!uninstallAllComponents())
            cout << "ERROR: No installed components to uninstall\n";
        return;
    }

    if(tokens.size() != 2){
        cout << "ERROR: Invalid command\n";
        return;
    }

    string id = tokens[1];
    Installable* comp = getComponent(id);

    if(!comp){
        cout << "ERROR: Component " << id << " does not exist\n";
        return;
    }
    if(comp->getState() != ComponentState::INSTALLED){
        cout << "ERROR: Component " << id << " is not currently installed\n";
        return;
    }
    if(comp->getInstalledParentsCount() > 0){
        cout << "ERROR: Component " << id << " is required by another package\n";
        return;
    }
    comp->uninstall();
}


void InstallationEngine::processCommand(const string& rawLine) {
    string line = trim(rawLine);
    if(line.empty()) return;

    vector<string> tokens = splitTokens(line);
    if(tokens.empty()) return;

    const string& cmd = tokens[0];

    if(cmd == "END"){
        if(tokens.size() != 1)
            cout << "ERROR: Invalid command\n";;
        return;
    }

    if(cmd == "ADD"){
        CommandADD(tokens);
        return;
    }

    if(cmd == "ATTACH"){
        CommandATTACH(tokens);
        return;
    }

    if(cmd == "MOCK_FAIL"){
        CommandMOCK_FAIL(tokens);
        return;
    }

    if(cmd == "RESOLVE_FAIL"){
        CommandRESOLVE(tokens);
        return;
    }

    if(cmd == "INSTALL"){
        CommandINSTALL(tokens);
        return;
    }

    if(cmd == "UNINSTALL"){
        CommandUNINSTALL(tokens);
        return;
    }

    cout << "ERROR: Invalid command\n";
}
