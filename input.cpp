#include <iostream>
#include <string>
#include <sstream>

using namespace std;

int main(int argc, char* argv[]) {
    string type;
    string address;
    string line;

    while(cin >> type >> address) {
        getline(cin, line);

        if(type == "S" || type == "M" || type == "L") {
            int pos = address.find(",");
            address = address.substr(0, pos);

            long hexNumber;
            sscanf(address.c_str(), "%lx", &hexNumber);

            cout << hexNumber << endl;
        }
    }
}