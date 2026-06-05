#pragma once
#include "component.hpp"

class Module : public Installable{
public:
    Module(std::string id, std::string title);
    bool install(TransactionContext& tx) override;
    void uninstall() override;
};
