/*
 * idtable.cpp
 *
 *  Created on: Jun 18, 2020
 *      Author: Michael Oudshoorn
 */

#include <iostream>
#include <string>

#include "token.h"
#include "error_handler.h"
#include "id_table.h"
#include "id_table_entry.h"
#include "lille_kind.h"
#include "lille_type.h"

using namespace std;

id_table::id_table(error_handler* err)
{
    error = err;
	debug_mode = true;
	scope_level = 0;

	for(int i = 0; i < max_depth; i++)
		sym_table[i] = NULL;
}


void id_table::dump_id_table(bool dump_all)
{
	if (!dump_all)
	{
		cout << "Dump of idtable for current scope only." << endl;
		cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
		
        // INSERT CODE HERE
	}
	else
	{
		cout << "Dump of the entire symbol table." << endl;
		cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
		
        // INSERT CODE HERE
	}
}

void id_table::enter_scope() {
	scope_level++;
}

void id_table::exit_scope() {
	scope_level--;
}

int id_table::scope() {
	return scope_level;
}

void id_table::add_table_entry(id_table_entry* id) {
	node* ptr = sym_table[scope()];

	bool match = false;

	if(ptr == NULL) {
		ptr = new node;
		ptr->idt = id;
		ptr->right = NULL;
		ptr->left = NULL;
		match = true;
	}
	while(not match) {
		else if(id->name() < ptr->idt->name()) {
			if(ptr->left != NULL)
				ptr = ptr->left;
			else
				match = true;
		}
		else if(id->name() >= ptr->idt->name()) {
			if(ptr->right != NULL)
				ptr = ptr->right;
			else
				match = true;
		} 
	}

	ptr = new node;
	ptr->idt = id;
	ptr->right = NULL;
	ptr->left = NULL;
}

id_table_entry* id_table::enter_id(token* id, lille_type typ, lille_kind kind, int level, int offset, lille_type return_tipe) {
	return new id_table_entry(id, typ, kind, level, offset, return_tipe);
}