/*
 * parser.h
 *
 *  Created on: Sept 16, 2020
 *      Author: Ethaiin Shealey
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <iostream>
#include <string>

#include "symbol.h"
#include "scanner.h"

using namespace std;

class parser {
public:

    parser(scanner* s);
    void PROG(); 

private:

    scanner* scan; // Copy of scanner

    // Functions
    void BLOCK(); 
    void DECLERATION(); 
    void TYPE();
    void PARAM_LIST();
    void PARAM();
    void PARAM_KIND();
    void STATEMENT_LIST();
    void STATEMENT();
    void SIMPLE_STATEMENT();
    void COMPOUND_STATEMENT();
    void EXPR();
    void SIMPLE_EXPR();
    void EXPR2();
    void TERM();
    void FACTOR();
    void PRIMARY();
    void RELOP();
    void IDENT();
    void IF_STATEMENT();
    void ELSIF_STATEMENT();
    void ELSE_STATEMENT();
    void LOOP_STATEMENT();
    void BOOL();
    void STRINGOP();
    void ADDOP();
    void MULTOP();
    void STRING();
    void IDENT2();
    void NUMBER();
    void DIGIT_SEQ();
    void EXP();
    void PRAGMA();
    void FOR_STATEMENT();
    void WHILE_STATEMENT();
    void RANGE();
    void IDENT_LIST();

    // Boolean Functions
    bool IS_EXPR();
    bool IS_BOOL();
    bool IS_RELOP();
    bool IS_MULTOP();
    bool IS_ADDOP();
    bool IS_PRIMARY();
    bool IS_NUMBER();
    bool IS_STRING();
    bool IS_FACTOR();
    bool IS_DECLERATION();
    bool IS_STATEMENT();
    
};

#endif