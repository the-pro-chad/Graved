/*
 * array.c: Archivo encargado de manejar los
 * arreglos en el interpreter
 */
#include "../include/array.h"
#include "../include/error.h"

Array * get_array (const char * name) {
  for (int i = 0; i < array_count; i++) {
        if (strcmp(arrays[i].name, name) == 0) return &arrays[i];
  }
  return NULL;
}
//array:arr?int(10)
void define_array_ (){
   int length_array = 0;
   char * param = NULL;
   void * value_init = NULL;
   if (array_count >= MAX_ARRAY_SIZE) show_errors("Too many arrays", ERROR);
   Array * array = &arrays[array_count];
   if (!verify_token("COLON")) show_errors("Expected ':' in array declaration", ERROR);
   current_token++;
   if (!verify_token("IDENTIFIER")) show_errors("Expected a name im array declaration", ERROR);
   strcpy(array->name, get_token_value());
   current_token++;
   if (!verify_token("INTERROGATION")) show_errors("Expected '?' in array declaration", ERROR);
   current_token++;
   if (verify_token("STRING_PARAM")) param = "string";
   else if (verify_token("INTEGER_PARAM")) param = "int";
   else if (verify_token("FLOAT_PARAM")) param = "float";
   else if (verify_token("CHAR_PARAM")) param = "char";
   else if (verify_token("BOOL_PARAM")) param = "bool";
   else show_errors("Invalid type in array declaration", ERROR);
   strcpy(array->type, param);
   current_token++;
   expected("(", "array", "L_PAREN");
   if (!verify_token("NUMBER")) show_errors("Expected a number in size array", ERROR);
   length_array = get_int_token_value();
   current_token++;
   expected(")", "array", "R_PAREN");
   for (int i = 0; i < length_array; i++){
      strcpy(array->members[i].name, atos(i));
      strcpy(array->members[i].type, param);
      if (strcmp(param, "string") == 0) strcpy(array->members[i].value.string_value, "");
      else if (strcmp(param, "int") == 0) array->members[i].value.int_value = -1;
      else if (strcmp(param, "float") == 0) array->members[i].value.float_value = 1;
      else if (strcmp(param, "char") == 0) array->members[i].value.char_value = 'a';
      else if (strcmp(param, "bool") == 0) array->members[i].value.bool_value = false;
      array->member_count++;
   }
   array_count++;
}
//create_array(arr, 10, int)
void define_array (){
   int max_elements;
   char * param;
   if (array_count >= MAX_ARRAY_SIZE) show_errors("Too many arrays", ERROR);
   Array * arr = &arrays[array_count];
   expected("(","create_array","L_PAREN");
   if (verify_token("STRING")){
      char str[MAX_TOKEN_LENGTH];
      removeQuotes(get_token_value(), str);
      strcpy(arr->name, str);
   } else if (verify_token("IDENTIFIER")){
      if (is_variable(get_token_value())) show_errors("Variable is defined", ERROR);
      strcpy(arr->name, get_token_value());
   } else simple_error_msg("Invalid name for array",ERROR);
   current_token++;
   expected(",","create_array","COMMA");
   if (verify_token("NUMBER")) max_elements = get_int_token_value();
   else if (verify_token("IDENTIFIER")){
      if (is_variable(get_token_value())) max_elements = get_variable_int(get_token_value());
      else show_errors("Expected a 'int' value in size_array", ERROR_INTERPRETER);
      current_token++;
      expected(",","create_array","COMMA");
      if (verify_token("STRING_PARAM")) param = "string";
      else if (verify_token("INTEGER_PARAM")) param = "int";
      else if (verify_token("FLOAT_PARAM")) param = "float";
      else if (verify_token("CHAR_PARAM")) param = "char";
      else if (verify_token("BOOL_PARAM")) param = "bool";
      else show_errors("Invalid type param", ERROR);
      strcpy(arr->type,param);
      for (int i = 0; i < max_elements; i++){
         strcpy(arr->members[i].name, atos(i));
	 strcpy(arr->members[i].type, param);
	 if (strcmp(param,"string") == 0) strcpy(arr->members[i].value.string_value, "(empty)");
	 else if (strcmp(param,"int") == 0) arr->members[i].value.int_value = -1;
	 else if (strcmp(param,"float") == 0) arr->members[i].value.float_value = 0;
	 else if (strcmp(param,"char") == 0) arr->members[i].value.char_value = 'a';
	 else if (strcmp(param,"bool") == 0) arr->members[i].value.bool_value = false;
	 arr->member_count++;
      }
      current_token++;
      expected(")","create_array","R_PAREN");
      array_count++;
   }
}
//Manejar el aceso de los valores en los arreglos
void * acess_array (){
   current_token--;
   char array_name[MAX_TOKEN_LENGTH];
   int index_acess;
   int concidence = 0;
   strcpy(array_name,get_token_value());
   current_token += 2;
   if (verify_token("NUMBER")) index_acess = get_int_token_value();
   else if (verify_token("IDENTIFIER")) {
      if (is_variable(get_token_value())) index_acess = get_variable_int(get_token_value());
      else indefined_id (get_token_value());
   }
   void * value;
   if (!is_array(array_name)) indefined_id (array_name);
   Array * array = get_array(array_name);
   if (index_acess < 0) index_negative_array (atos(index_acess), array->member_count);
   if (index_acess >= array->member_count) inassecible_array(array_name, atos(index_acess), array->member_count);
   for (int i = 0; i < array->member_count; i++){
      if (strcmp(array->members[i].name, atos(index_acess)) == 0){
         if (strcmp(array->type, "int") == 0) {
            concidence = i;
            value = &array->members[i].value.int_value;
            type_value_return = "int";
            break;
         } else if (strcmp(array->members[i].type, "float") == 0) {
            concidence = i;
            value = &array->members[i].value.float_value;
            type_value_return = "float";
            break;
         } else if (strcmp(array->members[i].type, "string") == 0) {
            concidence = i;
            value = &array->members[i].value.string_value;
            type_value_return = "string";
            break;
         } else if (strcmp(array->members[i].type, "char") == 0) {
            concidence = i;
            value = &array->members[i].value.char_value;
            type_value_return = "char";
            break;
         } else if (strcmp(array->members[i].type, "bool") == 0) {
            concidence = i;
            value = &array->members[i].value.bool_value;
            type_value_return = "bool";
            break;
         } else notEvaluateVariable(array->name, array->type, ERROR);
      }
   }
   current_token++;
   expected("]", "array", "BRACKET");
   //array[0] = 10
   //Si a = array[0] = 67 entonces 'a' vale 10 no 67
   if (verify_token("OPERATOR") && verify_token_value("=")){
      current_token++;
      //Tipo int
      if (strcmp(array->type, "int") == 0) {
         if (verify_token("IDENTIFIER")) {
            if (is_variable(get_token_value())){
               array->members[concidence].value.int_value = get_variable_int(get_token_value());
               return value;
            }
         } else if (verify_token("NUMBER")) {
            array->members[concidence].value.int_value = get_int_token_value();
            return value;
         } else {
               void * expression = evaluate_expression();
               if (strcmp(type_value_return, "int") == 0){
                  array->members[concidence].value.int_value = *(int *)expression;
                  return value;
               } else show_errors("Invalid type in array of type int", ERROR);
         }
      } else if (strcmp(array->type, "float") == 0) {//Float
         if (verify_token("IDENTIFIER")) {
            if (is_variable(get_token_value())){
               array->members[concidence].value.float_value = get_variable_float(get_token_value());
               return value;
            } else indefined_var(get_token_value());
         } else if (verify_token("FLOAT")) {
            array->members[concidence].value.float_value = get_float_token_value();
            return value;
         } else {
               void * expression = evaluate_expression();
               if (strcmp(type_value_return, "float") == 0){
                  array->members[concidence].value.float_value = *(double *)expression;
                  return value;
               } else show_errors("Invalid value in array of type float", ERROR);
            }
      } else if (strcmp(array->type, "char") == 0) {//CHAR
         if (verify_token("IDENTIFIER")) {
            if (is_variable(get_token_value())){
               array->members[concidence].value.char_value = get_variable_char(get_token_value());
               return value;
            } else indefined_var(get_token_value());
         } else if (verify_token("CHAR")) {
            array->members[concidence].value.int_value = get_char_token_value();
            return value;
         } else show_errors("Invalid value in array type char", ERROR);
      } else if (strcmp(array->type, "string") == 0) {
         if (verify_token("IDENTIFIER")) {
            if (is_variable(get_token_value())){
               strcpy(array->members[concidence].value.string_value, get_variable_string(get_token_value()));
               return value;
            } else indefined_var(get_token_value());
         } else if (verify_token("STRING")) {
            char str[MAX_TOKEN_LENGTH];
            removeQuotes(get_token_value(), str);
            strcpy(array->members[concidence].value.string_value, get_token_value());
            return value;
         } else show_errors("Invalid value in array type string", ERROR);
      } else if (strcmp(array->type, "bool") == 0) {
         if (verify_token("IDENTIFIER")) {
            if (is_variable(get_token_value())){
               array->members[concidence].value.bool_value = get_variable_bool(get_token_value());
               return value;
            } else indefined_var(get_token_value());
         } else if (verify_token("TRUE") || verify_token("FALSE")) {
            array->members[concidence].value.bool_value = get_bool_token_value();
            return value;
         } else show_errors("Invalid value in array type bool", ERROR);
      } else show_errors("Type array no defined", ERROR);
    }
    return value;
}
//Funciones que seran llamadas al usarse '.'
//en el nombre de los arreglos
static bool pop (const char * array_name){
   Array * array = get_array(array_name);
   if (array->member_count < 1) invalidUseInLengthArray(array_name, "pop");
   free(&array->members[array->member_count]);
   array->member_count--;
   return true;
}
//invalidUseInLengthArray(array_name, "");
static int count (const char * array_name){
   Array * array = get_array(array_name);
   return array->member_count;
}
//funciones include
static bool include_int (const char * array_name, int value){
   Array * array = get_array(array_name);
   if (array->member_count < 1) invalidUseInLengthArray(array_name, "include");
   if (strcmp(array->type, "int") != 0) notEvaluateVariable(array_name, array->type, ERROR);
   for (int i = 0; i < array->member_count; i++){
      if (array->members[i].value.int_value == value) return true;
   }
   return false;
}

static bool include_float (const char * array_name, float value){
   Array * array = get_array(array_name);
   if (array->member_count < 1) invalidUseInLengthArray(array_name, "include");
   if (strcmp(array->type, "float") != 0) notEvaluateVariable(array_name, array->type, ERROR);
   for (int i = 0; i < array->member_count; i++){
      if (array->members[i].value.float_value == value) return true;
   }
   return false;
}

static bool include_string (const char * array_name, const char * value){
   Array * array = get_array(array_name);
   if (array->member_count < 1) invalidUseInLengthArray(array_name, "include");
   if (strcmp(array->type, "string") != 0) notEvaluateVariable(array_name, array->type, ERROR);
   for (int i = 0; i < array->member_count; i++){
      if (strcmp(array->members[i].value.string_value, value) == 0) return true;
   }
   return false;
}

static bool include_char (const char * array_name, char value){
   Array * array = get_array(array_name);
   if (array->member_count < 1) invalidUseInLengthArray(array_name, "include");
   if (strcmp(array->type, "char") != 0) notEvaluateVariable(array_name, array->type, ERROR);
   for (int i = 0; i < array->member_count; i++){
      if (array->members[i].value.char_value == value) return true;
   }
   return false;
}

static bool include_bool (const char * array_name, bool value){
   Array * array = get_array(array_name);
   if (array->member_count < 1) invalidUseInLengthArray(array_name, "include");
   if (strcmp(array->type, "int") != 0) notEvaluateVariable(array_name, array->type, ERROR);
   for (int i = 0; i < array->member_count; i++){
      if (array->members[i].value.bool_value == value) return true;
   }
   return false;
}

static char * type_array (const char * array_name){
   Array * array = get_array(array_name);
   return array->type;
}
//funciones fill
static void fill_int (const char * array_name, int value){
   Array * array = get_array(array_name);
   if (array->member_count < 1) invalidUseInLengthArray(array_name, "fill");
   if (strcmp(array->type, "int") == 0) {
      for(int i = 0; i < array->member_count; i++) array->members[i].value.int_value = value;
   } else notEvaluateVariable(array_name, array->type, ERROR); 
}

static void fill_float (const char * array_name, float value){
   Array * array = get_array(array_name);
   if (array->member_count < 1) invalidUseInLengthArray(array_name, "fill");
   if (strcmp(array->type, "float") == 0) {
      for(int i = 0; i < array->member_count; i++) array->members[i].value.float_value = value;
   } else notEvaluateVariable(array_name, array->type, ERROR);
}

static void fill_string (const char * array_name, const char * value){
   Array * array = get_array(array_name);
   if (array->member_count < 1) invalidUseInLengthArray(array_name, "fill");
   if (strcmp(array->type, "string") == 0) {
      for(int i = 0; i < array->member_count; i++) strcpy(array->members[i].value.string_value, value);
   } else notEvaluateVariable(array_name, array->type, ERROR);
}

static void fill_char (const char * array_name, char value){
   Array * array = get_array(array_name);
   if (array->member_count < 1) invalidUseInLengthArray(array_name, "fill");
   if (strcmp(array->type, "int") == 0) {
      for(int i = 0; i < array->member_count; i++) array->members[i].value.char_value = value;
   } else notEvaluateVariable(array_name, array->type, ERROR);
}

static void fill_bool (const char * array_name, bool value){
   Array * array = get_array(array_name);
   if (array->member_count < 1) invalidUseInLengthArray(array_name, "fill");
   if (strcmp(array->type, "int") == 0) {
      for(int i = 0; i < array->member_count; i++) array->members[i].value.bool_value = value;
   } else notEvaluateVariable(array_name, array->type, ERROR);
}

void * methods_array (const char * method, const char * array_name){
   if (strcmp(method, "pop") == 0) {
      expected("(", "pop", "L_PAREN");
      expected(")", "pop", "R_PAREN");
      type_value_return = "empty";
      return (void *)pop(array_name);
   } else if (strcmp(method, "include") == 0) {
      expected("(", "include", "L_PAREN");
      if (verify_token("NUMBER")) {
         int value = get_int_token_value();
         current_token++;
         expected(")", "include", "R_PAREN");
         type_value_return = "bool";
         return (void *)include_int(array_name, value);
      }
      else if (verify_token("FLOAT")) {
         float value = get_float_token_value();
         current_token++;
         expected(")", "include", "R_PAREN");
         type_value_return = "bool";
         return (void *)include_float(array_name, value);
      }
      else if (verify_token("STRING")) {
         char * value = get_token_value();
         current_token++;
         expected(")", "include", "R_PAREN");
         type_value_return = "bool";
         return (void *)include_string(array_name, value);
      }
      else if (verify_token("CHAR")) {
         char value = get_char_token_value();
         current_token++;
         expected(")", "include", "R_PAREN");
         type_value_return = "bool";
         return (void *)include_char(array_name, value);
      }
      else if (verify_token("TRUE")) {
         int value = get_int_token_value();
         current_token++;
         expected(")", "include", "R_PAREN");
         type_value_return = "bool";
         return (void *)include_int(array_name, value);
      }
      else if (verify_token("FALSE")) {
         int value = get_int_token_value();
         current_token++;
         expected(")", "include", "R_PAREN");
         type_value_return = "bool";
         return (void *)include_int(array_name, value);
      }
      else if (verify_token("IDENTIFIER")) {
         if (is_variable(get_token_value())) {
            char * variable_name = get_token_value();
            if (is_int(variable_name)){
               int value_int = get_variable_int(variable_name);
               current_token++;
               expected(")", "include", "R_PAREN");
               type_value_return = "bool";
               return (void *)include_int(array_name, value_int);
            } else if (is_string(variable_name)) {
               char value_string[MAX_TOKEN_LENGTH];
               strcpy(value_string, get_variable_string(variable_name));
               current_token++;
               expected(")", "include", "R_PAREN");
               type_value_return = "bool";
               return (void *)include_string(array_name, value_string);
            } else if (is_float(variable_name)) {
               //Здесь!!!!
               float value_float = get_variable_float(variable_name);
               current_token++;
               expected(")", "include", "R_PAREN");
               type_value_return = "bool";
               return (void *)include_float(array_name, value_float);
            } else if (is_char(variable_name)) {
               char value_char = get_variable_char(get_token_value());
               current_token++;
               expected(")", "include", "R_PAREN");
               type_value_return = "bool";
               return (void *)include_char(array_name, value_char);
            } else if (is_bool(variable_name)) {
               bool value_bool = get_variable_bool(get_token_value());
               current_token++;
               expected(")", "include", "R_PAREN");
               return (void *)include_bool(array_name, value_bool);
            } else notEvaluateVariable(get_token_value(), "empty", ERROR);
         } else indefined_id(get_token_value());
      } else indefined_id(get_token_value());
   } else if (strcmp(method, "count") == 0) {
      expected("(", "count", "L_PAREN");
      expected(")", "count", "R_PAREN");
      type_value_return = "int";
      return (void *)count(array_name);
   } else if (strcmp(method, "type") == 0) {
      expected("(", "type", "L_PAREN");
      expected(")", "type", "R_PAREN");
      type_value_return = "string";
      return (void *)type_array(array_name);
   } else if (strcmp(method, "fill") == 0) {
      char type_value[10];
      expected("(", "fill", "L_PAREN");
      if (verify_token("NUMBER")){
         current_token++;
         fill_int(array_name, get_int_token_value());
         type_value_return = "empty";
         expected(")", "fill", "R_PAREN");
         return (void *)"(empty)";
      } else if (verify_token("FLOAT")){
         current_token++;
         fill_float(array_name, get_float_token_value());
         type_value_return = "empty";
         expected(")", "fill", "R_PAREN");
         return (void *)"(empty)";
      } else if (verify_token("STRING")){
         char str[MAX_TOKEN_LENGTH];
         removeQuotes(get_token_value(), str);
         current_token++;
         fill_string(array_name, str);
         type_value_return = "empty";
         expected(")", "fill", "R_PAREN");
         return (void *)"(empty)";
      } else if (verify_token("CHAR")){
         current_token++;
         fill_char(array_name, get_char_token_value());
         type_value_return = "empty";
         expected(")", "fill", "R_PAREN");
         return (void *)"(empty)";
      } else if (verify_token("IDENTIFIER")){
         if (is_variable(get_token_value())){
            if (is_int(get_token_value())){
               current_token++;
               fill_int(array_name, get_variable_int(get_token_value()));
               type_value_return = "empty";
               expected(")", "fill", "R_PAREN");
               return (void *)"(empty)";
            } else if (is_float(get_token_value())){
               current_token++;
               fill_float(array_name, get_variable_float(get_token_value()));
               type_value_return = "empty";
               expected(")", "fill", "R_PAREN");
               return (void *)"(empty)";
            } else if (is_string(get_token_value())){
               current_token++;
               fill_string(array_name, get_variable_string(get_token_value()));
               type_value_return = "empty";
               expected(")", "fill", "R_PAREN");
               return (void *)"(empty)";
            } else if (is_char(get_token_value())){
               current_token++;
               fill_char(array_name, get_variable_char(get_token_value()));
               type_value_return = "empty";
               expected(")", "fill", "R_PAREN");
               return (void *)"(empty)";
            } else if (is_bool(get_token_value())){
               current_token++;
               fill_bool(array_name, get_variable_bool(get_token_value()));
               type_value_return = "empty";
               expected(")", "fill", "R_PAREN");
               return (void *)"(empty)";
            }
         }
      }
      else {
      }
   }
   else undefined_member(method, "array", ERROR);
}

