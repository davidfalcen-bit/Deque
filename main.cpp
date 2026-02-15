#include "deque.h"
#include <iostream>
int main(void){
    Deque<int>stack = {1, 2, 3, 4, 5};
    for(auto &x:stack) std::cout << x << '\n';
}