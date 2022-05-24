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
    LINE,
    PATH,
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

string argsBaseCode [5] = {

    "(Coordinate (\"{left}\", \"{top}\"), Dimensions (\"{width}\", \"{height}\"), {origin_x: 0.5}, {origin_y: 0.5}, StrokeData ({stroke_width: 1}, Color32 (\"{stroke_color: #000000FF}\")), FillData (Color32 (\"{fill_color: #FFFFFF00}\")))",
    "({Coordinate (\"[x#]\", \"[y#]\") ...}, StrokeData ({stroke_width: 1}, Color32 (\"{stroke_color: #000000FF}\")), FillData (Color32 (\"{fill_color: #FFFFFF00}\")))",
    "(Coordinate (\"{start_x}\", \"{start_y}\"), Coordinate (\"{end_x}\", \"{end_y}\"), StrokeData ({stroke_width: 1}, Color32 (\"{stroke_color: #000000FF}\")))",
    "({Coordinate (\"[x#]\", \"[y#]\") ...}, StrokeData ({stroke_width: 1}, Color32 (\"{stroke_color: #000000FF}\")))",
    "(Coordinate (\"{left}\", \"{top}\"), {radius}, {angle_start}, {angle_end}, StrokeData ({stroke_width: 1}, Color32 (\"{stroke_color: #000000FF}\")), FillData (Color32 (\"{fill_color: #FFFFFF00}\")))"

};

string generateCreateCode (string elementType, string elementName, string command, int lineNumber) {

    smatch userArguments;
    regex_search (command, userArguments, regex ("\\((.+)\\)"));
    vector<string> arguments = split (userArguments [1], ',');
    
    string baseCode, searchCode;
    baseCode = searchCode = argsBaseCode[getElementTypeID (elementType)];

    smatch argumentMatch;
    while (regex_search (searchCode, argumentMatch, regex ("\\{([^{}]+)\\}"))) {

        bool argumentFound = false;
        string argumentName = removeSpaces (split (argumentMatch [1], ':')[0]);

        if (string (argumentMatch[0]).find ("...") == string::npos) {

            for (int i = 0; i < arguments.size (); i++) {
                if (argumentName == removeSpaces (split (arguments[i], '=') [0])) {

                    argumentFound = true;

                    string replaceSubString = argumentMatch[0];
                    replaceSubString = regex_replace(replaceSubString, regex ("(([\\{\\}\\:]))" ), "\\$1");

                    baseCode = regex_replace (
                        baseCode, 
                        regex (replaceSubString),
                        removeSpaces (split (arguments[i], '=') [1])
                    );

                }
            }

            if (!argumentFound) {
                if (string (argumentMatch[0]).find (':') != string::npos) {
                    string replaceSubString = argumentMatch[0];
                    replaceSubString = regex_replace(replaceSubString, regex ("(([\\{\\}\\:]))" ), "\\$1");
                    baseCode = regex_replace (
                        baseCode,
                        regex (replaceSubString),
                        removeSpaces (split (argumentMatch[1], ':') [1])
                    );
                } else {
                    cout << "ERROR: Invalid number of arguments provided at line " << lineNumber + 1 << endl;
                    return "";
                }
            }

        } else {

            // Recurring (Repeated) Argument

            smatch subArgument;
            string searchString = argumentMatch[0];

            vector<string> subArguments;
            vector<string> outputParts;

            int argumentIndex = 1;

            while (regex_search (searchString, subArgument, regex ("\\[([^\\[\\]]+)\\]"))) {
                subArguments.push_back (string (subArgument[1]));
                searchString = subArgument.suffix ();
            }

            while (true) {
                int matchedArguments = 0;
                bool matchedRequiredArguments = false;
                string argumentString = regex_replace (string (argumentMatch[1]), regex("\\s\\.\\.\\."), "");
                for (int i = 0; i < subArguments.size (); i++) {
                    string searchArgument = regex_replace (string (subArguments[i]), regex ("#"), to_string (argumentIndex));
                    for (int j = 0; j < arguments.size (); j++) {
                        if (removeSpaces (split (arguments[j], '=') [0]) == searchArgument) {
                            argumentString = regex_replace (argumentString, regex ("\\[" + subArguments[i] + "\\]"), removeSpaces (split (arguments[j], '=')[1]));
                            matchedArguments++;
                        }
                    }
                }
                if (matchedArguments == subArguments.size ()) {
                    argumentIndex++;
                    matchedArguments = 0;
                    outputParts.push_back (argumentString);
                } else {
                    break;
                }
            }

            if (!outputParts.size ()) {
                cout << "ERROR: Invalid number of arguments provided at line " << lineNumber + 1 << endl;
            } else {
                string output = "{ ";
                for (int i = 0; i < outputParts.size (); i++) {
                    output += outputParts[i];
                    if (i != outputParts.size () - 1) {
                        output += ", ";
                    }
                }
                output += " }";
                string searchString = regex_replace(string (argumentMatch[0]), regex ("(([\\{\\}\\[\\]\\#\\\"\\(\\)\\.]))" ), "\\$1");
                baseCode = regex_replace (baseCode, regex (searchString), output);
            }

        }

        searchCode = argumentMatch.suffix ();
    }

    return elementType + " " + elementName + " " + baseCode + ";";
}

string getElementName (string command) {
    smatch match;
    if (regex_search (command, match, regex ("as\\s+\\`(\\w+)\\`"))) {
        return match[1];
    }
    return "";
}

string processCommand (string command, int lineNumber) {
    string outputStatement = "";
    switch (getCommandID (command.substr (0, command.find (" ")))) {
        case CREATE: {
            string elementType = getElementType (command);
            if (isValidElement (elementType)) {
                outputStatement = generateCreateCode (elementType, getElementName (command), command, lineNumber);
                cout << outputStatement << endl;
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
    int lineNumber;

    ifstream Script ("./script.ugcl");

    while (getline (Script, line)) {
        if (isValidCommand (line)) {
            processCommand (line, lineNumber);
        }
        lineNumber++;
    }

    Script.close ();

    return 1;
}