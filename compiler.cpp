#include <iostream>
#include <fstream>
#include <regex>

using namespace std;

enum {
    CREATE,
    UPDATE
};

enum {
    RECTANGLE,
    POLYGON,
    PATH,
    LINE,
    ARC
};

int getCommandID (string commandName) {
    if (commandName == "CREATE") return CREATE;
    if (commandName == "UPDATE") return UPDATE;
    return -1;
}

int getElementID (string type) {
    if (type == "Line") return LINE;
    if (type == "Path") return PATH;
    if (type == "Arc") return ARC;
    return -1;
}

bool intToBool (int input) {
    return (input >= 1);
}

bool isValidCommand (string commandName) {
    return intToBool (
        getCommandID (
            commandName.substr (0, commandName.find (" "))
        ) + 1
    );
}

bool isValidElement (string type) {
    return intToBool (
        getElementID (type) + 1
    );
}

string getElementType (string command) {
    smatch match;
    regex_search (command, match, regex ("`(\\w+)`"));
    return match [1];
}

string processCommand (string command) {
    switch (getCommandID (command.substr (0, command.find (" ")))) {
        case CREATE: {
            if (isValidElement (getElementType (command))) {
                // There's alot of processing to be done here and I'm too lazy what the hell is wrong with me
            } else {
                cout << "ERROR: The element '" << type << "' is not supported." << endl;
                cout << "Are you sure you're entering the name correctly ?" << endl;
            }
            break;
        }
        case UPDATE: {
            break;
        }
        default: {
            cout << "INVALID COMMAND TYPE" << endl;
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