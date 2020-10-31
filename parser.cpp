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

lille_type parser::get_ident_type() {

    /***
     * get_ident_type
     * 
     * Returns the current symbols type
     * 
     */

    switch (scan->this_token()->get_symbol()->get_sym()) {
        case symbol::integer_sym:
            return lille_type::type_integer;
        case symbol::real_sym:
            return lille_type::type_real;
        case symbol::boolean_sym:
            return lille_type::type_boolean;
        case symbol::string_sym:
            return lille_type::type_string;
        default:
            return lille_type::type_unknown;
    }
}

void parser::handle_function_or_procedure_call(id_table_entry* current_entry) {

    /***
     * handle_function_or_procedure_call
     * 
     * Handles the arguments in a function or
     * procedure call
     * 
     */

    string current_entry_name = current_entry->name();
    if(current_entry->tipe().is_type(lille_type::type_func)) {
        /**** HANDEL FUNCTION CALL ****/
        for(int i = 0; i < current_entry->number_of_params(); i++) {
            if(scan->have(symbol::identifier)) {
                current_ident = table->lookup(scan->get_current_identifier_name());
                if(not current_entry->nth_parameter(i)->tipe().is_type(current_ident->tipe())) {
                    throw lille_exception("Identifier " + current_ident->name() + " Does Not Match Parameter Type in " + current_entry_name);
                }
                scan->must_be(symbol::identifier);
            }
            else {
                if(not current_entry->nth_parameter(i)->tipe().is_type(get_type())) 
                    throw lille_exception("Value Given Does Not Match Parameter Type in " + current_entry_name);
                if(scan->have(symbol::integer))
                   scan->must_be(symbol::integer);
                else if(scan->have(symbol::real_num))
                    scan->must_be(symbol::real_num);
                else if(scan->have(symbol::strng))
                    scan->must_be(symbol::strng);
                else if(scan->have(symbol::true_sym))
                    scan->must_be(symbol::true_sym);
                else if(scan->have(symbol::false_sym))
                    scan->must_be(symbol::false_sym);
                else throw lille_exception("Invalid type given");
            }
            if(scan->have(symbol::comma_sym))
                scan->must_be(symbol::comma_sym);
        }
    }
    else if(current_entry->tipe().is_type(lille_type::type_proc)) {
        /**** HANDEL PROCEDURE CALL ****/
        for(int i = 0; i < current_entry->number_of_params(); i++) {
            if(scan->have(symbol::identifier)) {
                current_ident = table->lookup(scan->get_current_identifier_name());
                if(not current_entry->nth_parameter(i)->tipe().is_type(current_ident->tipe())) {
                    throw lille_exception("Identifier " + current_ident->name() + " Does Not Match Parameter Type in " + current_entry_name);
                }
                scan->must_be(symbol::identifier);
            }
            else {
                if(not current_entry->nth_parameter(i)->tipe().is_type(get_type())) 
                    throw lille_exception("Value Given Does Not Match Parameter Type in " + current_entry_name);
                if(scan->have(symbol::integer))
                   scan->must_be(symbol::integer);
                else if(scan->have(symbol::real_num))
                    scan->must_be(symbol::real_num);
                else if(scan->have(symbol::strng))
                    scan->must_be(symbol::strng);
                else if(scan->have(symbol::true_sym))
                    scan->must_be(symbol::true_sym);
                else if(scan->have(symbol::false_sym))
                    scan->must_be(symbol::false_sym);
                else throw lille_exception("Invalid type given");
            }
            if(scan->have(symbol::comma_sym))
                scan->must_be(symbol::comma_sym);
        }
        
    }
    else throw lille_exception("Unkown Function/Procedure " + current_entry_name);

    scan->must_be(symbol::right_paren_sym);
}

void parser::handle_if_and_while() {

    /***
     * handle_if
     * 
     * Handles the binary evaluation of 
     * an if/elsif statement
     * 
     */
    
    bool and_or_flag;
    do {
        if(scan->have(symbol::identifier)) {
            id_table_entry* if_cond = table->lookup(scan->get_current_identifier_name());
            if(if_cond == NULL)
                throw lille_exception("Undeclared identifier " + scan->get_current_identifier_name());
            scan->must_be(symbol::identifier);
            if(IS_RELOP()) {
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
                else  if (scan->have(symbol::greater_or_equal_sym))
                    scan->must_be(symbol::greater_or_equal_sym);
                else
                    throw lille_exception("Expected a logical symbol (> < <> = >= <=)");
                if(scan->have(symbol::identifier)) {
                    id_table_entry* if_cond2 = table->lookup(scan->get_current_identifier_name());
                    if(if_cond2 == NULL)
                        throw lille_exception("Undeclared identifier " + scan->get_current_identifier_name());
                    if(not if_cond->tipe().is_type(if_cond2->tipe())) 
                        throw lille_exception("Cannot compare 2 items of different types");
                }
                else if(scan->have(symbol::integer) or scan->have(symbol::real_num)) {
                    if(not if_cond->tipe().is_type(get_type()))
                        throw lille_exception("Can only compare values of same type");
                    if(scan->have(symbol::integer))
                        scan->must_be(symbol::integer);
                    else
                        scan->must_be(symbol::real_num);
                }
                else if(scan->have(symbol::true_sym)) {
                    scan->must_be(symbol::true_sym);
                }
                else if(scan->have(symbol::false_sym)) {
                    scan->must_be(symbol::false_sym);
                }
                else throw lille_exception("Invalid argument for `if`");
            }
            else if(not if_cond->tipe().is_type(lille_type::type_boolean)) {
                throw lille_exception("Invalid argument for `if`");
            }        
        }
        else if(scan->have(symbol::true_sym)) {
            scan->must_be(symbol::true_sym);
        }
        else if(scan->have(symbol::false_sym)) {
            scan->must_be(symbol::false_sym);
        }
        else throw lille_exception("Invalid argument for `if`");

        if(scan->have(symbol::and_sym)) {
                scan->must_be(symbol::and_sym);
                and_or_flag = true;
        }
        else if(scan->have(symbol::or_sym)) {
            scan->must_be(symbol::or_sym);
            and_or_flag = true;
        }
        else and_or_flag = false;
    }
    while(and_or_flag);
}

lille_type parser::get_type() {
    if(scan->have(symbol::integer))
        return lille_type::type_integer;
    else if(scan->have(symbol::real_num))
        return lille_type::type_real;
    else if(scan->have(symbol::strng))
        return lille_type::type_string;
    else if(scan->have(symbol::true_sym) or scan->have(symbol::false_sym))
        return lille_type::type_boolean;
    else return lille_type::type_unknown;
}

list<token*> parser::IDENT_LIST() {
    // create an array of tokens to store all variables and their names
        list<token*> variables;

        bool comma_flag = false;
        
        do {
            // For each identifier found ->
            if(scan->have(symbol::identifier)) {
                // Add the new variable into the array
                variables.push_back(new token(new symbol(symbol::identifier), 0, 0));
                // Assign the name to the token
                variables.back()->set_identifier_value(scan->get_current_identifier_name());
                scan->must_be(symbol::identifier);
            }
            // If there are more variables ->
            comma_flag = scan->have(symbol::comma_sym);
            if(comma_flag)
                scan->must_be(symbol::comma_sym);
        }
        while(comma_flag);

    return variables;
}

void parser::PARAM() {
    
    id_table_entry* id;
    scan->must_be(symbol::left_paren_sym);
        table->enter_scope();
        bool semi_flag = false;
        // Find all paramters ->
        do {
            // If a new parameter is found ->
            if(scan->have(symbol::identifier)) {
                symbol* sym = new symbol(symbol::identifier);
                token* ident = new token(sym, 0, 0);
                ident->set_identifier_value(scan->get_current_identifier_name());
                scan->must_be(symbol::identifier);
                scan->must_be(symbol::colon_sym);
                // Get the kind of param (ref or value)
                int k = scan->this_token()->get_symbol()->get_sym();
                lille_kind knd = lille_kind::unknown;
                switch(k) {
                    case symbol::ref_sym: {
                        scan->must_be(symbol::ref_sym);
                        knd = lille_kind::ref_param;
                        break;
                    }
                    case symbol::value_sym: {
                        scan->must_be(symbol::value_sym);
                        knd = lille_kind::value_param;
                        break;
                    }
                }
                // Create the entry for the parameter
                lille_type ty;
    
                
                if (scan->have(symbol::integer_sym)) {
                    ty = lille_type::type_integer;
                    scan->must_be(symbol::integer_sym);
                }
                else if (scan->have(symbol::real_sym)) {
                    ty = lille_type::type_real;
                    scan->must_be(symbol::real_sym);
                }
                else if(scan->have(symbol::string_sym)) {
                    ty = lille_type::type_string;
                    scan->must_be(symbol::string_sym);
                }
                else {
                    ty = lille_type::type_boolean;
                    scan->must_be(symbol::boolean_sym);
                }

                id = table->enter_id(ident, ty, knd, table->scope(), 0, lille_type::type_unknown);
            }
            // add the entry to the table
            table->add_table_entry(id);
            // link the parameter to the procedure
            current_fun_or_proc->add_param(id);

            // If a semi-colon is found, keep going
            semi_flag = scan->have(symbol::semicolon_sym);
            if(semi_flag)
                scan->must_be(symbol::semicolon_sym);
        }
        while(semi_flag);
        scan->must_be(symbol::right_paren_sym);
}