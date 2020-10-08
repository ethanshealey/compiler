/*
 * id_table_entry.cpp
 *
 *  Created on: Sept 27, 2020
 *      Author: Ethan Shealey
 */

#include <iostream>
#include "id_table_entry.h"

id_table_entry::id_table_entry() {

}

id_table_entry::id_table_entry(token* id, lille_type typ, lille_kind kind, int level, int offset, lille_type return_tipe) {
    id_entry = id;
    typ_entry = typ;
    kind_entry = kind;
    lev_entry = level;
    offset_entry = offset;
    r_ty_entry = return_tipe;
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
    return id_entry->get_identifier_value();
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
   
}

