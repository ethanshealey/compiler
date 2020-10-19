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

	for(int i = 0; i < max_depth; i++) {
		sym_table[i] = new node;
		sym_table[i]->idt = NULL;
		sym_table[i]->left = NULL;
		sym_table[i]->right = NULL;
	}

}


void id_table::dump_id_table(bool dump_all)
{
	node* ptr;
	if (!dump_all)
	{
		cout << "Dump of idtable for current scope only." << endl;
		cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

		ptr = sym_table[scope()];
		exit_scope();
		delete ptr;
		ptr = NULL;
	}
	else
	{
		cout << "Dump of the entire symbol table." << endl;
		cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
		
		
        while(scope() > 0) {
			ptr = sym_table[scope()];
			exit_scope();
			delete ptr;
			ptr = NULL;
		}
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
	node* entry = new node;
	entry->idt = id;
	entry->right = NULL;
	entry->left = NULL;

	node* x = sym_table[scope()], * y = NULL;

	bool found = false;
	while(x->idt != NULL) {
		y = x;
		if(id->name() < x->idt->name()) {
			x = x->left;
		}
		else {
			x = x->right;
		}
		if(x == NULL)
			break;
	}

	if(y == NULL)  
		sym_table[scope()] = entry;
	else if (id->name() < y->idt->name()) 
        y->left = entry;
	else
		y->right = entry;

	if(debug_mode)
		cout << "ADDED ENTRY: Created Entry " << id->name() << " in Scope " << scope() << endl;
}

id_table_entry* id_table::enter_id(token* id, lille_type typ, lille_kind kind, int level, int offset, lille_type return_tipe) {
	return new id_table_entry(id, typ, kind, level, offset, return_tipe);
}

id_table_entry* id_table::lookup(string s) {	
	int sc = scope();
	node* ptr = sym_table[sc];
	bool found = false;
	
	while(sc >= 0) {
		if(ptr == NULL or ptr->idt == NULL) {
			if(sc > 0)
				ptr = sym_table[--sc];	
			else
				sc--;
		}
		else if(s < ptr->idt->name()) {
			ptr = ptr->left;
		}
		else if(s > ptr->idt->name()) {
			ptr = ptr->right;
		}
		else if(s == ptr->idt->name()) {
			if(debug_mode)
				cout << "FOUND ENTRY: Found entry " << s << endl;
			return ptr->idt; 
		}
	}
	if(debug_mode)
		cout << "DID NOT FIND: Failed to find entry " << s << endl; 
	return NULL;
}

id_table_entry* id_table::lookup(token* tok) {	
	int sc = scope();
	node* ptr = sym_table[sc];
	bool found = false;
	
	while(sc >= 0) {
		if(ptr == NULL or ptr->idt == NULL) {
			if(sc > 0)
				ptr = sym_table[--sc];	
			else
				sc--;
		}
		else if(tok < ptr->idt->token_value()) {
			ptr = ptr->left;
		}
		else if(tok > ptr->idt->token_value()) {
			ptr = ptr->right;
		}
		else if(tok == ptr->idt->token_value()) {
			if(debug_mode)
				cout << "FOUND ENTRY: Found entry " << tok->to_string() << endl;
			return ptr->idt; 
		}
	}
	if(debug_mode)
		cout << "DID NOT FIND: Failed to find entry " << tok->to_string() << endl; 
	return NULL;
}
