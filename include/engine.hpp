#pragma once
#include <unordered_map>
#include "logger.hpp"
#include "module.hpp"
#include "package.hpp"

class InstallationEngine {
private:
    std::vector<Installable*> allComponents;
    std::unordered_map<std::string, Installable*> byId;
    SystemLogger logger;

    Installable* getComponent(const std::string& id);
    void addComponent(Installable* comp);
    bool uninstallAllComponents();
    void resetFailedComponents();
    void CommandADD(const std::vector<std::string> tokens);
    void CommandATTACH(const std::vector<std::string> tokens);
    void CommandMOCK_FAIL(const std::vector<std::string> tokens);
    void CommandRESOLVE(const std::vector<std::string> tokens);
    void CommandINSTALL(const std::vector<std::string> tokens);
    void CommandUNINSTALL(const std::vector<std::string> tokens);

public:
    InstallationEngine();
    ~InstallationEngine();
    void processCommand(const std::string& line);
};

