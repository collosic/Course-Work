#include "driver.h"

int main()
{
    std::cout << "Hello, Wolrd" << std::endl;
    std::string rawInput, buf;
    Driver driver;
    vecstr in; 

    while(true) {
        std::cout << "> "; 
        std::getline(std::cin, rawInput);
        std::stringstream ss(rawInput); 
        
        while(ss >> buf) {
            in.push_back(buf);
        }

        std::cout << "> " + driver.interface(&in) << std::endl;
    }
    return 0;
}















































