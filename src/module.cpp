#include "module.hpp"
#include <utility>

using namespace std;

Module::Module(string id, string title) : Installable(std::move(id), std::move(title)) {}

bool Module::install(TransactionContext& tx){
    (void)tx;
    if(state == ComponentState::INSTALLED) return true;
    if(mockFail){
        setState(ComponentState::FAILED);
        return false;
    }
    setState(ComponentState::INSTALLED);
    return true;
}

void Module::uninstall(){
    if (state == ComponentState::INSTALLED){
        setState(ComponentState::PENDING);
        explicitlyInstalled = false;
    }
}
