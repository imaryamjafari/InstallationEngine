#pragma once
#include "component.hpp"

class SystemLogger : public Observer{
public:
    void onStateChanged(const Installable* comp,
                        ComponentState oldState,
                        ComponentState newState) override;
};
