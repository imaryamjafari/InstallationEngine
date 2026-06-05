#pragma once
#include <string>
#include <vector>

class Installable;

struct TransactionContext{
    std::vector<Installable*> stateChangedNodes;
    std::vector<Installable*> countIncreasedNodes;
};

enum class ComponentState{
    PENDING,
    INSTALLED,
    FAILED
};

std::string stateToString(ComponentState s);
std::string trim(const std::string& s);
std::string joinTitle(const std::vector<std::string>& tokens, std::size_t from);

class Observer{
public:
    virtual void onStateChanged(const Installable* comp,
                                ComponentState oldState,
                                ComponentState newState) = 0;
    virtual ~Observer() = default;
};

class Installable{
protected:
    std::string id;
    std::string title;
    ComponentState state;
    bool mockFail;
    int installedParentsCount;
    bool explicitlyInstalled;
    std::vector<Observer*> observers;

    void notify(ComponentState oldState, ComponentState newState);

public:
    Installable(std::string id, std::string title);
    virtual ~Installable() = default;

    void setState(ComponentState newState);
    const std::string& getId() const { return id; }
    const std::string& getTitle() const { return title; }
    ComponentState getState() const { return state; }
    bool isMockFail() const { return mockFail; }
    void setMockFail(bool val) { mockFail = val; }
    int getInstalledParentsCount() const { return installedParentsCount; }
    void incParents() { ++installedParentsCount; }
    void decParents() { if (installedParentsCount > 0) --installedParentsCount; }
    bool isExplicit() const { return explicitlyInstalled; }
    void setExplicit(bool val) { explicitlyInstalled = val; }
    void addObserver(Observer* obs) { observers.push_back(obs); }

    virtual bool isPackage() const { return false; }
    virtual bool install(TransactionContext& tx) = 0;
    virtual void uninstall() = 0;
    virtual void forcePending();
};

