#pragma once
#include "component.hpp"

class Package : public Installable{
private:
    std::vector<Installable*> children;

    void rollbackInstalledChildren(const std::vector<Installable*>& installedChildren,
                                   const std::vector<Installable*>& countedChildren);

public:
    Package(std::string id, std::string title);
    bool isPackage() const override { return true; }
    void addChild(Installable* child);
    bool hasChild(const std::string& childId) const;
    bool install(TransactionContext& tx) override;
    void uninstall() override;
};

