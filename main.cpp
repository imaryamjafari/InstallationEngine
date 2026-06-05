#include "engine.hpp"
#include <iostream>
#include <string>

using namespace std;

/*
____________Naming Convention Guideline____________
Function Names: camelCase
Variable Names: camelCase
Constant Names: UPPER_SNAKE_CASE
Class Names: PascalCase
*/

int main(){
    InstallationEngine engine;
    string line;

    while(getline(cin, line)){
        string trimmed = trim(line);
        if(trimmed == "END"){
            break;
        }
        engine.processCommand(line);
    }
    return 0;
}
