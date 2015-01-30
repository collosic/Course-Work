#include "driver.h"
void runDriver(bool hasArgument, std::string commands);

int main(int argc, char* argv[])
{
    if (argc == 2) {
        std::string incoming(argv[1]);
        runDriver(true, incoming);
    } else if (argc == 1) {
        std::string incoming;
        runDriver(false, incoming);
    } else {
        std::cerr << "too many arguments used" << std::endl;
        return -1;
    }
    return 0;
}


void runDriver(bool hasArgument, std::string commands) {
    std::cout << "Enter a command below: " << std::endl;
    std::string rawInput, buf;
    Driver *driver = new Driver();
    vecstr in;

    if (hasArgument) { 
        std::ofstream out("11233529.txt");
        std::ifstream incoming(commands);
        //incoming.exceptions(std::ifstream::failbit|std::ifstream::badbit);
        if (!incoming && !out) 
            std::cout << "error opening argument file, resuming default behaviour" << std::endl;
        else {
            //incoming.clear();
            while (std::getline(incoming, rawInput)) {
                std::stringstream ss(rawInput);
                if (rawInput.size() == 0) 
                    continue;
                while (ss >> buf) 
                    in.push_back(buf);
               std::string o = driver->interface(&in);
               out << o.c_str() << std::endl;
               std::cout << o << std::endl;
            }
            incoming.close();
            out.close();
        }
    }

   while(true) {
        std::getline(std::cin, rawInput);
        std::stringstream ss(rawInput);
        while (ss >> buf) 
               in.push_back(buf);
        std::cout << driver->interface(&in) << std::endl;
   }
   delete driver;

}

















































