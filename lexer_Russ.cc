/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "IF", "WHILE", "DO", "THEN", "PRINT",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN",
    "NOTEQUAL", "GREATER", "LESS", "LTEQ", "GTEQ",
    "DOT", "NUM", "ID", "ERROR",  // TODO: Add labels for new token types here (as string)
    "REALNUM", "BASE08NUM", "BASE16NUM"
};

#define KEYWORDS_COUNT 5
string keyword[] = { "IF", "WHILE", "DO", "THEN", "PRINT" };

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanNumber()
{
    char c;
    string prior = "";
    string current = "";
    bool base16check = false;

    input.GetChar(c);
    // if (isdigit(c) || ( c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F')) {
    if (isdigit(c)) {
        if (c == '0') {
            tmp.lexeme = "0";
        } else {
            tmp.lexeme = "";
            while (!input.EndOfInput() && (isdigit(c) || ( c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F'))) {
                if (c == '8' || c =='9' || c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F')
                    base16check = true;
                tmp.lexeme += c;
                // tmp.lexeme += 'b';
                input.GetChar(c);
            }
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
        }
        // TODO: You can check for REALNUM, BASE08NUM and BASE16NUM here!
        
        //  ---------------------------------------------------- REALNUM
        if(c == '0' && tmp.lexeme.size() <= 1){ // This if handles cases like 00.00
            input.GetChar(c);                   // Ensures a 0 is always followed with a '.' for it to be realnum
            if(isdigit(c)){
            // if(isdigit(c) || ( c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F')){
                input.UngetChar(c);
                tmp.token_type = NUM;
                tmp.line_no = line_no;

                return tmp;
            }
            input.UngetChar(c);
        }
        
        

        if (!input.EndOfInput()){
            input.GetChar(c);

            if (c == '.'){
                bool broken = false;

                for(int i = 0; i < tmp.lexeme.size()-1; i++){
                    if(isalpha(tmp.lexeme[i]))
                        broken = true;
                }
                if(broken == true){
                    current = "";
                    input.UngetChar(c);
                    int ii = 0;
                    while(isdigit(tmp.lexeme[ii])){
                        current += tmp.lexeme[ii];
                        ii++;
                    }
                    for(int i = tmp.lexeme.size()-1; i > ii-1; i--){
                        input.UngetChar(tmp.lexeme[i]);
                    }

                    tmp.lexeme = current;
                    tmp.token_type = NUM;
                    tmp.line_no = line_no;
                    return tmp;
                }

                prior = tmp.lexeme;
                tmp.lexeme += c;
                current += c;
                input.GetChar(c);

                if(!isdigit(c)){
                // if(!isdigit(c) && (c != 'A' && c != 'B' && c != 'C' && c != 'D' && c != 'E' && c != 'F')){
                                
                    current += c;
                    input.UngetString(current);
                    tmp.lexeme = prior;
                    tmp.token_type = NUM;
                    tmp.line_no = line_no;

                    return tmp;
                }

                tmp.lexeme += c;
                input.GetChar(c);
                    // Continue scanning normally, 
                    // there must be at least 1 other digit in order for it to be REALNUM

                while (!input.EndOfInput() && isdigit(c)) { // Goes through adding to lexeme after '.'
                    tmp.lexeme += c;
                    input.GetChar(c);
                }
                
                if (!input.EndOfInput()){
                    input.UngetChar(c);
                }


                tmp.token_type = REALNUM;
                tmp.line_no = line_no;
                return tmp;
            }
        //  ---------------------------------------------------- REALNUM



            if (c == 'x' && base16check == false){
                prior = tmp.lexeme;
                tmp.lexeme += c;
                current += c;
                input.GetChar(c);

                if((c != '0')){    // if 0 is does not follow an x, it will immediately quit
                    if(c == '1')
                        goto jmp;

                    current = "";
                    input.UngetChar(c);
                    int ii = 0;
                    while(isdigit(tmp.lexeme[ii])){
                        current += tmp.lexeme[ii];
                        ii++;
                    }
                    for(int i = tmp.lexeme.size()-1; i > ii-1; i--){
                        input.UngetChar(tmp.lexeme[i]);
                    }

                    tmp.lexeme = current;
                    tmp.token_type = NUM;
                    tmp.line_no = line_no;
                    return tmp;
                }
                
                tmp.lexeme += c;
                current += c;
                input.GetChar(c);

                    if((c != '8')){    // if 8 does not follow the 0, it will immediately quit

                    current = "";
                    input.UngetChar(c);
                    int ii = 0;
                    while(isdigit(tmp.lexeme[ii])){
                        current += tmp.lexeme[ii];
                        ii++;
                    }
                    for(int i = tmp.lexeme.size()-1; i > ii-1; i--){
                        input.UngetChar(tmp.lexeme[i]);
                    }

                    tmp.lexeme = current;
                    tmp.token_type = NUM;
                    tmp.line_no = line_no;
                    return tmp;
                }

                tmp.lexeme += c;
                tmp.token_type = BASE08NUM;
                tmp.line_no = line_no;
                return tmp;
            }


            if (c == 'x' && base16check == true){
                prior = tmp.lexeme;
                tmp.lexeme += c;
                current += c;
                input.GetChar(c);

                if((c != '1')){    // if 0 is does not follow an x, it will immediately quit
                    current = "";
                    input.UngetChar(c);
                    int ii = 0;
                    while(isdigit(tmp.lexeme[ii])){
                        current += tmp.lexeme[ii];
                        ii++;
                    }
                    for(int i = tmp.lexeme.size()-1; i > ii-1; i--){
                        input.UngetChar(tmp.lexeme[i]);
                    }

                    tmp.lexeme = current;
                    tmp.token_type = NUM;
                    tmp.line_no = line_no;
                    return tmp;
                }
                jmp:
                
                tmp.lexeme += c;
                // current += c;
                input.GetChar(c);

                if((c != '6')){    // if 8 does not follow the 0, it will immediately quit
                    current = "";
                    input.UngetChar(c);
                    int ii = 0;
                    while(isdigit(tmp.lexeme[ii])){
                        current += tmp.lexeme[ii];
                        ii++;
                    }
                    for(int i = tmp.lexeme.size()-1; i > ii-1; i--){
                        input.UngetChar(tmp.lexeme[i]);
                    }

                    tmp.lexeme = current;
                    tmp.token_type = NUM;
                    tmp.line_no = line_no;
                    return tmp;
                }
                tmp.lexeme += c;
                tmp.token_type = BASE16NUM;
                tmp.line_no = line_no;
                return tmp;
            }

            input.UngetChar(c);
        }

        int ii = 0;
        while(isdigit(tmp.lexeme[ii])){
            current += tmp.lexeme[ii];
            ii++;
        }
        for(int i = tmp.lexeme.size()-1; i > ii-1; i--){
                input.UngetChar(tmp.lexeme[i]);
        }

        tmp.lexeme = current;
        tmp.token_type = NUM;
        tmp.line_no = line_no;
        return tmp;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
        case '.':
            tmp.token_type = DOT;
            return tmp;
        case '+':
            tmp.token_type = PLUS;
            return tmp;
        case '-':
            tmp.token_type = MINUS;
            return tmp;
        case '/':
            tmp.token_type = DIV;
            return tmp;
        case '*':
            tmp.token_type = MULT;
            return tmp;
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '[':
            tmp.token_type = LBRAC;
            return tmp;
        case ']':
            tmp.token_type = RBRAC;
            return tmp;
        case '(':
            tmp.token_type = LPAREN;
            return tmp;
        case ')':
            tmp.token_type = RPAREN;
            return tmp;
        case '<':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = LTEQ;
            } else if (c == '>') {
                tmp.token_type = NOTEQUAL;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = LESS;
            }
            return tmp;
        case '>':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = GTEQ;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = GREATER;
            }
            return tmp;
        default:
            if (isdigit(c)) {
            // if (isdigit(c) || ( c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F')) {
                input.UngetChar(c);
                return ScanNumber();
            } else if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

            return tmp;
    }
}

int main()
{
    LexicalAnalyzer lexer;
    Token token;

    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }
}
