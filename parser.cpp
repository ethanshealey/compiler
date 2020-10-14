/*
 * parser.cpp
 *
 *  Created on: Sept 16, 2020
 *      Author: Ethan Shealey
 */

#include <iostream>
#include <algorithm>
#include <string>  
#include "symbol.h"
#include "scanner.h"
#include "token.h"
#include "parser.h"
#include "lille_type.h"
#include "lille_kind.h"
#include "id_table.h"
#include "id_table_entry.h"

using namespace std;

parser::parser(scanner* s, id_table* t, error_handler* e) {
    scan=s;
    table=t;
    error=e;

    current_entry = NULL;
    current_fun_or_proc = NULL;
    current_ident = NULL;
}
parser::~parser() {
    delete scan;
    scan = NULL;

    delete table;
    table = NULL;
}

void parser::define_function(string name, lille_type t, lille_type p) {

    // Create variables 
    token* fun, * arg;
    symbol* sym;
    id_table_entry* fun_id, * param_id;
    // Generate the entry
    sym = new symbol(symbol::identifier);
    fun = new token(sym, 0, 0);
    fun->set_identifier_value(name);
    fun_id = table->enter_id(fun, lille_type::type_func, lille_kind::unknown, 0, 0, t);
    table->add_table_entry(fun_id);
    // Generate the Arguments

    arg = new token(sym, 0, 0);
    arg->set_identifier_value("__" + name + "_arg__");
    param_id = new id_table_entry(arg, p, lille_kind::value_param, 0, 0, lille_type::type_unknown);
    fun_id->add_param(param_id);
}

void parser::PROG() { // Begin program
    scan->must_be(symbol::program_sym); 

    symbol* sym = new symbol(symbol::program_sym);
    token* prog = new token(sym, 0, 0);
    prog->set_prog_value(scan->get_current_identifier_name());
    id_table_entry* prog_id = table->enter_id(prog, lille_type::type_prog, lille_kind::unknown, table->scope(), 0, lille_type::type_unknown);
    table->add_table_entry(prog_id);
    current_entry = prog_id;

    // Define the predifined functions
    parser::define_function("INT2REAL", lille_type::type_real, lille_type::type_integer);
    parser::define_function("REAL2INT", lille_type::type_integer, lille_type::type_real);
    parser::define_function("INT2STRING", lille_type::type_string, lille_type::type_integer);
    parser::define_function("REAL2STRING", lille_type::type_string, lille_type::type_real);
    table->enter_scope();

    sym = new symbol(symbol::identifier);
    token* ident = new token(sym, 0, 0);
    ident->set_identifier_value(scan->get_current_identifier_name());
    id_table_entry* ident_entry = table->enter_id(ident, lille_type::type_proc, lille_kind::unknown, 0, 0, lille_type::type_unknown);
    table->add_table_entry(ident_entry);
    scan->must_be(symbol::identifier);

    scan->must_be(symbol::is_sym);
    BLOCK();
    scan->must_be(symbol::semicolon_sym);
}

void parser::BLOCK() {
    table->enter_scope();
    while (IS_DECLERATION()) 
         DECLERATION();
    scan->must_be(symbol::begin_sym);
    STATEMENT_LIST();
    scan->must_be(symbol::end_sym);
    if (scan->have(symbol::identifier)) {
        IDENT();
    }
}

void parser::DECLERATION() { 

    if (scan->have(symbol::identifier)) {
        int x = 0;
        token* variables[15];

        for(int i = 0; i < 15; i++) 
            variables[i] = NULL;

        bool comma_flag = false;
        
        do {
            if(scan->have(symbol::identifier)) {
                variables[x] = new token(new symbol(symbol::identifier), 0, 0);
                variables[x++]->set_identifier_value(scan->get_current_identifier_name());
                scan->must_be(symbol::identifier);
            }
            comma_flag = scan->have(symbol::comma_sym);
            if(comma_flag)
                scan->must_be(symbol::comma_sym);
        }
        while(comma_flag);

        scan->must_be(symbol::colon_sym);

        bool const_flag = false;
        lille_kind knd = lille_kind(lille_kind::variable);

        if(scan->have(symbol::constant_sym)) {
            const_flag = true;
            scan->must_be(symbol::constant_sym);
            knd = lille_kind(lille_kind::constant);
        }

        lille_type ty = get_ident_type();
        TYPE();

        float r_value;
        int i_value;
        string s_value;
        bool b_value; 

        if(const_flag) {
            scan->must_be(symbol::becomes_sym);
            int sym = scan->this_token()->get_symbol()->get_sym();
            switch(sym) {
                case symbol::real_num: {
                    r_value = scan->this_token()->get_real_value();
                    if (!ty.is_type(lille_type::type_real))
                        error->flag(scan->this_token(), 111);
                        // Const expr does not match type declaration.
                    break;
                }
                case symbol::integer: {
                    i_value = scan->this_token()->get_integer_value();
                    if (!ty.is_type(lille_type::type_integer))
                        error->flag(scan->this_token(), 111);
                        // Const expr does not match type declaration.
                    break;
                }
                case symbol::strng: {
                    s_value = scan->this_token()->get_string_value();
                    if (!ty.is_type(lille_type::type_string))
                        error->flag(scan->this_token(), 111);
                        // Const expr does not match type declaration.
                    break;
                }
                case symbol::boolean_sym: {
                    b_value = true ? scan->have(symbol::true_sym) : false;
                    if (!ty.is_type(lille_type::type_boolean))
                        error->flag(scan->this_token(), 111);
                        // Const expr does not match type declaration.
                    break;
                }
            }
            scan->get_token();
        }

        id_table_entry* id;
        for(token* &v : variables) {
            if(v != NULL) {
                id = table->enter_id(v, ty, knd, table->scope(), 0, lille_type::type_unknown);
                if(const_flag) {
                    id->fix_const(i_value, r_value, s_value, b_value);
                }
                table->add_table_entry(id);
                current_entry->add_param(id);
            }
        }
        scan->must_be(symbol::semicolon_sym);
    }

    else {
        id_table_entry* id;
        bool is_func = false;
        if(scan->have(symbol::procedure_sym)) {
            scan->must_be(symbol::procedure_sym);

            symbol* sym = new symbol(symbol::procedure_sym);
            token* proc = new token(sym, 0, 0);
            proc->set_proc_value(scan->get_current_identifier_name());
            id_table_entry* proc_id = table->enter_id(proc, lille_type::type_proc, lille_kind::unknown, table->scope(), 0, lille_type::type_unknown);
            table->add_table_entry(proc_id);
            current_fun_or_proc = proc_id;

            scan->must_be(symbol::identifier);
        }
        else {
            scan->must_be(symbol::function_sym);

            symbol* sym = new symbol(symbol::function_sym);
            token* fun = new token(sym, 0, 0);
            fun->set_fun_value(scan->get_current_identifier_name());
            id_table_entry* fun_id = table->enter_id(fun, lille_type::type_func, lille_kind::unknown, table->scope(), 0, lille_type::type_unknown);
            table->add_table_entry(fun_id);
            current_fun_or_proc = fun_id;

            scan->must_be(symbol::identifier);
            is_func = true;
        }
        scan->must_be(symbol::left_paren_sym);
        table->enter_scope();
        bool semi_flag = false;
        do {
            if(scan->have(symbol::identifier)) {
                symbol* sym = new symbol(symbol::identifier);
                token* ident = new token(sym, 0, 0);
                ident->set_identifier_value(scan->get_current_identifier_name());
                scan->must_be(symbol::identifier);
                scan->must_be(symbol::colon_sym);
                int ty = scan->this_token()->get_symbol()->get_sym();
                switch(ty) {
                    case symbol::ref_sym: {
                        scan->must_be(symbol::ref_sym);
                        id = table->enter_id(ident, get_ident_type(), lille_kind::ref_param, table->scope(), 0, lille_type::type_unknown);
                        TYPE();
                        break;
                    }
                    case symbol::value_sym: {
                        scan->must_be(symbol::value_sym);
                        id = table->enter_id(ident, get_ident_type(), lille_kind::value_param, table->scope(), 0, lille_type::type_unknown);
                        TYPE();
                        break;
                    }
                }
                
            }
            table->add_table_entry(id);
            current_fun_or_proc->add_param(id);

            semi_flag = scan->have(symbol::semicolon_sym);
            if(semi_flag)
                scan->must_be(symbol::semicolon_sym);
        }
        while(semi_flag);
        scan->must_be(symbol::right_paren_sym);

        if(is_func) {
            scan->must_be(symbol::return_sym);
            current_fun_or_proc->fix_return_type(get_ident_type());
            TYPE();
        }

        
        scan->must_be(symbol::is_sym);
        BLOCK();
        scan->must_be(symbol::semicolon_sym);
        current_entry = NULL;
        current_fun_or_proc = NULL;
        table->exit_scope();
    }
}

void parser::STATEMENT_LIST() {
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
        string current_entry_name = scan->get_current_identifier_name();
        current_entry = table->lookup(current_entry_name);
        if(current_entry == NULL) {
            throw lille_exception("Use of undeclared identifier " + current_entry_name);
        }
        scan->must_be(symbol::identifier);
        
        if (scan->have(symbol::left_paren_sym)) {
            scan->must_be(symbol::left_paren_sym);
            handle_function_or_procedure_call(current_entry);
        } 
        else {
            scan->must_be(symbol::becomes_sym);
            /**** HANDLE BECOMES ****/
            bool finished = true;
            do {
                if(scan->have(symbol::left_paren_sym)) {
                    scan->must_be(symbol::left_paren_sym);
                    finished = false;
                }
                else if(scan->have(symbol::right_paren_sym)) {
                    scan->must_be(symbol::right_paren_sym);
                    finished = false;
                }
                else if(scan->have(symbol::not_sym)) {
                    if(not current_entry->tipe().is_equal(lille_type::type_boolean))
                        throw lille_exception("Cannot use operator 'not' in a non-boolean ");
                    scan->must_be(symbol::not_sym);
                    finished = false;
                }
                else if(scan->have(symbol::identifier)) {
                    current_ident = table->lookup(scan->get_current_identifier_name());
                    if(not current_entry->tipe().is_equal(current_ident->tipe()))
                        throw lille_exception("Identifier Given Does Not Match Any Function Calls For " + current_entry_name);
                    scan->must_be(symbol::identifier);

                    /**** HANDLE FUNCTION/PROCEDURE CALL ****/
                    if(scan->have(symbol::left_paren_sym)) {
                        scan->must_be(symbol::left_paren_sym);
                        if(current_ident->tipe().is_type(lille_type::type_func) or current_ident->tipe().is_type(lille_type::type_proc))
                            handle_function_or_procedure_call(current_ident);
                        else
                            throw lille_exception(current_ident->name() + " is not a function or procedure.");
                    }
                    finished = false;
                }
                else if(IS_NUMBER()) {
                    if(not current_entry->tipe().is_equal(get_ident_type())) 
                        throw lille_exception("Value given does not match the type of " + current_entry_name);
                    if(scan->have(symbol::integer)) {
                        scan->must_be(symbol::integer);
                    }
                    else if(scan->have(symbol::real_num)) {
                        scan->must_be(symbol::real_num);
                    }
                    finished = false;
                }
                else if(IS_ADDOP()) {
                    ADDOP();
                    if(not current_entry->tipe().is_equal(lille_type::type_integer) and not current_entry->tipe().is_equal(lille_type::type_real))
                        throw lille_exception("Cannot add or subtract non-numbers");
                    finished = false;
                }
                else if(IS_MULTOP()) {
                    MULTOP();
                    if(not current_entry->tipe().is_equal(lille_type::type_integer) and not current_entry->tipe().is_equal(lille_type::type_real))
                        throw lille_exception("Cannot multiply or divide non-numbers");
                    finished = false;
                }
                else if(IS_RELOP()) {
                    RELOP();
                    if(not current_entry->tipe().is_equal(lille_type::type_boolean))
                        throw lille_exception("Cannot compare non-boolean values");
                    finished = false;
                }
                else if(IS_BOOL()) {
                    if(not current_entry->tipe().is_equal(lille_type::type_boolean))
                        throw lille_exception("Cannot assign boolean values to a non boolean identifier");
                    finished = false;
                }
                else {
                    finished = true;
                }
                
            }
            while(not finished);
        }
    } 
    else if (scan->have(symbol::exit_sym)) {
        scan->must_be(symbol::exit_sym);
        if (scan->have(symbol::when_sym)) {
            scan->must_be(symbol::when_sym);
            current_ident = table->lookup(scan->get_current_identifier_name());
            if(not current_ident->tipe().is_type(lille_type::type_integer))
                throw lille_exception("Exit condition must be of Integer type");
            scan->must_be(symbol::identifier);
            RELOP();
            if(scan->have(symbol::identifier)) {
                current_ident = table->lookup(scan->get_current_identifier_name());
                if(not current_ident->tipe().is_type(lille_type::type_integer))
                    throw lille_exception("Exit condition must be of Integer type");
                scan->must_be(symbol::identifier);
            }
            else {
                scan->must_be(symbol::integer);
            }
        }
    } 
    else if (scan->have(symbol::return_sym)) {
        scan->must_be(symbol::return_sym);
        if (IS_EXPR()) {
            if(scan->have(symbol::identifier)) {
                id_table_entry* current_return_ident = table->lookup(scan->get_current_identifier_name());
                if(current_fun_or_proc == NULL)
                    throw lille_exception("Return in non-function");
                else if(not current_fun_or_proc->tipe().is_equal(current_return_ident->tipe()))
                        throw lille_exception("Function return value is " + current_fun_or_proc->tipe().to_string() + " but given " + current_return_ident->tipe().to_string());
                scan->must_be(symbol::identifier);
            }
            else if(IS_NUMBER()) {
                if(scan->have(symbol::integer)) {
                    if(not current_fun_or_proc->tipe().is_type(lille_type::type_integer))
                        throw lille_exception("Function return value is " + current_fun_or_proc->tipe().to_string() + " but given Integer");
                    scan->must_be(symbol::integer);
                }
                if(scan->have(symbol::real_num)) {
                    if(not current_fun_or_proc->tipe().is_type(lille_type::type_integer))
                        throw lille_exception("Function return value is " + current_fun_or_proc->tipe().to_string() + " but given Real Number");
                    scan->must_be(symbol::real_num);
                }
            }
            else if(IS_BOOL()) {
                if(scan->have(symbol::true_sym)) 
                    scan->must_be(symbol::true_sym);
                else if(scan->have(symbol::false_sym)) 
                    scan->must_be(symbol::false_sym);
                if(not current_fun_or_proc->tipe().is_type(lille_type::type_integer))
                        throw lille_exception("Function return value is " + current_fun_or_proc->tipe().to_string() + " but given Boolean");
                    
            }
            else if(scan->have(symbol::strng)) {
                if(not current_fun_or_proc->tipe().is_type(lille_type::type_integer))
                        throw lille_exception("Function return value is " + current_fun_or_proc->tipe().to_string() + " but given String");
                scan->must_be(symbol::strng);
            }
            else throw lille_exception("Function return is " + current_fun_or_proc->tipe().to_string() + " but given Unknown");
        }
    } 
    else if (scan->have(symbol::read_sym)) {
        scan->must_be(symbol::read_sym);
        bool lp = false, comma_sym = false;
        if (scan->have(symbol::left_paren_sym)) {
            scan->must_be(symbol::left_paren_sym);
            lp = true;
        }
        do {
            current_ident = table->lookup(scan->get_current_identifier_name());
            if(current_ident == NULL)
                throw lille_exception("Undeclared Variable " + scan->get_current_identifier_name());
            scan->must_be(symbol::identifier);
            if(scan->have(symbol::comma_sym)) {
                comma_sym = true;
                scan->must_be(symbol::comma_sym);
            }
            else comma_sym = false;
        }
        while (comma_sym);

        if(lp) {
            scan->must_be(symbol::right_paren_sym);
        }
    } 
    else if (scan->have(symbol::write_sym)) {
        scan->must_be(symbol::write_sym);
        bool lp = false, amp_sym = false;
        if (scan->have(symbol::left_paren_sym)) {
            scan->must_be(symbol::left_paren_sym);
            lp = true;
        }
        do {
            if(scan->have(symbol::identifier)) {
                current_ident = table->lookup(scan->get_current_identifier_name());
                if(not current_ident->tipe().is_type(lille_type::type_string)) 
                    if(not (current_ident->name() == "INT2STRING" or current_ident->name() == "REAL2STRING" or current_ident->return_tipe().is_type(lille_type::type_string)))
                        throw lille_exception("Function write only takes strings as arguments");
                scan->must_be(symbol::identifier);
                if(scan->have(symbol::left_paren_sym)) {
                    scan->must_be(symbol::left_paren_sym);
                    handle_function_or_procedure_call(current_ident);
                }
            }
            else if(scan->have(symbol::strng)) {
                scan->must_be(symbol::strng);
            }
            if(scan->have(symbol::ampersand_sym)) {
                scan->must_be(symbol::ampersand_sym);
                amp_sym = true;
            }   
            else amp_sym = false;
        }
        while(amp_sym);
        if (lp) {
            scan->must_be(symbol::right_paren_sym);
        } 
    }
    else if (scan->have(symbol::writeln_sym)) {
        scan->must_be(symbol::writeln_sym);
        bool lp = false, amp_sym = false;
        if (scan->have(symbol::left_paren_sym)) {
            scan->must_be(symbol::left_paren_sym);
            lp = true;
        }
        do {
            if(scan->have(symbol::identifier)) {
                current_ident = table->lookup(scan->get_current_identifier_name());
                if(not current_ident->tipe().is_type(lille_type::type_string)) 
                    if(not current_ident->return_tipe().is_type(lille_type::type_string))
                        throw lille_exception("Function writeln only takes strings as arguments");
                scan->must_be(symbol::identifier);
                if(scan->have(symbol::left_paren_sym)) {
                    scan->must_be(symbol::left_paren_sym);
                    handle_function_or_procedure_call(current_ident);
                }
            }
            else if(scan->have(symbol::strng)) {
                scan->must_be(symbol::strng);
            }
            if(scan->have(symbol::ampersand_sym)) {
                scan->must_be(symbol::ampersand_sym);
                amp_sym = true;
            }   
            else amp_sym = false;
        }
        while(amp_sym);
        if (lp) {
            scan->must_be(symbol::right_paren_sym);
        }
    } 
    else {
        scan->must_be(symbol::null_sym);
    }
    current_entry = NULL;
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
    scan->must_be(symbol::integer);
    scan->must_be(symbol::range_sym);
    scan->must_be(symbol::integer);
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
    else  if (scan->have(symbol::greater_or_equal_sym))
        scan->must_be(symbol::greater_or_equal_sym);
    else
        throw lille_exception("Expected a logical symbol (> < <> = >= <=)");
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

void parser::IDENT() {
    // Generate the entry
    symbol* sym = new symbol(symbol::identifier);
    token* ident = new token(sym, 0, 0);
    ident->set_identifier_value(scan->get_current_identifier_name());
    id_table_entry* ident_entry = table->enter_id(ident, lille_type::type_integer, lille_kind::unknown, 0, 0, lille_type::type_unknown);
    table->add_table_entry(ident_entry);

    scan->must_be(symbol::identifier);
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
    if (scan->have(symbol::not_sym) or scan->have(symbol::odd_sym) or scan->have(symbol::left_paren_sym) or scan->have(symbol::identifier) or IS_NUMBER() or scan->have(symbol::strng) or IS_BOOL() or IS_ADDOP() or IS_NUMBER()) 
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
    string current_entry_name = current_entry->name();
    if(current_entry->tipe().is_equal(lille_type::type_func)) {
                /**** HANDEL FUNCTION CALL ****/
                for(int i = 0; i < current_entry->number_of_params(); i++) {
                    if(scan->have(symbol::identifier)) {
                        current_ident = table->lookup(scan->get_current_identifier_name());
                        if(not current_entry->nth_parameter(i)->tipe().is_equal(current_ident->tipe()))
                            throw lille_exception("Identifier Given Does Not Match Match Parameter Type in " + current_entry_name);
                        scan->must_be(symbol::identifier);
                    }
                    if(scan->have(symbol::comma_sym))
                        scan->must_be(symbol::comma_sym);
                    else {
                        if(not current_entry->nth_parameter(i)->tipe().is_equal(get_ident_type())) 
                            throw lille_exception("Value Given Does Not Match Match Parameter Type in " + current_entry_name);
                    }
                }

            }
            else if(current_entry->tipe().is_equal(lille_type::type_proc)) {
                /**** HANDEL PROCEDURE CALL ****/
                for(int i = 0; i < current_entry->number_of_params(); i++) {
                    if(scan->have(symbol::identifier)) {
                        current_ident = table->lookup(scan->get_current_identifier_name());
                        if(not current_entry->nth_parameter(i)->tipe().is_equal(current_ident->tipe()))
                            throw lille_exception("Identifier Given Does Not Match Parameter Type in " + current_entry_name);
                        scan->must_be(symbol::identifier);
                    }
                    if(scan->have(symbol::comma_sym))
                        scan->must_be(symbol::comma_sym);
                    else {
                        if(not current_entry->nth_parameter(i)->tipe().is_equal(get_ident_type())) 
                            throw lille_exception("Value Given Does Not Match Parameter Type in " + current_entry_name);
                    }
                }
            }
            else throw lille_exception("Unkown Function/Procedure " + current_entry_name);

            scan->must_be(symbol::right_paren_sym);
}