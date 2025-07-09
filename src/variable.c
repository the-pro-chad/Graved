/*
 * Variable.c : Archivo encargado del manejo, declaración y aceso a
 * las variables.
 */
#include "../include/variable.h"
#include "../include/error.h"

int char_is_start(const char *cadena, char caracter) {
    if (cadena == NULL || cadena[0] == '\0') return 0;
    return cadena[0] == caracter;
}

void set_variable(const char* name, const char* type, void* value) {
    Variable* var = get_variable(name);
    if (var != NULL) {
       if (var->is_const) const_var(name);
    }
    if (var == NULL) {
        if (variable_count >= MAX_VARIABLES) show_errors ("Too many variables",ERROR_INTERPRETER);
        var = &variables[variable_count++];
        if(char_is_start(name,'_')) var->is_const = true;
        strcpy(var->name, name);
    }
    strcpy(var->type, type);
    if (strcmp(type, "int") == 0) var->value.int_value = *(int*)value;
    else if (strcmp(type, "float") == 0) var->value.float_value = *(double*)value;
    else if (strcmp(type, "string") == 0) strcpy(var->value.string_value, (char*)value);
    else if (strcmp(type, "char") == 0) var->value.char_value = (char)value;
    else if (strcmp(type, "bool") == 0) var->value.bool_value = *(bool *)value;
    else var->value.int_value = 0;
}

Variable * get_variable(const char* name) {
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0) return &variables[i];
    }
    return NULL;
}

bool is_int (const char * name){
   if (!is_variable(name)) indefined_var(name);
   Variable * variable = get_variable(name);
   if (strcmp(variable->type, "int") != 0) return false;
   return true;
}

bool is_string (const char * name){
   if (!is_variable(name)) indefined_var(name);
   Variable * variable = get_variable(name);
   if (strcmp(variable->type, "string") != 0) return false;
   return true;
}

bool is_float (const char * name){
   if (!is_variable(name)) indefined_var(name);
   Variable * variable = get_variable(name);
   if (strcmp(variable->type, "float") != 0) return false;
   return true;
}

bool is_bool (const char * name){
   if (!is_variable(name)) indefined_var(name);
   Variable * variable = get_variable(name);
   if (strcmp(variable->type, "bool") != 0) return false;
   return true;
}

bool is_char (const char * name){
   if (!is_variable(name)) indefined_var(name);
   Variable * variable = get_variable(name);
   if (strcmp(variable->type, "char") != 0) return false;
   return true;
}

char * get_type_variable (const char * name){
  Variable * var = get_variable(name);
  if (!var) return "(empty)";
  return var->type;
}

void * get_value_variable (const char * name) {
  Variable * var = get_variable(name);
  if (!var) return NULL;
  if (strcmp(var->type, "int") == 0) return (void *)var->value.int_value;
  else if (strcmp(var->type, "float") == 0) return &var->value.float_value;
  else if (strcmp(var->type, "char") == 0) return (void *)var->value.char_value;
  else if (strcmp(var->type, "string") == 0) return (void *)var->value.string_value;
  else if (strcmp(var->type, "bool") == 0) return (void *)var->value.bool_value;
  else if (strcmp(var->type, "empty") == 0) return (void *)"(empty)";
  else return NULL;
}

int get_variable_int (const char * name){
   Variable * var = get_variable(name);
   if (!var) indefined_var(name);
   if (strcmp(var->type, "int") != 0) notEvaluateVariable(name, var->type, ERROR);
   return var->value.int_value;
}

float get_variable_float (const char * name){
   Variable * var = get_variable(name);
   if (!var) indefined_var(name);
   if (strcmp(var->type, "float") != 0) notEvaluateVariable(name, var->type, ERROR);
   return var->value.float_value;
}

char get_variable_char (const char * name){
   Variable * var = get_variable(name);
   if (!var) indefined_var(name);
   if (strcmp(var->type, "char") != 0) notEvaluateVariable(name, var->type, ERROR);
   return var->value.char_value;
}

char * get_variable_string (const char * name){
   Variable * var = get_variable(name);
   if (!var) indefined_var(name);
   if (strcmp(var->type, "string") != 0) notEvaluateVariable(name, var->type, ERROR);
   return var->value.string_value;
}

bool get_variable_bool (const char * name){
   Variable * var = get_variable(name);
   if (!var) indefined_var(name);
   if (strcmp(var->type, "bool") != 0) notEvaluateVariable(name, var->type, ERROR);
   return var->value.bool_value;
}

