/*
 * id_table_entry.cpp
 *
 *  Created on: Sept 27, 2020
 *      Author: Ethan Shealey
 */

#include <iostream>
#include "id_table.h"
#include "id_table_entry.h"

id_table_entry::id_table_entry() {
    id_entry = NULL;
    lev_entry = 0;
    offset_entry = 0;
    kind_entry = lille_kind::unknown;
    trace_entry = false;
    typ_entry = lille_type::type_unknown;
    i_val_entry = 0;
    r_val_entry = 0.0;
    s_val_entry = "";
    b_val_entry = false;
    p_list_entry = NULL;
    n_par_entry = 0;
    r_ty_entry = lille_type::type_unknown;
}

id_table_entry::id_table_entry(token* id, lille_type typ, lille_kind kind, int level, int offset, lille_type return_tipe) {
    id_entry = id;
    typ_entry = typ;
    kind_entry = kind;
    lev_entry = level;
    offset_entry = offset;
    r_ty_entry = return_tipe;

    p_list_entry = NULL;
    n_par_entry = 0;
    lev_entry = 0;
    offset_entry = 0;
    trace_entry = false;
    i_val_entry = 0;
    r_val_entry = 0.0;
    s_val_entry = "";
    b_val_entry = false;
}

int id_table_entry::offset() {
    return offset_entry;
}

int id_table_entry::level() {
    return lev_entry;
}

lille_kind id_table_entry::kind() {
    return kind_entry;
}

lille_type id_table_entry::tipe() {
    return typ_entry;
}

token* id_table_entry::token_value() {
    return id_entry;
}

string id_table_entry::name() {
    if(id_entry->get_sym() == symbol::identifier)
        return id_entry->get_identifier_value();
    else if(id_entry->get_sym() == symbol::program_sym)
        return id_entry->get_prog_value();
    else if(id_entry->get_sym() == symbol::procedure_sym)
        return id_entry->get_proc_value();
    else if(id_entry->get_sym() == symbol::function_sym)
        return id_entry->get_fun_value();
    return "";
}

int id_table_entry::integer_value() {
    return i_val_entry;
}

float id_table_entry::real_value() {
    return r_val_entry;
}

string id_table_entry::string_value() {
    return s_val_entry;
}

bool id_table_entry::bool_value() {
    return b_val_entry;
}

lille_type id_table_entry::return_tipe() {
    return r_ty_entry;
}

void id_table_entry::add_param(id_table_entry* param_entry) {
    bool finished = false;
    id_table_entry* ptr = this;
    while(not finished) {
        if(ptr->p_list_entry == NULL) {
            ptr->p_list_entry = param_entry;
            finished = true;
            this->n_par_entry++;
            cout << "LINKED PARAM: Linked " << param_entry->name() << " to " << this->name() << endl;
        }
        else
            ptr = ptr->p_list_entry;
    }

}

void id_table_entry::fix_const(int integer_value, float real_value, string string_value, bool bool_value) {
    i_val_entry = integer_value;
    r_val_entry = real_value;
    s_val_entry = string_value;
    b_val_entry = bool_value;
}

void id_table_entry::fix_return_type(lille_type ret_ty) {
    this->r_ty_entry = ret_ty;
}

id_table_entry* id_table_entry::nth_parameter(int n) {
    id_table_entry* ptr = this; 
    for(int i = 0; i < n+1; i++) {
        if(ptr->p_list_entry != NULL) {
            ptr = ptr->p_list_entry;
        }
    }
    if(ptr == this)
        return NULL;
    return ptr;
}

int id_table_entry::number_of_params() {
    return n_par_entry;
}

bool id_table_entry::trace() {
    return trace_entry;
}

string id_table_entry::to_string() {
    return this->name() + ":\n" + "Type: " + this->tipe().to_string() + "\nKind: " + this->kind().to_string() + "\nScope: " + ::to_string(this->level()) + "\nReturn Type: " + this->r_ty_entry.to_string();
}