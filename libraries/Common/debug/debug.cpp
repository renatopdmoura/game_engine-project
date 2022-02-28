#include "../Core.hpp"

int main(int argv, char** args){
    if(Core::initialize("Build width MSVC", 800, 800)){
        Core::shutdown();
    }
    return 0;
}