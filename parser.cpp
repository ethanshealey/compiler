/*
 * parser.cpp
 *
 *  Created on: Sept 16, 2020
 *      Author: Michael Oudshoorn
 */

#include <iostream>
#include "parser.h"
#include "symbol.h"
#include "scanner.h"

parser::parser(scanner* s) {
    scan=s;
}

void parser::PROG() {
    scan->must_be(symbol::program_sym);
    IDENT();
    scan->must_be(symbol::is_sym);
    BLOCK();
    scan->must_be(symbol::semicolon_sym);
}

void parser::BLOCK() {
    while(IS_IDENT() or scan->have(symbol::procedure_sym) or scan->have(symbol::function_sym))
        DECLERATION();
    scan->must_be(symbol::begin_sym);
    STATEMENT_LIST();
    scan->must_be(symbol::end_sym);
    if(IS_IDENT())
        IDENT();
}

void parser::DECLERATION() {
    if(IS_IDENT()) { // IDENTIFIER
        IDENT_LIST();
        scan->must_be(symbol::colon_sym);
        if(scan->have(symbol::constant_sym))
            scan->must_be(symbol::constant_sym);
        TYPE();
    }
    else if(scan->have(symbol::procedure_sym)) { // PROCEDURE
        scan->must_be(symbol::procedure_sym);
        IDENT();
        if(scan->have(symbol::left_paren_sym)) {
            scan->must_be(symbol::left_paren_sym);
            PARAM_LIST();
            scan->must_be(symbol::right_paren_sym);
        }
        scan->must_be(symbol::is_sym);
        BLOCK();
        //scan->must_be(symbol::semicolon_sym);
    }
    else if(scan->have(symbol::function_sym)) { // FUNCTION
        scan->must_be(symbol::function_sym);
        IDENT();
        if(scan->have(symbol::left_paren_sym)) {
            scan->must_be(symbol::left_paren_sym);
            PARAM_LIST();
            scan->must_be(symbol::right_paren_sym);
        }
        scan->must_be(symbol::is_sym);
        BLOCK();
        //scan->must_be(symbol::semicolon_sym);
    }
    scan->must_be(symbol::semicolon_sym);
}

void parser::IDENT_LIST() {
    IDENT();
    while(scan->have(symbol::comma_sym))  {
        scan->must_be(symbol::comma_sym);
        IDENT();
    }
}

void parser::TYPE() {
    if(scan->have(symbol::integer_sym))
        scan->must_be(symbol::integer_sym);
    else if(scan->have(symbol::real_sym))
        scan->must_be(symbol::real_sym);
    else if(scan->have(symbol::string_sym))
        scan->must_be(symbol::string_sym);
    else if(scan->have(symbol::boolean_sym))
        scan->must_be(symbol::boolean_sym);
}

void parser::PARAM_LIST() {
    PARAM();
    while(scan->have(symbol::semicolon_sym)) {
        scan->must_be(symbol::semicolon_sym);
        PARAM();
    }
}

void parser::PARAM() {
    IDENT_LIST();
    scan->must_be(symbol::colon_sym);
    PARAM_KIND();
    TYPE();
}

void parser::PARAM_KIND() {
    if(scan->have(symbol::value_sym))
        scan->must_be(symbol::value_sym);
    else if(scan->have(symbol::ref_sym))
        scan->must_be(symbol::ref_sym);
}

void parser::STATEMENT_LIST() {
    STATEMENT();
    scan->must_be(symbol::semicolon_sym);
    while(IS_SIMPLE_STATEMENT() or IS_COMPOUND_STATEMENT()) {
        STATEMENT();
        scan->must_be(symbol::semicolon_sym);
    }
}

void parser::STATEMENT() {
    if(IS_SIMPLE_STATEMENT()) 
        SIMPLE_STATEMENT();
    else if(IS_COMPOUND_STATEMENT()) 
        COMPOUND_STATEMENT();
}

bool parser::IS_SIMPLE_STATEMENT() {
    return (IS_IDENT() or 
              scan->have(symbol::exit_sym) or 
              scan->have(symbol::return_sym) or 
              scan->have(symbol::read_sym) or 
              scan->have(symbol::write_sym) or 
              scan->have(symbol::writeln_sym) or
              scan->have(symbol::null_sym));
}

bool parser::IS_COMPOUND_STATEMENT() {
    return (scan->have(symbol::if_sym) or
              scan->have(symbol::loop_sym) or
              scan->have(symbol::for_sym) or
              scan->have(symbol::while_sym));
}

void parser::SIMPLE_STATEMENT() {
    if(IS_IDENT()) {
        IDENT();
        if(scan->have(symbol::becomes_sym)) {
            scan->must_be(symbol::becomes_sym);
            EXPR();
        }
        else if(scan->have(symbol::left_paren_sym)) {
            scan->must_be(symbol::left_paren_sym);
            EXPR();
            while(scan->have(symbol::comma_sym)) {
                scan->must_be(symbol::comma_sym);
                EXPR();
            }
            scan->must_be(symbol::right_paren_sym);
        }
    }
    else if(scan->have(symbol::exit_sym)) {
        scan->must_be(symbol::exit_sym);
        if(scan->have(symbol::when_sym)) {
            scan->must_be(symbol::when_sym);
            EXPR();
        }
    }
    else if(scan->have(symbol::return_sym)) {
        scan->must_be(symbol::return_sym);
        if(IS_EXPR())
            EXPR();
    }
    else if(scan->have(symbol::read_sym)) {
        scan->must_be(symbol::read_sym);
        if(scan->have(symbol::left_paren_sym))
            scan->must_be(symbol::left_paren_sym);
        IDENT_LIST();
        if(scan->have(symbol::right_paren_sym))
            scan->must_be(symbol::right_paren_sym);

    }
    else if(scan->have(symbol::write_sym)) {
        scan->must_be(symbol::write_sym);
         if(scan->have(symbol::left_paren_sym))
            scan->must_be(symbol::left_paren_sym);
        EXPR();
        while(scan->have(symbol::comma_sym)) {
            scan->must_be(symbol::comma_sym);
            EXPR();
        }
        if(scan->have(symbol::right_paren_sym))
            scan->must_be(symbol::right_paren_sym);

    }
    else if(scan->have(symbol::writeln_sym)) {
        scan->must_be(symbol::writeln_sym);
         if(scan->have(symbol::left_paren_sym))
            scan->must_be(symbol::left_paren_sym);
        EXPR();
        while(scan->have(symbol::comma_sym)) {
            scan->must_be(symbol::comma_sym);
            EXPR();
        }
        if(scan->have(symbol::right_paren_sym))
            scan->must_be(symbol::right_paren_sym);
    }
    else if(scan->have(symbol::null_sym)) {
        scan->must_be(symbol::null_sym);
    }
}

void parser::COMPOUND_STATEMENT() {
    if(scan->have(symbol::if_sym)) {
        IF_STATEMENT();
    }
    else if(scan->have(symbol::loop_sym)) {
        LOOP_STATEMENT();
    }
    if(scan->have(symbol::for_sym)) {
        FOR_STATEMENT();
    }
    if(scan->have(symbol::while_sym)) {
        WHILE_STATEMENT();
    }
}

bool parser::IS_RELOP() {
    return (scan->have(symbol::greater_than_sym) or
              scan->have(symbol::less_than_sym) or
              scan->have(symbol::equals_sym) or
              scan->have(symbol::not_equals_sym) or
              scan->have(symbol::greater_or_equal_sym) or
              scan->have(symbol::less_or_equal_sym));
}

void parser::EXPR() {
    SIMPLE_EXPR();
    if(IS_RELOP()) {
        RELOP();
        SIMPLE_EXPR();
    }
    else if(scan->have(symbol::in_sym)) {
        scan->must_be(symbol::in_sym);
        RANGE();
    }
}

void parser::RELOP() {
    if(scan->have(symbol::greater_than_sym))
        scan->must_be(symbol::greater_than_sym);
    else if(scan->have(symbol::less_than_sym))
        scan->must_be(symbol::less_than_sym);
    else if(scan->have(symbol::equals_sym))
        scan->must_be(symbol::equals_sym);
    else if(scan->have(symbol::greater_or_equal_sym))
        scan->must_be(symbol::greater_or_equal_sym);
    else if(scan->have(symbol::less_or_equal_sym))
        scan->must_be(symbol::less_or_equal_sym);
}

void parser::RANGE() {
    SIMPLE_EXPR();
    scan->must_be(symbol::range_sym);
    SIMPLE_EXPR();
}

bool parser::IS_IDENT() {
    return scan->have(symbol::identifier);
}

void parser::IDENT() {
    scan->must_be(symbol::identifier);
}

void parser::IF_STATEMENT() {
    scan->must_be(symbol::if_sym);
    EXPR();
    scan->must_be(symbol::then_sym);
    STATEMENT_LIST();
    while(scan->have(symbol::elsif_sym)) {
        scan->must_be(symbol::elsif_sym);
        EXPR();
        scan->must_be(symbol::then_sym);
        STATEMENT_LIST();
    }
    if(scan->have(symbol::else_sym)) {
        scan->must_be(symbol::else_sym);
        EXPR();
    }
    scan->must_be(symbol::end_sym);
    scan->must_be(symbol::if_sym);
}

void parser::WHILE_STATEMENT() {
    scan->must_be(symbol::while_sym);
    EXPR();
    LOOP_STATEMENT();
}

void parser::FOR_STATEMENT() {
    scan->must_be(symbol::for_sym);
    IDENT();
    scan->must_be(symbol::in_sym);
    if(scan->have(symbol::reverse_sym))
        scan->must_be(symbol::reverse_sym);
    RANGE();
    LOOP_STATEMENT();
}

void parser::LOOP_STATEMENT() {
    scan->must_be(symbol::loop_sym);
    STATEMENT_LIST();
    scan->must_be(symbol::end_sym);
    scan->must_be(symbol::loop_sym);
}

void parser::BOOL() {
    if(scan->have(symbol::true_sym))
        scan->must_be(symbol::true_sym);
    else
        scan->must_be(symbol::false_sym);
}

void parser::SIMPLE_EXPR() {
    EXPR2();
    while(scan->have(symbol::ampersand_sym)) {
        scan->must_be(symbol::ampersand_sym);
        EXPR2();
    }
}

void parser::STRINGOP() {
    scan->must_be(symbol::ampersand_sym);
}

void parser::EXPR2() {
    TERM();
    while(IS_ADDOP() or scan->have(symbol::or_sym) or IS_TERM()) {
        if(IS_ADDOP())
            ADDOP();
        else if(scan->have(symbol::or_sym))
            scan->must_be(symbol::or_sym);
        else if(IS_TERM()) 
            TERM();
    }
}

bool parser::IS_ADDOP() {
    return (scan->have(symbol::plus_sym) or scan->have(symbol::minus_sym));
}

bool parser::IS_TERM() {
   return IS_PRIMARY();
}

bool parser::IS_PRIMARY() {
    return (scan->have(symbol::not_sym) or
              scan->have(symbol::odd_sym) or
              scan->have(symbol::left_paren_sym) or
              IS_IDENT() or
              IS_NUMBER() or
              IS_STRING() or
              IS_BOOL());
}

bool parser::IS_NUMBER() {
    return (scan->have(symbol::integer_sym) or scan->have(symbol::real_sym));
}

void parser::NUMBER() {
    if(scan->have(symbol::integer_sym)) {
        scan->must_be(symbol::integer_sym);
    }
    else
        scan->must_be(symbol::real_sym);
}

bool parser::IS_STRING() {
    return (scan->have(symbol::string_sym));
}

void parser::STRING() {
    scan->must_be(symbol::string_sym);
}

void parser::TERM() {
    FACTOR();
    while(IS_MULTOP() or scan->have(symbol::and_sym) or IS_FACTOR()) {
        if(IS_MULTOP()) 
            MULTOP();
        else
            scan->must_be(symbol::and_sym);
        FACTOR();
    }
}

bool parser::IS_FACTOR() {
    return IS_PRIMARY();
}

void parser::FACTOR() {
    if(IS_PRIMARY()) {
        PRIMARY();
        if(scan->have(symbol::power_sym)) {
            scan->must_be(symbol::power_sym);
            PRIMARY();
        }
    }
    else {
        ADDOP();
        PRIMARY();
    }

}

void parser::PRIMARY() {
    if(scan->have(symbol::not_sym))
        scan->must_be(symbol::not_sym);
    else if(scan->have(symbol::odd_sym))
        scan->must_be(symbol::odd_sym);
    else if(scan->have(symbol::left_paren_sym))
        scan->must_be(symbol::left_paren_sym);
    else if(IS_IDENT())
        IDENT();
    else if(IS_NUMBER())
        NUMBER();
    else if(IS_STRING())
        STRING();
    else if(IS_BOOL())
        BOOL();
}

void parser::IDENT2() {
    if(scan->have(symbol::integer_sym))
        scan->must_be(symbol::integer_sym);
    else
        scan->must_be(symbol::real_sym);
}

bool parser::IS_MULTOP() {
    return (scan->have(symbol::asterisk_sym) or scan->have(symbol::slash_sym));
}

void parser::MULTOP() {
    if(scan->have(symbol::asterisk_sym))
        scan->must_be(symbol::asterisk_sym);
    else
        scan->must_be(symbol::slash_sym);
}

void parser::ADDOP() {
    if(scan->have(symbol::plus_sym))
        scan->must_be(symbol::plus_sym);
    else
        scan->must_be(symbol::minus_sym);
}

bool parser::IS_BOOL() {
    return (scan->have(symbol::true_sym) or scan->have(symbol::false_sym));
}

bool parser::IS_EXPR() {
    return IS_PRIMARY();
}