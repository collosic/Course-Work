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
        std::cout << "init ";
        out << "init ";
        if (!incoming && !out) 
            std::cout << "error opening argument file, resuming default behaviour" << std::endl;
        else {
            while (std::getline(incoming, rawInput)) {
                const char *c = rawInput.c_str();
                if (c[0] == '#') continue;
                if (rawInput == "\n" || rawInput == "\r\n" || rawInput == "" || rawInput == "\r") { 
                    std::cout << std::endl;
                    out << std::endl;
                    continue;
                }
                std::stringstream ss(rawInput);
                while (ss >> buf) { 
                    in.push_back(buf);
                }
               std::string o = driver->interface(&in);
               out << o + " "; 
               std::cout << o + " ";
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
        if(driver->getQuit()) {
            break;
        }
   }
   delete driver;

}

















































