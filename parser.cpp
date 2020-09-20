/*
 * parser.cpp
 *
 *  Created on: Sept 16, 2020
 *      Author: Ethan Shealey
 */

#include <iostream>
#include "symbol.h"
#include "scanner.h"
#include "parser.h"

using namespace std;

parser::parser(scanner* s) {
    scan=s;
}

void parser::PROG() { // Begin program
    //scan->get_token();
    scan->must_be(symbol::program_sym);
    IDENT();
    scan->must_be(symbol::is_sym);
    BLOCK();
    scan->must_be(symbol::semicolon_sym);
}

void parser::BLOCK() {
    while (IS_DECLERATION()) {
         DECLERATION();
    }
    scan->must_be(symbol::begin_sym);
    STATEMENT_LIST();
    scan->must_be(symbol::end_sym);
    if (scan->have(symbol::identifier)) {
        IDENT();
    }
}

void parser::DECLERATION() {
    if (scan->have(symbol::identifier)) {
        IDENT_LIST();
        scan->must_be(symbol::colon_sym);
         if (scan->have(symbol::constant_sym)) {
            scan->must_be(symbol::constant_sym);
         }
         TYPE();
         if (scan->have(symbol::becomes_sym)) {
            scan->must_be(symbol::becomes_sym);
            if (scan->have(symbol::real_num) or scan->have(symbol::integer)) 
                NUMBER();
            else if (scan->have(symbol::string_sym)) 
                scan->must_be(symbol::string_sym);
            else if (scan->have(symbol::true_sym) or scan->have(symbol::false_sym)) 
                BOOL();
        }
        scan->must_be(symbol::semicolon_sym);
    } 
    else if (scan->have(symbol::procedure_sym)) {
        scan->must_be(symbol::procedure_sym);
        IDENT();
        if (scan->have(symbol::left_paren_sym)) {
            scan->must_be(symbol::left_paren_sym);
            PARAM_LIST();
            scan->must_be(symbol::right_paren_sym);
        }
        scan->must_be(symbol::is_sym);
        BLOCK();
        scan->must_be(symbol::semicolon_sym);
    } 
    else if (scan->have(symbol::function_sym)) {
        scan->must_be(symbol::function_sym);
        IDENT();
        if (scan->have(symbol::left_paren_sym)) {
            scan->must_be(symbol::left_paren_sym);
            PARAM_LIST();
            scan->must_be(symbol::right_paren_sym);
        }
        scan->must_be(symbol::return_sym);
        TYPE();
        scan->must_be(symbol::is_sym);
        BLOCK();
        scan->must_be(symbol::semicolon_sym);
    }
}

void parser::STATEMENT_LIST() {
    //need a while loop here, could have many statements.
    STATEMENT();
    scan->must_be(symbol::semicolon_sym);
    while (IS_STATEMENT()) {
        STATEMENT();
        scan->must_be(symbol::semicolon_sym);
    }
    
}

void parser::IDENT_LIST() {
    IDENT();
    while (scan->have(symbol::comma_sym)) {
        scan->must_be(symbol::comma_sym);
        IDENT();
    }
}

void parser::TYPE() {
    if (scan->have(symbol::integer_sym)) 
        scan->must_be(symbol::integer_sym);
    else if (scan->have(symbol::real_sym)) 
        scan->must_be(symbol::real_sym);
    else if(scan->have(symbol::string_sym)) 
        scan->must_be(symbol::string_sym);
    else 
        scan->must_be(symbol::boolean_sym);
}

void parser::NUMBER() {
    if (scan->have(symbol::integer)) 
        scan->must_be(symbol::integer);
    else 
        scan->must_be(symbol::real_num);
}

void parser::BOOL() {
    if (scan->have(symbol::true_sym)) 
        scan->must_be(symbol::true_sym);
    else 
        scan->must_be(symbol::false_sym);
}

void parser::PARAM_LIST() {
    PARAM();
    while (scan->have(symbol::semicolon_sym)) {
        scan->must_be(symbol::semicolon_sym);
        PARAM();
    }
}

void parser::STATEMENT() {
    if (scan->have(symbol::if_sym) or scan->have(symbol::while_sym) or scan->have(symbol::for_sym) or scan->have(symbol::loop_sym)) 
        COMPOUND_STATEMENT();
    else 
        SIMPLE_STATEMENT();
    
}

void parser::PARAM() {
    IDENT_LIST();
    scan->must_be(symbol::colon_sym);
    PARAM_KIND();
    TYPE();
}

void parser::COMPOUND_STATEMENT() {
    if (scan->have(symbol::if_sym)) 
        IF_STATEMENT();
    else if (scan->have(symbol::loop_sym)) 
        LOOP_STATEMENT();
    else if (scan->have(symbol::for_sym)) 
        FOR_STATEMENT();
    else if (scan->have(symbol::while_sym)) 
        WHILE_STATEMENT();
}

void parser::SIMPLE_STATEMENT() {
    if (scan->have(symbol::identifier)) {
        IDENT();
        if (scan->have(symbol::left_paren_sym)) {
            scan->must_be(symbol::left_paren_sym);
            EXPR();
            while(scan->have(symbol::comma_sym)){
                scan->must_be(symbol::comma_sym);
                EXPR();
            }
            scan->must_be(symbol::right_paren_sym);
        } 
        else {
            scan->must_be(symbol::becomes_sym);
            EXPR();
        }
    } 
    else if (scan->have(symbol::exit_sym)) {
        scan->must_be(symbol::exit_sym);
        if (scan->have(symbol::when_sym)) {
            scan->must_be(symbol::when_sym);
            EXPR();
        }
    } 
    else if (scan->have(symbol::return_sym)) {
        scan->must_be(symbol::return_sym);
        if (IS_EXPR()) {
            EXPR();
        }
    } 
    else if (scan->have(symbol::read_sym)) {
        scan->must_be(symbol::read_sym);
        bool lp = false;
        if (scan->have(symbol::left_paren_sym)) {
            scan->must_be(symbol::left_paren_sym);
            lp = true;
        }
        IDENT();
        while (scan->have(symbol::comma_sym)) {
            scan->must_be(symbol::comma_sym);
            IDENT();
        }

        if (lp) {
            scan->must_be(symbol::right_paren_sym);
        }
    } 
    else if (scan->have(symbol::write_sym)) {
        scan->must_be(symbol::write_sym);
        bool lp = false;
        if (scan->have(symbol::left_paren_sym)) {
            scan->must_be(symbol::left_paren_sym);
            lp = true;
        }
        EXPR();
        while (scan->have(symbol::comma_sym)) {
            scan->must_be(symbol::comma_sym);
            EXPR();
        }
        if (lp) {
            scan->must_be(symbol::right_paren_sym);
        } 
        }
        else if (scan->have(symbol::writeln_sym)) {
            scan->must_be(symbol::writeln_sym);
            bool lp = false;
            if (scan->have(symbol::left_paren_sym)) {
                scan->must_be(symbol::left_paren_sym);
                lp = true;
            }
            if (IS_EXPR()) {
                EXPR();
                while (scan->have(symbol::comma_sym)) {
                    scan->must_be(symbol::comma_sym);
                    EXPR();
                }
            }
            if (lp) {
                scan->must_be(symbol::right_paren_sym);
            }
        } 
        else {
            scan->must_be(symbol::null_sym);
        }
    
}

void parser::IF_STATEMENT() {
    scan->must_be(symbol::if_sym);
    EXPR();
    scan->must_be(symbol::then_sym);
    STATEMENT_LIST();
    while (scan->have(symbol::elsif_sym)) {
        scan->must_be(symbol::elsif_sym);
        EXPR();
        scan->must_be(symbol::then_sym);
        STATEMENT_LIST();
    }
    if (scan->have(symbol::else_sym)) {
        scan->must_be(symbol::else_sym);
        STATEMENT_LIST();
    }
    scan->must_be(symbol::end_sym);
    scan->must_be(symbol::if_sym);
}

void parser::LOOP_STATEMENT() {
    scan->must_be(symbol::loop_sym);
    STATEMENT_LIST();
    scan->must_be(symbol::end_sym);
    scan->must_be(symbol::loop_sym);
}

void parser::FOR_STATEMENT() {
    scan->must_be(symbol::for_sym);
    IDENT();
    scan->must_be(symbol::in_sym);
    if (scan->have(symbol::reverse_sym)) {
        scan->must_be(symbol::reverse_sym);
    }
    RANGE();
    LOOP_STATEMENT();
}

void parser::WHILE_STATEMENT() {
    scan->must_be(symbol::while_sym);
    EXPR();
    LOOP_STATEMENT();
}

void parser::PARAM_KIND() {
    if (scan->have(symbol::value_sym)) 
        scan->must_be(symbol::value_sym);
    else 
        scan->must_be(symbol::ref_sym);
}

void parser::EXPR() {
    SIMPLE_EXPR();
    if (IS_RELOP()) {
        RELOP();
        SIMPLE_EXPR();
    } 
    else {
        if (scan->have(symbol::in_sym)) {
            scan->must_be(symbol::in_sym);
            RANGE();
        }
    }

}

void parser::RANGE() {
    SIMPLE_EXPR();
    scan->must_be(symbol::range_sym);
    SIMPLE_EXPR();
}

void parser::SIMPLE_EXPR() {
    EXPR2();
    while (scan->have(symbol::ampersand_sym)) {
        scan->must_be(symbol::ampersand_sym);
        EXPR2();
    }
}

void parser::RELOP() {
    if (scan->have(symbol::greater_than_sym)) 
        scan->must_be(symbol::greater_than_sym);
    else if (scan->have(symbol::less_than_sym)) 
        scan->must_be(symbol::less_than_sym);
    else if (scan->have(symbol::equals_sym)) 
        scan->must_be(symbol::equals_sym);
    else if (scan->have(symbol::not_equals_sym)) 
        scan->must_be(symbol::not_equals_sym);
    else if (scan->have(symbol::less_or_equal_sym)) 
        scan->must_be(symbol::less_or_equal_sym);
    else 
        scan->must_be(symbol::greater_or_equal_sym);
}

void parser::EXPR2() {
    TERM();
    while (IS_ADDOP() or scan->have(symbol::or_sym)) {
        if (IS_ADDOP()) 
            ADDOP();
        else 
            scan->must_be(symbol::or_sym);
        TERM();
    }
}

void parser::TERM() {
    FACTOR();
    while (IS_MULTOP() or scan->have(symbol::and_sym)) {
        if (IS_MULTOP()) 
            MULTOP();
        else 
            scan->must_be(symbol::and_sym);
        FACTOR();
    }
}

void parser::FACTOR() {
    PRIMARY();
    if (IS_ADDOP()) {
        ADDOP();
        PRIMARY();
    } 
    else {
        if (scan->have(symbol::power_sym)) {
            scan->must_be(symbol::power_sym);
            PRIMARY();
        }
        
    }
}

void parser::ADDOP() {
    if (scan->have(symbol::plus_sym)) 
        scan->must_be(symbol::plus_sym);
    else 
        scan->must_be(symbol::minus_sym);
}

void parser::MULTOP() {
    if (scan->have(symbol::asterisk_sym)) 
        scan->must_be(symbol::asterisk_sym);
    else 
        scan->must_be(symbol::slash_sym);
}

void parser::PRIMARY() {
    if (scan->have(symbol::not_sym)) {
        scan->must_be(symbol::not_sym);
        EXPR();
    } 
    else if (scan->have(symbol::odd_sym)) {
        scan->must_be(symbol::odd_sym);
        EXPR();
    } 
    else if (scan->have(symbol::left_paren_sym)) {
        scan->must_be(symbol::left_paren_sym);
        SIMPLE_EXPR();
        scan->must_be(symbol::right_paren_sym);
    } 
    else if (scan->have(symbol::identifier)) {
        IDENT();
        if (scan->have(symbol::left_paren_sym)) {
            scan->must_be(symbol::left_paren_sym);
            EXPR();
            while (scan->have(symbol::comma_sym)) {
                scan->must_be(symbol::comma_sym);
                EXPR();
            }
            scan->must_be(symbol::right_paren_sym);
        }
        
    }
    else if (IS_NUMBER()) 
        NUMBER();
    else if (scan->have(symbol::strng)) 
        scan->must_be(symbol::strng);
    else
        BOOL();
}

bool parser::IS_NUMBER() {
    if (scan->have(symbol::real_num) or scan->have(symbol::integer)) 
        return true;
    return false;
}

bool parser::IS_ADDOP() {
    if (scan->have(symbol::plus_sym) or scan->have(symbol::minus_sym)) 
        return true;
    return false;
}

bool parser::IS_MULTOP() {
    if (scan->have(symbol::asterisk_sym) or scan->have(symbol::slash_sym))
        return true;
    return false;
}

bool parser::IS_RELOP() {
    if (scan->have(symbol::less_than_sym) or scan->have(symbol::equals_sym) or scan->have(symbol::greater_than_sym) or scan->have(symbol::less_or_equal_sym) or scan->have(symbol::greater_or_equal_sym) or scan->have(symbol::not_equals_sym)) 
        return true;
    return false;
}

bool parser::IS_EXPR() {
    if (scan->have(symbol::not_sym) or scan->have(symbol::odd_sym) or scan->have(symbol::left_paren_sym) or scan->have(symbol::identifier) or IS_NUMBER() or scan->have(symbol::strng) or IS_BOOL() or IS_ADDOP()) 
        return true;
    return false;
}

bool parser::IS_BOOL() {
    if (scan->have(symbol::true_sym) or scan->have(symbol::false_sym)) 
        return true;
    return false;
}

bool parser::IS_STATEMENT() {
    if (scan->have(symbol::identifier) or scan->have(symbol::exit_sym) or scan->have(symbol::return_sym) or scan->have(symbol::read_sym) or scan->have(symbol::write_sym) or scan->have(symbol::writeln_sym) or scan->have(symbol::null_sym) or scan->have(symbol::if_sym) or scan->have(symbol::loop_sym) or scan->have(symbol::for_sym) or scan->have(symbol::while_sym))
        return true;
    return false;
}

bool parser::IS_DECLERATION() {
    if (scan->have(symbol::identifier) or scan->have(symbol::procedure_sym) or scan->have(symbol::function_sym)) 
        return true;
    return false;
}

void parser::IDENT() {
    scan->must_be(symbol::identifier);
}