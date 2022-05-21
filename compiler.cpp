#include <iostream>
#include <fstream>
#include <regex>
#include <vector>

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

int getElementTypeID (string type) {
    if (type == "Rectangle") return RECTANGLE;
    if (type == "Polygon") return POLYGON;
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
        getElementTypeID (type) + 1
    );
}

string getElementType (string command) {
    smatch match;
    regex_search (command, match, regex ("`(\\w+)`"));
    return match [1];
}

vector<string> split (string input, char character) {
    vector<string> parts;
    string currentPart = "";
    for (int i = 0; i < input.length (); i++) {
        if (input[i] == character) {
            parts.push_back (currentPart);
            currentPart = "";
        } else {
            currentPart += input[i];
        }
    }
    parts.push_back (currentPart);
    return parts;
}

string removeSpaces (string input) {
    string output;
    for (int i = 0; i < input.length (); i++) {
        if (input[i] != ' ') {
            output += input[i];
        }
    }
    return output;
}

// {propertyName, defaultValue, classGroup}

string argsBaseCode [1] = {

    "(Coordinate (\"{left}\", \"{top}\"}), Dimensions (\"{width}\", \"{height}\"), {origin_x: 0.5}, {origin_y: 0.5}, StrokeData ({stroke_width: 1}, FillColor ({stroke_color: #000000FF})), FillData ({fill_color: #FFFFFF00}))"  

};




string getArgumentByIndex (int elementType, int index) {
    switch (elementType) {
        case RECTANGLE: return args_RECTANGLE[index][name];
    }
    return "";
}

// CREATE `Rectangle` `heheh` SET (width = 10%, height = 100, top = 50%, left = 50%, originX = 0.5, originY = 0.5) as `r1`
string generateCreateCode (string elementType, string command) {
    
    smatch userArguments;
    regex_search (command, userArguments, regex ("\\((.+)\\)"));
    vector<string> arguments = split (userArguments [1], ',');
    
    smatch argumentMatch;
    while (regex_search (command, argumentMatch, regex ("\\{.+\\}"))) {
        string baseArgument = argumentMatch[1];
    }

    return "";
}

string processCommand (string command) {
    string outputStatement = "";
    switch (getCommandID (command.substr (0, command.find (" ")))) {
        case CREATE: {
            string elementType = getElementType (command);
            if (isValidElement (elementType)) {
                outputStatement = generateCreateCode (elementType, command);
            } else {
                cout << "ERROR: The element '" << elementType << "' is not supported." << endl;
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