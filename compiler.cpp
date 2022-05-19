#include <iostream>
#include <fstream>

using namespace std;

enum {
    CREATE,
    UPDATE
};

int getCommandID (string cmd) {
    if (cmd == "CREATE") return CREATE;
    if (cmd == "UPDATE") return UPDATE;
    return -100;
}

bool intToBool (int input) {
    if (input >= 1)
        return true;
    return false;
}

bool isValidCommand (string command) {
    return intToBool (
        getCommandID (command.substr (0, command.find (" "))) + 1
    );
}

string getElementType (string command) {
    command.substr (command.find ("`"), )
    return "";
}

string processCommand (string command) {
    switch (getCommandID (command.substr (0, command.find (" ")))) {
        case 0: {
            string type = getElementType (command);
            break;
        }
        case 1: {
            cout << "UPDATE" << endl;
            break;
        }
        default: {
            cout << "lol" << endl;
        }
    }
    return "";
}

int main () {

    string line;

    ifstream Script ("./script.ugcl");

    while (getline (Script, line)) {
        if (isValidCommand (line)) {
            processCommand (line);
        }
    }

    Script.close ();

    return 1;
}