#ifndef ARRAY_H
#define ARRAY_H

#include "interpret.h"
#include "variable.h"

void define_array();
void define_array_();
Array * get_array (const char * name);
void * acess_array ();
void * methods_array (const char * method, const char * array_name);

#endif

