#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <string>
#include<sstream>
#include <regex>
using namespace std;


#define BUFFER_SIZE 25
char buffer1[BUFFER_SIZE] = { 0 };
char buffer2[BUFFER_SIZE] = { 0 };
int lb = 0, lf = 0;
int activeBuffer = 1;
bool inMultiLineComment = false;
bool bufferChanged = false;
bool ended = false;

const int rows = 58;
const int cols = 25;
bool Accept[rows];
int T[rows][cols];
int startState = 55;
int A[rows][cols];

vector < string> toks;
int pointer = 0;
string lookaheadToken;
string err;


void populateAccept() {
    for (int i = 0;i < rows;i++) {
        Accept[i] = 1;
    }
    Accept[55] = 0; Accept[0] = 0; Accept[2] = 0;Accept[3] = 0;Accept[5] = 0;
    Accept[9] = 0;Accept[10] = 0;Accept[11] = 0;Accept[12] = 0;Accept[13] = 0;
    Accept[14] = 0;Accept[16] = 0;Accept[17] = 0;Accept[20] = 0;Accept[30] = 0;
    Accept[32] = 0;Accept[35] = 0;Accept[37] = 0;Accept[39] = 0;Accept[42] = 0;
    Accept[46] = 0;Accept[51] = 0; Accept[56] = 0;
}

void populateT() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < 25; j++) {
            T[i][j] = -1;
        }
    }
    T[55][0] = 2;
    T[55][1] = 9;
    T[55][2] = 0;
    T[55][3] = 5;
    T[55][4] = 20;
    T[55][5] = 37;
    T[55][6] = 2;
    T[55][7] = 23;
    T[55][8] = 22;
    T[55][9] = 24;
    T[55][10] = 25;
    T[55][11] = 26;
    T[55][12] = 27;
    T[55][13] = 28;
    T[55][14] = 29;
    T[55][15] = 30;
    T[55][16] = 33;
    T[55][17] = 34;
    T[55][18] = 35;
    T[55][19] = 39;
    T[55][20] = 42;
    T[55][21] = 46;
    T[55][22] = 51;
    T[55][23] = 54;
    T[55][24] = 54;

    for (int j = 0;j < cols;j++) {
        T[0][j] = 1;
        T[2][j] = 4;
        T[3][j] = 1;
        T[5][j] = 6;
        T[9][j] = 19;
        T[12][j] = 15;
        T[14][j] = 15;
        T[17][j] = 18;
        T[20][j] = 21;
        T[30][j] = 53;
        T[32][j] = 53;
        T[35][j] = 53;
        T[37][j] = 53;
        T[39][j] = 53;
        T[42][j] = 45;
        T[46][j] = 50;
        T[51][j] = 51;
        T[56][j] = 53;

    }
    T[56][4] = 57; T[56][20] = 57;
    T[3][0] = 3; T[3][1] = 3; T[3][2] = 3; T[3][6] = 3;
    T[0][0] = 0;T[0][1] = 0;T[0][2] = 0; T[0][6] = 0;

    T[2][0] = 2;T[2][1] = 2;T[2][2] = 3;T[2][6] = 2; T[2][4] = 56; T[2][21] = 56;

    T[5][1] = 9;T[5][3] = 8;T[5][5] = 7;

    T[9][1] = 9;T[9][6] = 10;T[9][23] = 16;

    T[10][1] = 11;  T[10][3] = 11; T[10][4] = 11;

    T[11][1] = 12;

    T[12][1] = 12; T[12][6] = 10; T[12][23] = 13;

    T[13][1] = 14;

    T[14][1] = 14; T[14][6] = 10;

    T[16][1] = 17;

    T[17][1] = 17; T[17][6] = 10;

    T[20][1] = 9;

    T[30][5] = 31;

    T[32][16] = 33;

    T[35][18] = 36;

    T[37][5] = 38;

    T[39][5] = 41; T[39][19] = 40;

    T[42][5] = 44; T[42][20] = 43;

    T[46][5] = 47; T[46][20] = 48; T[46][21] = 49;

    T[51][22] = 52;

}

void populateA() {
    for (int i = 0;i < rows;i++) {
        for (int j = 0;j < cols;j++) {
            if (T[i][j] == -1) {
                A[i][j] = -1;
            }
            else {
                A[i][j] = 1;
                if (T[i][j] == 1 || T[i][j] == 4 || T[i][j] == 6 || T[i][j] == 15 || T[i][j] == 18 || T[i][j] == 19 || T[i][j] == 21 || T[i][j] == 45 || T[i][j] == 50 || T[i][j] == 53)
                {
                    A[i][j] = 0;

                }
            }
        }
    }
}

void loadBuffer(ifstream& file, char buffer[], bool& inMultiLineComment) { //MEOW CATEGORY FUNCTIONALITY HANDLED HERE
    int idx = 0;
    char c;
    bool inSingleLineComment = false;
    bool lastWasSpace = false;


    while (idx < BUFFER_SIZE - 1 && file.get(c)) {
        if (inSingleLineComment) {  // Skip until newline
            if (c == '\n') {
                inSingleLineComment = false;
            }
            else
                continue;
        }

        if (inMultiLineComment) {  // Skip until "*/"
            if (c == '*' && file.peek() == '/') {
                file.get(c);  // Consume '/'
                inMultiLineComment = false;
                c = ' ';
            }
            else
                continue;
        }

        if (c == '/' && file.peek() == '/') {
            inSingleLineComment = true;  // Enter single-line comment
            continue;
        }

        if (c == '/' && file.peek() == '*') {
            inMultiLineComment = true;  // Enter multi-line comment
            file.get(c); // Consume '*'
            continue;
        }

        // Normalize multiple spaces/newlines into a single space
        if (isspace(c)) {
            if (!lastWasSpace) {
                buffer[idx++] = ' ';
                lastWasSpace = true;
            }
        }
        else {
            buffer[idx++] = c;
            lastWasSpace = false;
        }
    }

    buffer[idx] = '\0';  // Null-terminate the buffer
}

char getcurrChar() {

    if (activeBuffer == 1) {
        return buffer1[lf];
    }
    else
        return buffer2[lf];
}

char getChar(ifstream& file) {                  //concat with getchar, since all these actions will be performed when we fetch a new char

    lf++;

    char* currentBuffer = (activeBuffer == 1) ? buffer1 : buffer2;
    if (file.eof() && currentBuffer[lf] == '\0') {
        ended = true;
        return ' ';
    }



    if (!file.eof()) {



        if (currentBuffer[lf] == '\0') {
            bufferChanged = true;
            activeBuffer = 3 - activeBuffer;
            lf = 0;


            if (activeBuffer == 1)
                loadBuffer(file, buffer1, inMultiLineComment);
            else
                loadBuffer(file, buffer2, inMultiLineComment);


        }


    }
    currentBuffer = (activeBuffer == 1) ? buffer1 : buffer2;
    char c = currentBuffer[lf]; // Simulate token scanning


    return c;

}

int getCharMap(char c) {
    if ((c >= 'A' && c < 'E') || (c >= 'F' && c <= 'Z') || (c >= 'a' && c <= 'z'))
        return 0;

    // Digits -> return 1
    if (c >= '0' && c <= '9')
        return 1;

    // Specific character mappings
    switch (c) {
    case '_': return 2;
    case '+': return 3;
    case '-': return 4;
    case '=': return 5;
    case 'E': return 6;
    case '(': return 7;
    case ')': return 8;
    case '[': return 9;
    case ']': return 10;
    case '{': return 11;
    case '}': return 12;
    case '/': return 13;
    case '*': return 14;
    case '!': return 15;
    case '|': return 16;
    case '%': return 17;
    case '&': return 18;
    case ':': return 19;
    case '>': return 20;
    case '<': return 21;
    case '"': return 22;
    case '.': return 23;
    }

    // All other printable ASCII characters (including whitespace) -> return 24
    if (c >= 32 && c <= 126)
        return 24;

    // Default case (should never happen for valid input)
    return -1;
}

bool errorState(int state) {
    if (state == 53 || state == 54)
        return true;
}

string tokenise(int& finalState, ifstream& file) {
    int state = startState;
    char a = getcurrChar();


    int ch = getCharMap(a);
    while (!Accept[state]) {
        int newState = T[state][ch];
        if (A[state][ch] == 1) {
            a = getChar(file); /////////////
            ch = getCharMap(a);
        }
        state = newState;


    }

    if (Accept[state]) {
        finalState = state;
        char* prevBuffer;
        char* currentBuffer = (activeBuffer == 1) ? buffer1 : buffer2;
        if (activeBuffer == 1)
        {

            prevBuffer = buffer2;
        }
        else {
            prevBuffer = buffer1;
        }

        if (bufferChanged) {

            string token(prevBuffer + lb, prevBuffer + 24);
            token.append(currentBuffer, lf);
            bufferChanged = false;
            lb = lf;
            return token;

        }
        else {
            string token(currentBuffer + lb, currentBuffer + lf);
            lb = lf;
            return token;
        }
    }
}

int classify(ifstream& file) {
    ofstream sym("symbTable.txt");
    if (!sym) {
        cout << "symbTable.txt could not be opened" << endl;
        return -1;
    }
    ofstream lit("litTable.txt");
    if (!lit) {
        cout << "litTable.txt could not be opened" << endl;
        return -1;
    }
    ofstream error("error.txt");
    if (!error) {
        cout << "error.txt could not be opened" << endl;
        return -1;
    }
    ofstream tokens("tokens.txt");
    if (!tokens) {
        cout << "tokens.txt could not be opened" << endl;
        return -1;
    }
    ifstream key("res.txt");
    if (!key) {
        cout << "res.txt could not be opened" << endl;
        return -1;
    }

    vector<string> identifiers;
    vector<string> keywords;

    string line;
    while (getline(key, line)) {  //reading keywords
        keywords.push_back(line);
    }

    int idIndex = 1, litIndex = 1;


    int finalstate = 0;
    char* currentBuffer = (activeBuffer == 1) ? buffer1 : buffer2;

    while ((!file.eof() || currentBuffer[lf] != '\0') && ended == false) {
        string token = tokenise(finalstate, file);
        if (finalstate == 1) { //identifier
            bool found = false;
            for (int i = 0;i < identifiers.size();i++) {
                if (identifiers[i] == token) {
                    found = true;
                    tokens << "<id," << i + 1 << "> ";
                    break;
                }
            }
            if (found == false) {
                sym << idIndex << " " << token << "\n";
                identifiers.push_back(token);
                tokens << "<id," << idIndex++ << "> ";

            }
        }
        else if (finalstate == 4 || finalstate == 57) {  //keyword
            bool found = false;
            for (int i = 0;i < keywords.size();i++) {
                if (token == keywords[i]) {
                    tokens << "<res," << i + 1 << "> ";
                    found = true;
                    break;
                }
            }
            if (found == false) {
                error << token << " ";
            }
        }
        else if (finalstate == 6 || finalstate == 7 || finalstate == 8 || finalstate == 21 || finalstate == 28 || finalstate == 29 || finalstate == 31 || finalstate == 33 || finalstate == 34 || finalstate == 36 || finalstate == 38 || finalstate == 41 || finalstate == 43 || finalstate == 44 || finalstate == 45 || finalstate >= 47 && finalstate <= 50) {
            tokens << "<op," << token << "> ";
        }
        else if (finalstate == 15 || finalstate == 18 || finalstate == 19) {
            tokens << "<num," << token << "> ";
        }
        else if ((finalstate >= 22 && finalstate <= 27) || finalstate == 40) {
            tokens << "<punc," << token << "> ";

        }
        else if (finalstate == 52) { //string lit
            string beechwala = token.substr(1, token.size() - 2);
            tokens << "<op,\">" << "<lit," << litIndex << ">" << "<op,\"> ";
            lit << litIndex++ << " " << beechwala << '\n';
        }
        else if (finalstate == 53 || finalstate == 54) {
            if (token != "\n" && token != " " && token != "\t") {
                error << token << " ";
            }
        }

    }
    tokens.close();
    lit.close();
    key.close();
    sym.close();
    error.close();

    return 0;
}

////CODE HERE SISTERSSSSSS***************************************************************************************************************************************************************************
bool Function(int level);
bool ArgList(int level);
bool Arg(int level);
bool ArgList_(int level);
bool Declaration(int level);
bool Type(int level);
bool IdentList(int level);
bool IdentList_(int level);
bool Stmt(int level);
bool ForStmt(int level);
bool OptExpr(int level);
bool WhileStmt(int level);
bool IfStmt(int level);
bool ElsePart(int level);
bool CompStmt(int level);
bool StmtList(int level);
bool StmtList_(int level);
bool Expr(int level);
bool Expr_(int level);
bool Rvalue(int level);
bool Rvalue_(int level);
bool Compare(int level);
bool Mag(int level);
bool Mag_(int level);
bool Term(int level);
bool Term_(int level);
bool Factor(int level);

bool isLit(const string& token) {
    return token.substr(1, 4) == "lit,";

}

bool isNum(const string& token) {
    return token.substr(1, 4) == "num,";

}

int readfiles(vector<string>& translated) {
    ifstream key("res.txt");
    if (!key) {
        cout << "res.txt could not be opened" << endl;
        return -1;
    }
    ifstream tokens("tokens.txt");
    if (!tokens) {
        cout << "tokens.txt could not be opened" << endl;
        return -1;
    }

    vector<string> keywords;
    string line;

    // Read keywords into vector
    while (getline(key, line)) {
        keywords.push_back(line);
    }

    regex res_regex(R"(<res,(\d+)>)");

    // Process each line from tokens.txt
    while (getline(tokens, line)) {
        stringstream ss(line);
        string token;

        while (ss >> token) {
            smatch match;
            if (regex_match(token, match, res_regex)) {
                int index = stoi(match[1]);
                if (index > 0 && index <= keywords.size())
                    translated.push_back("<res," + keywords[index - 1] + ">");
                else
                    translated.push_back("<res,UNKNOWN>");
            }
            else {
                translated.push_back(token);
            }
        }
    }

    //// Print each translated token
    //for (size_t i = 0; i < translated.size(); ++i) {
    //    cout << i << " " << translated[i] << endl;
    //}

    return 0;
}

int Advance() {
    if (pointer + 1 == toks.size()) {
        return -1;
    }
    return ++pointer;
}

bool printNode(const string& nodeName, int level) {
    for (int i = 0; i < level; i++) cout << "    "; // indent with two spaces per level
    cout << "|-> " << nodeName << endl;
    return true;
}

string getNextToken(int l) {
    string tokk = toks[pointer];
    Advance();
    lookaheadToken = toks[pointer];
    printNode(tokk, l);
    return tokk;
}

bool isIdentifier(const string& token) {
    // printNode("<id>", l);
    return token.substr(1, 3) == "id,";
}

void printErr(string t) {
    cout << "error in tokens in " << t << " ";
    //for (int i = pointer - 1;i < pointer + 3 && i < toks.size();i++) {
    //    //cout<< toks[i]<<" ";
    //}
    cout << endl;
    err = t;
}

bool StmtList(int level) {
    printNode("StmtList", level);
    if (StmtList_(level + 1)) return true;
    printErr("StmtList");
    return false;
}

bool StmtList_(int level) {
    printNode("StmtList'", level);
    if (
        lookaheadToken == "<res,Agar>" ||   // if
        lookaheadToken == "<res,while>" ||  // while
        lookaheadToken == "<res,for>" ||  // for
        lookaheadToken == "<punc,{>" ||  // compound stmt
        isIdentifier(lookaheadToken) || // identifier (for assignments)
        /*isLit(lookaheadToken)||*/
        lookaheadToken == "<res,Adadi>" ||
        lookaheadToken == "<res,Ashriya>" ||
        lookaheadToken == "<res,Harf>" ||
        lookaheadToken == "<res,Matn>" ||
        lookaheadToken == "<res,Mantiqi>" ||   // declaration types
        lookaheadToken == "<punc,::>" ||
        lookaheadToken == "<punc,(>" ||
        isNum(lookaheadToken)
        )
    {

        if (Stmt(level + 1) && StmtList_(level + 1)) return true;
        printErr("StmtList_");
        return false;
    }
    // Empty production
    printNode("(null)", level + 1);
    return true;
}

bool Type(int l) {
    printNode("Type", l);
    string tokk = getNextToken(l + 1);
    if (tokk == "<res,Adadi>" || tokk == "<res,Ashriya>" || tokk == "<res,Harf>" || tokk == "<res,Matn>" || tokk == "<res,Mantiqi>") {

        return true;
    }
    else {
        printErr("Type");
        return false;
    }
}

bool Compare(int l) {
    printNode("Compare", l);

    string tokk = getNextToken(l + 1);
    if (tokk == "<op,==>" || tokk == "<op,<>" || tokk == "<op,>>" || tokk == "<op,<=>" || tokk == "<op,>=>" || tokk == "<op,!=>" || tokk == "<op,<>>") {

        return true;
    }
    else {
        printErr("Compare");
        return false;
    }
}

bool Function(int l) {
    printNode("Function", l);
    if (Type(l + 1) && isIdentifier(getNextToken(l + 1)) && getNextToken(l + 1) == "<punc,(>" && ArgList(l + 1) &&
        getNextToken(l + 1) == "<punc,)>" && CompStmt(l + 1))
    {
        return true;

    }
    else {
        printErr("Function");
        return false;
    }
}

bool ArgList(int l) {
    printNode("ArgList", l);
    if (Arg(l + 1) && ArgList_(l + 1)) {
        return true;
    }
    printErr("ArgList");
    return false;
}

bool ArgList_(int l) {
    printNode("ArgList'", l);
    if (lookaheadToken == "<res,Adadi>" ||
        lookaheadToken == "<res,Ashriya>" ||
        lookaheadToken == "<res,Harf>" ||
        lookaheadToken == "<res,Matn>" ||
        lookaheadToken == "<res,Mantiqi>")
    {
        if (Arg(l + 1) && ArgList_(l + 1)) {
            return true;
        }
        printErr("ArgList'");
        return false;
    }
    printNode("(null)", l + 1);
    return true;
}

bool Arg(int l) {
    printNode("Arg", l);
    if (Type(l + 1) && isIdentifier(getNextToken(l + 1))) { return true; }

    printErr("Arg");
    return false;
}

bool Declaration(int l) {
    printNode("Declaration", l);
    if (Type(l + 1) && IdentList(l + 1) && getNextToken(l + 1) == "<punc,::>") {
        return true;
    }
    printErr("Declaration");
    return false;
}

bool IdentList(int l) {
    printNode("IdentList", l);
    if (isIdentifier(getNextToken(l + 1)) && IdentList_(l + 1)) return true;
    printErr("IdentList");
    return false;

}

bool IdentList_(int l) {
    printNode("IdentList'", l);
    if (lookaheadToken == "<punc,,>") {

        if (getNextToken(l + 1) == "<punc,,>" && IdentList(l + 1)) {
            return true;
        }
        printErr("IdentList'");
        return false;
    }
    printNode("(null)", l + 1);
    return true;
}

bool Stmt(int l) {
    printNode("Stmt", l);
    if (lookaheadToken == "<res,for>") {
        if (ForStmt(l + 1)) return true;
        printErr("Stmt");
        return false;
    }
    else if (lookaheadToken == "<res,while>") {
        if (WhileStmt(l + 1)) return true;
        printErr("Stmt");
        return false;
    }
    else if (
        isIdentifier(lookaheadToken) || // identifier (for assignments)
        lookaheadToken == "<punc,(>" ||
        isNum(lookaheadToken)
        ) {
        if (Expr(l + 1) && getNextToken(l + 1) == "<punc,::>") return true;
        printErr("Stmt");
        return false;
    }
    else if (lookaheadToken == "<res,Agar>") {
        if (IfStmt(l + 1)) return true;
        printErr("Stmt");
        return false;
    }
    else if (lookaheadToken == "<punc,{>") {
        if (CompStmt(l + 1)) return true;
        printErr("Stmt");
        return false;
    }
    else if (lookaheadToken == "<res,Adadi>" ||
        lookaheadToken == "<res,Ashriya>" ||
        lookaheadToken == "<res,Harf>" ||
        lookaheadToken == "<res,Matn>" ||
        lookaheadToken == "<res,Mantiqi>") {
        if (Declaration(l + 1)) return true;
        printErr("Stmt");
        return false;
    }
    else if (lookaheadToken == "<punc,::>") {
        getNextToken(l + 1);
        return true;
    }
    printErr("Stmt");
    return false;

}

bool ForStmt(int l) {
    printNode("ForStmt", l);
    if (getNextToken(l + 1) == "<res,for>"
        && getNextToken(l + 1) == "<punc,(>"
        && Expr(l + 1) && getNextToken(l + 1) == "<punc,::>"
        && OptExpr(l + 1) && getNextToken(l + 1) == "<punc,::>"
        && OptExpr(l + 1) && getNextToken(l + 1) == "<punc,)>" &&
        Stmt(l + 1)
        )
    {
        return true;
    }
    printErr("ForStmt");
    return false;
}

bool OptExpr(int level) {
    printNode("OptExpr", level);
    // If the next token can start an Expr, parse it; otherwise epsilon
    if (
        lookaheadToken == "<punc,(>" ||
        isNum(lookaheadToken) ||
        isIdentifier(lookaheadToken)
        // You could add more if your grammar allows strings, booleans, etc.
        )
    {
        if (Expr(level + 1)) {
            return true;
        }
        printErr("OptExpr");
        return false;
    }

    // ? - production
    printNode("(null)", level + 1);
    return true;
}

bool WhileStmt(int level) {
    printNode("WhileStmt", level);
    // while ( Expr ) Stmt
    if (getNextToken(level + 1) == "<res,while>" &&
        getNextToken(level + 1) == "<punc,(>" &&
        Expr(level + 1) &&
        getNextToken(level + 1) == "<punc,)>" &&
        Stmt(level + 1))
    {
        return true;
    }
    printErr("WhileStmt");
    return false;
}

bool IfStmt(int level) {
    printNode("IfStmt", level);
    // Agar ( Expr ) Stmt ElsePart
    if (getNextToken(level + 1) == "<res,Agar>" &&
        getNextToken(level + 1) == "<punc,(>" &&
        Expr(level + 1) &&
        getNextToken(level + 1) == "<punc,)>" &&
        Stmt(level + 1) &&
        ElsePart(level + 1))
    {
        return true;
    }
    printErr("IfStmt");
    return false;
}

bool ElsePart(int level) {
    printNode("ElsePart", level);
    // Wagarna Stmt | ?
    if (lookaheadToken == "<res,Wagarna>") {
        if (getNextToken(level + 1) == "<res,Wagarna>" &&
            Stmt(level + 1))
        {
            return true;
        }
        printErr("ElsePart");
        return false;
    }

    // ? - production
    printNode("(null)", level + 1);
    return true;
}

bool CompStmt(int level) {
    printNode("CompStmt", level);
    // { StmtList }
    if (getNextToken(level + 1) == "<punc,{>" &&
        StmtList(level + 1) &&
        getNextToken(level + 1) == "<punc,}>")
    {
        return true;
    }
    printErr("CompStmt");
    return false;
}

bool Expr(int level) {
    printNode("Expr", level);
    // According to your grammar, Expr has 3 possible forms:
    //  1) identifier Expr'
    //  2) (Expr) Term' Mag' Rvalue'
    //  3) number Term' Mag' Rvalue'

    // 1) identifier Expr'
    if (isIdentifier(lookaheadToken)) {
        getNextToken(level + 1);  // consume the identifier
        if (Expr_(level + 1)) {
            return true;
        }
        printErr("Expr");
        return false;
    }
    // 2) ( Expr ) Term' Mag' Rvalue'
    else if (lookaheadToken == "<punc,(>") {
        getNextToken(level + 1);  // consume '('
        if (!Expr(level + 1)) {
            printErr("Expr");
            return false;
        }
        if (getNextToken(level + 1) != "<punc,)>") {
            printErr("Expr");  // expected ')'
            return false;
        }
        // now parse Term' Mag' Rvalue'
        if (!Term_(level + 1)) {
            printErr("Expr");
            return false;
        }
        if (!Mag_(level + 1)) {
            printErr("Expr");
            return false;
        }
        if (!Rvalue_(level + 1)) {
            printErr("Expr");
            return false;
        }
        return true;
    }
    // 3) number Term' Mag' Rvalue'
    else if (isNum(lookaheadToken)) {
        getNextToken(level + 1);  // consume the number
        // now Term' Mag' Rvalue'
        if (!Term_(level + 1)) {
            printErr("Expr");
            return false;
        }
        if (!Mag_(level + 1)) {
            printErr("Expr");
            return false;
        }
        if (!Rvalue_(level + 1)) {
            printErr("Expr");
            return false;
        }
        return true;
    }

    // If none of the above matched, it's an error
    printErr("Expr");
    return false;
}

bool Expr_(int level) {
    printNode("Expr'", level);
    // Grammar:
    //  Expr' -> := Expr
    //          | Term' Mag' Rvalue'

    // 1) := Expr
    if (lookaheadToken == "<op,:=>") {
        getNextToken(level + 1);  // consume ':='
        if (!Expr(level + 1)) {
            printErr("Expr'");
            return false;
        }
        return true;
    }

    // 2) Term' Mag' Rvalue'
    // We'll try to parse all three in sequence
    if (Term_(level + 1) && Mag_(level + 1) && Rvalue_(level + 1)) {
        return true;
    }

    printErr("Expr'");
    return false;
}

bool Rvalue(int level) {
    printNode("Rvalue", level);
    // Rvalue -> Mag Rvalue_
    if (Mag(level + 1) && Rvalue_(level + 1)) {
        return true;
    }
    printErr("Rvalue");
    return false;
}

bool Rvalue_(int level) {
    printNode("Rvalue'", level);
    // Rvalue' -> Compare Mag Rvalue' | ?
    if (
        lookaheadToken == "<op,==>" ||
        lookaheadToken == "<op,<>" ||
        lookaheadToken == "<op,>>" ||
        lookaheadToken == "<op,<=>" ||
        lookaheadToken == "<op,>=>" ||
        lookaheadToken == "<op,!=>" ||
        lookaheadToken == "<op,<>>"
        )
    {
        if (Compare(level + 1) && Mag(level + 1) && Rvalue_(level + 1)) {
            return true;
        }
        printErr("Rvalue'");
        return false;
    }

    // ? - production
    printNode("(null)", level + 1);
    return true;
}

bool Mag(int level) {
    printNode("Mag", level);
    // Mag -> Term Mag_
    if (Term(level + 1) && Mag_(level + 1)) {
        return true;
    }
    printErr("Mag");
    return false;
}

bool Mag_(int level) {
    printNode("Mag'", level);
    // Mag' -> + Term Mag' | - Term Mag' | ?
    if (lookaheadToken == "<op,+>" || lookaheadToken == "<op,->") {
        string op = getNextToken(level + 1); // consume + or -
        if (Term(level + 1) && Mag_(level + 1)) {
            return true;
        }
        printErr("Mag_");
        return false;
    }

    // ?
    printNode("(null)", level + 1);
    return true;
}

bool Term(int level) {
    printNode("Term", level);
    // Term -> Factor Term_
    if (Factor(level + 1) && Term_(level + 1)) {
        return true;
    }
    printErr("Term");
    return false;
}

bool Term_(int level) {
    printNode("Term'", level);
    // Term' -> * Factor Term' | / Factor Term' | ?
    if (lookaheadToken == "<op,*>" || lookaheadToken == "<op,/>") {
        string op = getNextToken(level + 1); // consume * or /
        if (Factor(level + 1) && Term_(level + 1)) {
            return true;
        }
        printErr("Term_");
        return false;
    }

    // ?
    printNode("(null)", level + 1);
    return true;
}

bool Factor(int level) {
    printNode("Factor", level);
    // Factor -> ( Expr ) | identifier | number
    if (lookaheadToken == "<punc,(>") {
        getNextToken(level + 1); // consume '('
        if (!Expr(level + 1)) {
            printErr("Factor");
            return false;
        }
        if (getNextToken(level + 1) == "<punc,)>") {
            return true;
        }
        printErr("Factor");
        return false;
    }
    else if (isIdentifier(lookaheadToken)) {
        getNextToken(level + 1);
        return true;
    }
    else if (isNum(lookaheadToken)) {
        getNextToken(level + 1);
        return true;
    }
    printErr("Factor");
    return false;
}


int main() {

    //////************************************************************************LEXER PART ********************************************************************************
    //buffer1[BUFFER_SIZE - 1] = '\0';
    //buffer2[BUFFER_SIZE - 1] = '\0';

    //populateT();
    //populateA();
    //populateAccept();

    //ifstream file("testCode.txt");
    //if (!file) {
    //    cerr << "Error opening testCode.txt!" << endl;
    //    return 1;
    //}
    //loadBuffer(file, buffer1, inMultiLineComment);

    //if (classify(file) == -1) {
    //    cout << "error in files" << endl;
    //}

    //file.close();
    //cout << "Scanner has edited output files successfully!" << endl;
    //cout << "heheheheheheeeeeeeee" << endl;

    //////************************************************************************PARSER+SEMANTIC PART ********************************************************************************
    if (readfiles(toks) == -1) //lexer output in the vector
    {
        cout << "error in readFiles" << endl;
        return 0;
    }
    int level = 0;
    if (!Function(level)) {
        cout << "Error in tokens: " << endl;
        for (int i = pointer - 1;i < pointer + 3 && i < toks.size();i++) {
            cout << toks[i] << " ";
        }
    }

    return 0;

}


