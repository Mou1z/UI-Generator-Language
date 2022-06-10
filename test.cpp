#include <iostream>
#include <regex>

using namespace std;

int main () {

    if (regex_match ("hello_11212_world", regex ("hello_\\d+_world"))) {
        cout << "Matched"<< endl; 
    }

    return 1;
}