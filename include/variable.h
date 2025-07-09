/*
 * Variable.h
 */
#ifndef _VARIABLE_H
#define _VARIABLE_H

#include "interpret.h"
#include "error.h"
#include <stdbool.h>

char * get_type_variable (const char * name);
void set_variable(const char* name, const char* type, void* value);
Variable * get_variable(const char* name);
bool is_int (const char * name);
bool is_string (const char * name);
bool is_float (const char * name);
bool is_bool (const char * name);
bool is_char (const char * name);
char * get_type_variable (const char * name);
void * get_value_variable (const char * name);
int get_variable_int (const char * name);
float get_variable_float (const char * name);
char get_variable_char (const char * name);
char * get_variable_string (const char * name);
bool get_variable_bool (const char * name);

#endif

