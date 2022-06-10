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

string strip_left (string input) {
    string output = "";
    bool searching = true;
    for (int i = 0; i < input.length (); i++) {
        if (searching) {
            if (input[i] != ' ') {
                output += input[i];
                searching = false;
            }
        } else {
            output += input[i]; 
        }
    }
    return output;
}

string strip_right (string input) {
    string output = "";
    bool searching = true;
    for (int i = input.length () - 1; i >= 0; i--) {
        if (searching) {
            if (input[i] != ' ') {
                output = input[i] + output;
                searching = false;
            }
        } else {
            output = input[i] + output; 
        }
    }
    return output;
}

string strip (string input) {

    string output; 

    output = strip_left (input);
    output = strip_right (output);

    return output;
}

enum {
    CREATE,
    UPDATE,
    VAR,
    SET
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

enum {
    creationCode,
    onDrawCode,
    onShapeClick,
    onKeyPress,
    onKeyRelease,
    onMouseMove,
    onMouseClick,
    onMouseUp,
    onMouseDown
};

string replacementNames [9] = {
    "\\{creationCode\\}",
    "\\{onDrawCode\\}",
    "\\{OnShapeClick\\}",
    "\\{OnKeyPress\\}",
    "\\{OnKeyRelease\\}",
    "\\{OnMouseMove\\}",
    "\\{OnMouseClick\\}",
    "\\{OnMouseUp\\}",
    "\\{OnMouseDown\\}"
};

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

// Set all elements of the array to an empty strings.
string eventsCode [9] = { "" };

int getCommandID (string commandName) {
    if (commandName == "CREATE") return CREATE;
    if (commandName == "UPDATE") return UPDATE;
    if (commandName == "VAR") return VAR;
    if (commandName == "SET") return SET;
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
            removeWhitespace (commandName.substr (0, commandName.find (" ")))
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

string updateArgsBaseCode [][3] = {

    {"top", "set_position_y ({uv})", "string"},
    {"left", "set_position_x ({uv})", "string"},

    {"width", "set_width ({uv})", "string"},
    {"height", "set_height ({uv})", "string"},

    {"origin_x", "set_origin_x ({rv})", "float"},
    {"origin_y", "set_origin_y ({rv})", "float"},

    {"vertix_#_x", "set_vertix_x (#, {uv})", "string"},
    {"vertix_#_y", "set_vertix_y (#, {uv})", "string"},

    {"stroke_width", "getStrokeData ()->setWidth ({rv})", "int"},
    {"stroke_color", "getStrokeData ()->setColor (Color32 ({rv}))", "string"},

    {"fill_color", "getFillData ()->setColor (Color32 ({rv}))", "string"},

    {"start_x", "set_start_x ({uv})", "string"},
    {"start_y", "set_start_y ({uv})", "string"},

    {"end_x", "set_end_x ({uv})", "string"},
    {"end_y", "set_end_y ({uv})", "string"},

    {"point_#_x", "set_point_x (#, {uv})", "string"},
    {"point_#_y", "set_point_y (#, {uv})", "string"},

    {"radius", "set_radius ({rv})", "float"},

    {"angle_start", "set_angle_start ({rv})", "float"},
    {"angle_end", "set_angle_end ({rv})", "float"}

};

string getArgumentCode (string argumentName) {
    int arrayLength = sizeof (updateArgsBaseCode) / 
                        sizeof (updateArgsBaseCode[0]);
    for (int i = 0; i < arrayLength; i++) {
        if (regex_match (argumentName, regex (string (regex_replace (updateArgsBaseCode[i][0], regex ("#"), "\\d+"))))) {
            if (updateArgsBaseCode[i][0].find ("#") != string::npos) {
                smatch match;
                regex_search (argumentName, match, regex ("(\\d+)"));
                return string (regex_replace (updateArgsBaseCode[i][1], regex ("#"), string (match[1])));
            } else {
                return updateArgsBaseCode[i][1];
            }
        }
    }
    return "";
}

string getPropertyDataType (string property) {
    int arrayLength = sizeof (updateArgsBaseCode) / 
                        sizeof (updateArgsBaseCode[0]);
    for (int i = 0; i < arrayLength; i++) {
        if (regex_match (property, regex (string (regex_replace (updateArgsBaseCode[i][0], regex ("#"), "\\d+"))))) {
            return updateArgsBaseCode[i][2];
        }
    }
    return "";
}

bool isValidProperty (string property) {
    int arrayLength = sizeof (updateArgsBaseCode) / 
                        sizeof (updateArgsBaseCode[0]);
    for (int i = 0; i < arrayLength; i++) {
        if (regex_match (property, regex (string (regex_replace (updateArgsBaseCode[i][0], regex ("#"), "\\d+"))))) {
            return true;
        }
    }
    return false;
}

bool isValidUnit (string unit) {
    if 
    (
        unit == "px" ||
        unit == "%"
    )
    return true; 
    return false;
}

bool requiresUnit (string argumentName) {
    int arrayLength = sizeof (updateArgsBaseCode) / 
                        sizeof (updateArgsBaseCode[0]);
    for (int i = 0; i < arrayLength; i++) {
        if (regex_match (argumentName, regex (string (regex_replace (updateArgsBaseCode[i][0], regex ("#"), "\\d+"))))) {
            if (updateArgsBaseCode[i][1].find ("{uv}") != string::npos) {
                return true;
            }
        }
    }
    return false;
}

string generateUpdateCode (string inputString, string elementName, int lineNumber) {
    string outputCode = "";
    vector<string> userArguments = split (inputString, ',');
    for (int i = 0; i < userArguments.size (); i++) {
        string userArgument = strip (userArguments[i]);
        vector<string> userArgParts = split (userArgument, '=');
        string argumentName = strip (userArgParts[0]);
        string argumentValue = strip (userArgParts[1]);

        if (!isValidProperty (argumentName)) {
            cout << "ERROR: Invalid argument '" << argumentName << "' provided at line " << lineNumber << endl;
            return "";
        }

        smatch match;
        regex_search (argumentValue, match, regex ("\\{(.+)\\}"));

        if (match[1] != "") {
            if (argumentValue.find (":") == string::npos) {
                if (requiresUnit (argumentName)) {
                    cout << "ERROR: Unit not specified for argument '" << argumentName << "' at line " << lineNumber << endl;
                    return "";
                } else {
                    if (getPropertyDataType (argumentName) == "string") {
                        outputCode += (outputCode == "" ? "" : "\n") + elementName + "." + regex_replace (getArgumentCode (argumentName), regex ("\\{.+\\}"), "to_string (" + string (regex_replace (argumentValue, regex ("[\\{\\}]+"), "")) + ")") + ";";
                    } else {
                        outputCode += (outputCode == "" ? "" : "\n") + elementName + "." + regex_replace (getArgumentCode (argumentName), regex ("\\{.+\\}"), regex_replace (argumentValue, regex ("[\\{\\}]+"), "")) + ";";
                    }
                }
            } else {
                if (!isValidUnit (strip (split (argumentValue, ':') [1]))) {
                    cout << "ERROR: The specified unit '" << strip (split (argumentValue, ':') [1]) << "' for argument " << argumentName << " is not valid. " << endl;
                    return "";
                } else {
                    string value = regex_replace (strip (split (argumentValue, ':') [0]), regex ("[\\{\\}]+"), "");
                    string unit = split (argumentValue, ':') [1];
                    outputCode += (outputCode == "" ? "" : "\n") + elementName + "." + regex_replace (getArgumentCode (argumentName), regex ("\\{.+\\}"), "to_string (" + value + ") + " + "\"" + strip (unit) + "\"") + ";";
                }
            }
        } else {
            if (getPropertyDataType (argumentName) == "string") {
                outputCode += (outputCode == "" ? "" : "\n") + elementName + "." + regex_replace (getArgumentCode (argumentName), regex ("\\{.+\\}"), "\"" + argumentValue + "\"") + ";";
            } else {
                outputCode += (outputCode == "" ? "" : "\n") + elementName + "." + regex_replace (getArgumentCode (argumentName), regex ("\\{.+\\}"), argumentValue) + ";";
            }
        }

    }
    return outputCode;
}

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
                return "";  
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

string processCommand (string command, int lineNumber, bool inEvent) {
    string outputStatement = "";
    switch (getCommandID (removeWhitespace (command.substr (0, command.find (" "))))) {
        case CREATE: {
            string elementType = getElementType (command);
            if (isValidElement (elementType)) {
                outputStatement = generateCreateCode (elementType, getElementName (command), command, lineNumber);
                eventsCode[creationCode] += "\n" + outputStatement;
                eventsCode[onDrawCode] += "\n" + getElementName (command) + ".draw (cr);";
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
            regex_search (command, match, regex ("`(\\w+)`"));
            for (int i = 0; i < createdElements.size (); i++) { 
                if (createdElements[i] == match[1]) {
                    found = true;
                    break;
                }
            }
            if (!found && match[1]) {
                cout << "ERROR: Target element " << match[1] << " at line " << lineNumber << " is not defined. ";
                break;
            } else {
                string elementName = match[1];
                regex_search (command, match, regex ("\\((.*)\\)"));
                string generatedCode = generateUpdateCode (match[1], elementName, lineNumber);
                return generatedCode;
            }
            break;
        }
        case VAR: {
            string generatedCode = "int " + command.substr (command.find (" "), command.length ()) + ";";
            eventsCode[creationCode] += "\n" + generatedCode;
            return generatedCode;
        }
        case SET: {
            string generatedCode = command.substr (command.find (" "), command.length ()) + ";";
            return generatedCode;
        }
        default: {
            cout << "INVALID COMMAND TYPE: " << command << endl;
        }
    }
    return "";
}

bool isConditional (string line) {

    if (removeWhitespace (line.substr (0, line.find (" "))) == "IF") {
        return true;
    }
    return false;
}

string processEvent (string lineBlock, int lineNumber, int eventLines) {

    string conditionalCode = "";
    bool readingConditional = false;
    string eventCode = "";

    int startingLine = lineNumber - eventLines;
    vector<string> lines = split (lineBlock, '\n');
    for (int i = 1; i < lines.size (); i++) {
        if (readingConditional) {
            if (lines[i].substr (0, 2) != "\t\t") {
                readingConditional = false;
                eventCode += conditionalCode + " }";
                conditionalCode = "";
                i--;
                continue;
            } else {
                conditionalCode += " " + processCommand (lines[i], startingLine + i, true);
            }
        } else {
            if (isConditional (lines[i])) {
                readingConditional = true;
                conditionalCode = "if (" + strip (lines[i].substr (lines[i].find (" "), lines[i].length ())) + ") { ";
            } else if (isValidCommand (lines[i])) {
                eventCode += (eventCode == "" ? "" : "\n" ) + processCommand (lines[i], startingLine + i, true);
            }
        }
    }

    if (conditionalCode != "") {
        readingConditional = false;
        eventCode += conditionalCode + " }";
        conditionalCode = "";
    }

    string eventName = removeSpaces (lines[0].substr (lines[0].find (" "), lines[0].length ()));
    eventsCode[getEventTypeID (eventName) + 2] += "\n" + eventCode;

    return "";
}

string getBaseCode () {
    string output = "", line;
    ifstream baseScript ("./base.cpp");
    while (getline (baseScript, line)) {
        output += "\n" + line;
    }
    return output;
}

int main () {

    string line;
    string lineBlock = "";

    int lineNumber;

    int eventLines;
    bool readingEvent = false;

    ifstream Script ("./script.ugcl");

    while (getline (Script, line)) {
        if (readingEvent && line[0] == '\t') {
            lineBlock += "\n" + line;
            eventLines++;
        } else {
            if (readingEvent) {
                processEvent (lineBlock, lineNumber, eventLines);

                // Emptying the data
                readingEvent = false;
                eventLines = 0;
                lineBlock = "";
            } else {
                if (isValidCommand (line)) {
                    processCommand (line, lineNumber, false);
                } else if (isValidEvent (line)) {
                    lineBlock = line;
                    readingEvent = true;
                    eventLines = 1;
                }
            }
        }
        lineNumber++;
    }

    // In Case the lineBlock is left unprocessed
    if (lineBlock != "") {
        processEvent (lineBlock, lineNumber, eventLines);
    }

    Script.close ();

    cout << ">> Compilation Completed Successfully. " << endl;

    string baseCode = getBaseCode ();

    for (int i = 0; i < 9; i++) {
        if (eventsCode[i] != "" && i > 1) {
            if (i == 2 || i == 3 || i == 4 || i == 6) {
                eventsCode[i] += "\ncanvas.queue_draw ();";
            } else {
                eventsCode[i] += "\nqueue_draw ();";
            }
        }
        baseCode = regex_replace (baseCode, regex ("// " + replacementNames[i]), eventsCode[i]);
    }

    ofstream outputFile ("outputCode.cpp");

    outputFile << baseCode;

    outputFile.close ();    

    cout << ">> Generated code has been copied to the output file." << endl;

    return 1;
}