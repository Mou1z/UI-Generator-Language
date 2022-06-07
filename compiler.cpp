#include <iostream>
#include <fstream>
#include <regex>
#include <vector>

using namespace std;

vector<string> createdElements;

string removeSpaces (string input) {
    string output;
    for (int i = 0; i < input.length (); i++) {
        if (input[i] != ' ') {
            output += input[i];
        }
    }
    return output;
}

string removeTabs (string input) {
    string output;
    for (int i = 0; i < input.length (); i++) {
        if (input[i] != '\t') {
            output += input[i];
        }
    }
    return output;
}

string removeWhitespace (string input) {
    string output;
    for (int i = 0; i < input.length (); i++) {
        if (input[i] != ' ' && input[i] != '\t') {
            output += input[i];
        }
    }
    return output;
}

enum {
    CREATE,
    UPDATE
};

// Element Types Enumerator
enum {
    RECTANGLE,
    POLYGON,
    LINE,
    PATH,
    ARC
};

// Events Enumerator
enum {
    OnShapeClick,
    
    OnKeyPress,
    OnKeyRelease,

    OnMouseMove,
    OnMouseClick,

    OnMouseUp,
    OnMouseDown
};

// Set all elements of the array to an empty strings.
string eventsCode [7] = { "" };

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

int getEventTypeID (string type) {
    if (type == "OnShapeClick") return OnShapeClick;

    if (type == "OnKeyPress") return OnKeyPress;
    if (type == "OnKeyRelease") return OnKeyRelease;

    if (type == "OnMouseMove") return OnMouseMove;
    if (type == "OnMouseClick") return OnMouseClick;

    if (type == "OnMouseUp") return OnMouseUp;
    if (type == "OnMouseDown") return OnMouseDown;
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

bool isValidEvent (string line) {
    if (line.substr (0, line.find (" ")) == "event") {
        return intToBool (
            getEventTypeID (
                removeSpaces (line.substr (line.find (" "), line.length ())) 
            ) + 1
        );
    }
    return false;
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

string updateArgsBaseCode [][2] = {
    {"top", "set_top ({uv})"},
    {"left", "set_left ({uv})"},
    {"width", "set_width ({uv})"},
    {"height", "set_height ({uv})"},
    {"origin_x", "set_origin_x ({uv})"},
    {"origin_y", "set_origin_y ({uv})"},
    {"stroke_width", "set_stroke_width ({uv})"},
    {"stroke_color", "set_stroke_color ({rv})"},
    {"fill_color", "set_fill_color ({rv})"},
    {"vertix_#_x", "set_vertix (#, {uv})"},
    {"vertix_#_y"}
};

string argsBaseCode [5] = {

    "(Coordinate (\"{left}\", \"{top}\"), Dimensions (\"{width}\", \"{height}\"), {origin_x: 0.5}, {origin_y: 0.5}, StrokeData ({stroke_width: 1}, Color32 (\"{stroke_color: #000000FF}\")), FillData (Color32 (\"{fill_color: #FFFFFF00}\")))",
    "({Coordinate (\"[vertix_#_x]\", \"[vertix_#_y]\") ...}, StrokeData ({stroke_width: 1}, Color32 (\"{stroke_color: #000000FF}\")), FillData (Color32 (\"{fill_color: #FFFFFF00}\")))",
    "(Coordinate (\"{start_x}\", \"{start_y}\"), Coordinate (\"{end_x}\", \"{end_y}\"), StrokeData ({stroke_width: 1}, Color32 (\"{stroke_color: #000000FF}\")))",
    "({Coordinate (\"[point_#_x]\", \"[point_#_y]\") ...}, StrokeData ({stroke_width: 1}, Color32 (\"{stroke_color: #000000FF}\")))",
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

    createdElements.push_back (elementName);

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
                return outputStatement;
            } else {
                cout << "ERROR: The element '" << elementType << "' is not supported." << endl;
                cout << "Are you sure you're entering the name correctly ?" << endl;
            }
            break;
        }
        case UPDATE: {
            bool found = false;
            smatch match;
            regex_search (line, match, regex ("`(\\w+)`"));
            for (int i = 0; i < createdElements.size (); i++) {
                createdElements[i] == match[i];
                found = true;
                break;
            }
            if (!found) {
                cout << "ERROR: Target element " << match[1] << " at line " << lineNumber << " is not defined. ";
                break;
            } else {

            }
            break;
        }
        default: {
            cout << "INVALID COMMAND TYPE" << endl;
        }
    }
    return "";
}

string processEvent (string lineBlock) {
    vector<string> lines = split (lineBlock, '\n');
    for (int i = 1; i < lines.size (); i++) {
        if (isValidCommand (lines[i])) {
            //processCommand (lines[i]);
        }
    }
    return "";
}

int main () {

    string line;
    string lineBlock = "";

    int lineNumber;
    bool readingEvent = false;

    ifstream Script ("./script.ugcl");

    while (getline (Script, line)) {
        if (readingEvent && line[0] == '\t') {
            lineBlock += "\n" + line;
        } else {
            if (readingEvent) {
                readingEvent = false;
                processEvent (lineBlock);
            }
            if (isValidCommand (line)) {
                cout << processCommand (line, lineNumber) << endl;
            } else if (isValidEvent (line)) {
                lineBlock = line;
                readingEvent = true;
            }
        }
        lineNumber++;
    }

    Script.close ();

    return 1;
}