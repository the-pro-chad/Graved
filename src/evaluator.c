/*
 * Evaluator.c: encargado de evaluar las expresiones
 * del interprete.
 */
#include "../include/interpret.h"
#include "../include/error.h"
#include "../include/array.h"
#include "../include/variable.h"
#include "../include/enum.h"
#include "../include/protocol.h"
#include "../include/properties.h"
#include "../include/object.h"
#include "../include/function.h"
#include "../include/str.h"
#include <ncurses.h>

void * evaluate_term(){
  void * left = evaluate_factor();
  int l = *(int *)left;
  while (current_token < token_count && strcmp(tokens[current_token].type, "OPERATOR") == 0 && (tokens[current_token].value[0] == '*' || tokens[current_token].value[0] == '/')){
    char op = tokens[current_token].value[0];
    current_token++;
    void * right = evaluate_factor();
    int r = *(int *)right;
    if (op == '*') l *= r;
    else{
      if (right == 0)
        simple_error_msg("Division by zero",ERROR);
      l /= r;
    }
  }
  return left;
}

void * evaluate_expression(){
  if (current_token + 2 < token_count &&
                strcmp(tokens[current_token].type, "IDENTIFIER") == 0 &&
                strcmp(tokens[current_token + 1].type, "OPERATOR") == 0 &&
                tokens[current_token + 1].value[0] == '='){
    char var_name[MAX_TOKEN_LENGTH];
    strcpy(var_name, tokens[current_token].value);
    current_token += 2;
    char var_str[MAX_TOKEN_LENGTH * 2];
    if (verify_token("NUMBER")){
      int value_int = get_int_token_value();
      set_variable(var_name, "int", &value_int);
    } else if (strcmp(tokens[current_token].type, "STRING") == 0){
      removeQuotes(get_token_value(), var_str);
      char * strings = string_format(var_str);
      if (strings == NULL || strcmp(strings, "") == 0) set_variable(var_name, "string", var_str);
      else set_variable(var_name, "string", strings);
    } else if (verify_token("CHAR")){
      int char_ = tokens[current_token].value[1];
      set_variable(var_name, "char", &char_);
    } else if (verify_token("FLOAT")){
      double float_v = atof(tokens[current_token].value);
      set_variable(var_name, "float", &float_v);
    } else if (verify_token("TRUE")){
      int true_v = 1;
      set_variable(var_name, "bool", &true_v);
    } else if (verify_token("FALSE")){
      int false_v = 0;
      set_variable(var_name, "bool", &false_v);
    } else if (verify_token("EMPTY"))
      set_variable(var_name, "empty", NULL);
    else{
       if (cstr(type_value_return, "int")) {
          int int_value = *(int *)evaluate_expression();
          set_variable(var_name, "int", &int_value);
       } else if (cstr(type_value_return, "float")) {
          float float_value = *(double *)evaluate_expression();
          set_variable(var_name, "float", &float_value);
       }
      void * returning_value = evaluate_expression();
      set_variable(var_name, type_value_return, &returning_value);
    }
    return evaluate_expression();
  }
  int left = *(int *)evaluate_term();
  while (current_token < token_count &&
      strcmp(tokens[current_token].type, "OPERATOR") == 0 &&
      (tokens[current_token].value[0] == '+'         || tokens[current_token].value[0] == '-' ||
      strcmp(tokens[current_token].value, "==") == 0 || strcmp(tokens[current_token].value, "<") == 0 ||
      strcmp(tokens[current_token].value, ">") == 0  || strcmp(tokens[current_token].value, "<=") == 0 ||
      strcmp(tokens[current_token].value, ">=") == 0 || strcmp(tokens[current_token].value, "!=") == 0)){
    char op[3];
    strcpy(op, tokens[current_token].value);
    current_token++;
    int right = *(int *)evaluate_term();
    if (strcmp(op, "+") == 0) left += right;
    else if (strcmp(op, "-")  == 0) left -= right;
    else if (strcmp(op, "*")  == 0) left *= right;
    else if (strcmp(op, "/")  == 0) left /= right;
    else if (strcmp(op, "==") == 0) left = (right == left);
    else if (strcmp(op, "<")  == 0) left = (left < right);
    else if (strcmp(op, ">")  == 0) left = (left > right);
    else if (strcmp(op, "<=") == 0) left = (left <= right);
    else if (strcmp(op, ">=") == 0) left = (left >= right);
    else if (strcmp(op, "!=") == 0) left = (left != right);
  }
  return &left;
}

void * evaluate_factor(){
   Token token = tokens[current_token];
   current_token++;
   if (strcmp(token.type, "NUMBER") == 0) return (void *)atoi(token.value);
   else if (strcmp(token.type, "FLOAT") == 0) {
      float float_value = atof(token.value);
      void * float_return = &float_value;
      return float_return;
   } else if (strcmp(token.type, "STRING") == 0){
      char str[MAX_TOKEN_LENGTH];
      removeQuotes(get_token_value(), str);
      char * result_str = string_format(str);
      if (result_str == NULL || strcmp(result_str,"") == 0) return (void *)result_str;
      return (void *)str;
   } else if (strcmp(token.type, "CHAR") == 0) return (void *)get_char_token_value();
   else if (strcmp(token.type, "TRUE") == 0)   return (void *)true;
   else if (strcmp(token.type, "FALSE") == 0)  return (void *)false;
   else if (strcmp(token.type, "EMPTY") == 0)  return NULL;
   else if (strcmp(token.type, "IDENTIFIER") == 0){
      if (current_token < token_count && verify_token("L_PAREN")){
            Function *func = get_function(token.value);
            if (func == NULL) notDefined(token.value, "function", ERROR_INTERPRETER);
            current_token++;
            void * args[MAX_PARAMS];
            int arg_count = 0;
            while (current_token < token_count && verify_token("L_PAREN")){
                args[arg_count++] = evaluate_expression();
                if (current_token < token_count && verify_token("COMMA")) current_token++;
            }
            if (current_token >= token_count || !verify_token("R_PAREN")){
                printf("Error: Expected ')' after function arguments\n");
                exit(ERROR);
            }
            current_token++;
            return call_function(func, &args, arg_count);
        } else if (current_token < token_count && strcmp(tokens[current_token].type, "BRACKET") == 0 && strcmp(tokens[current_token].value, "[") == 0){
            void * value = acess_array();
            return value;
        } else if (current_token < token_count && strcmp(tokens[current_token].type, "DOT") == 0){
            if (is_protocol(token.value)){
               Protocol * protocol = get_protocol(token.value);
                current_token++;
                if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER")) undefined_member(get_token_value(), "protocol", ERROR_INTERPRETER);
                char * member_name = tokens[current_token].value;
                if (protocol->type_protocol == 0) evalue_http_protocol(protocol, member_name);
                else show_errors("Undifined type protocol", ERROR_SINTAXIS);
            } else if (is_array(token.value)){
               Array * array = get_array(token.value);
               char * array_name = token.value;
               current_token++;
               current_token++;
               return methods_array(get_token_value(), array_name);
            } else if (is_object(token.value)){
               Object * object = get_object(token.value);
                current_token++;
                if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0) undefined_member(get_token_value(), "object", ERROR_INTERPRETER);
                char *member_name = tokens[current_token].value;
                current_token++;
                for (int i = 0; i < object->member_count; i++){
                    if (strcmp(object->members[i].name, member_name) == 0){
                        if (strcmp(object->members[i].type, "int") == 0) return &object->members[i].value.int_value;
                        else if (strcmp(object->members[i].type, "float") == 0) return &object->members[i].value.float_value;
                        else if (strcmp(object->members[i].type, "char") == 0) return &object->members[i].value.char_value;
                        else if (strcmp(object->members[i].type,"string") == 0) return &object->members[i].value.string_value;
                        else if (strcmp(object->members[i].type,"empty") == 0) return NULL;
                        else if (strcmp(object->members[i].type, "function") == 0){
                            Function * func = object->members[i].value.func_value;
                            void * args[MAX_PARAMS];
                            int arg_count = 0;
                            if (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") == 0 && strcmp(tokens[current_token].value, "(") == 0){
                                current_token++;
                                while (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") != 0){
                                    args[arg_count++] = evaluate_expression();
                                    if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0)
                                        current_token++;
                                }
                                if (current_token >= token_count || strcmp(tokens[current_token].value, ")") != 0){
                                    printf("Error: Expected ')' after function arguments\n");
                                    exit(ERROR);
                                }
                                current_token++;
                            }
                            return call_function(func, &args, arg_count);
                        } else notEvaluateStructure(member_name, "object", token.value, ERROR);
                    }
                }
                printf("Error: Member '%s' not found in object '%s'\n", member_name, token.value);
                exit(ERROR);
            } else if (is_enum(token.value)){
               Enum * enum_ = get_enum(token.value);
               current_token++;
               if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0){
                  printf("Error: Expected enum value after '.'\n");
                  exit(ERROR);
               }
               char *enum_value = get_token_value();
               current_token++;
               for (int i = 0; i < enum_->member_count; i++){
                  if (strcmp(enum_->members[i], enum_value) == 0) return &i;
                }
                printf("Error: Enum value '%s' not found in enum '%s'\n", enum_value, token.value);
                exit(ERROR);
            } else if (is_value(token.value)){
               Values * values = get_values(token.value);
                current_token++;
                if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0)
                   show_errors("Error: Expected member name after '.'\n", ERROR);
                char *member_name = get_token_value();
                current_token++;
                for (int i = 0; i < values->member_count; i++){
                    if (strcmp(values->members[i].name, member_name) == 0){
                        if (strcmp(values->members[i].type, "int") == 0)         return (void *)values->members[i].value.int_value;
                        else if (strcmp(values->members[i].type, "float") == 0)  return &values->members[i].value.float_value;
                        else if (strcmp(values->members[i].type, "char") == 0)   return (void *)values->members[i].value.char_value;
                        else if (strcmp(values->members[i].type, "string") == 0) return (void *)values->members[i].value.string_value;
                        else show_errors("Error: Cannot evaluate member '%s' of values\n", ERROR);
                    }
                }
                show_errors("Error: Member '' not found in values ''\n", ERROR);
            } else if (is_properties(token.value)){
               Properties * properties = get_properties(token.value);
                current_token++;
                if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0) show_errors("Error: Expected member name after '.'\n", ERROR);
                char * member_name = tokens[current_token].value;
                current_token++;
                for (int i = 0; i < properties->member_count; i++){
                    if (strcmp(properties->members[i].name, member_name) == 0){
                        if (properties->visible[i] == 1){
                           current_token++;
                            if (!verify_token("IDENTIFIER")){
                                if (verify_token("NUMBER")){
                                    strcpy(properties->members[i].type, "int");
                                    properties->members[i].value.int_value = get_int_token_value();
                                } else if (verify_token("FLOAT")){
                                    strcpy(properties->members[i].type, "float");
                                    properties->members[i].value.float_value = get_float_token_value();
                                } else if (verify_token("STRING")){
                                    strcpy(properties->members[i].type, "string");
                                    char str[MAX_TOKEN_LENGTH];
                                    removeQuotes(get_token_value(), str);
                                    strcpy(properties->members[i].value.string_value, str);
                                } else if (strcmp(tokens[current_token].type, "CHAR") == 0){
                                    strcpy(properties->members[i].type, "char");
                                    properties->members[i].value.char_value = get_char_token_value();
                                }
                            } if (current_token + 1 < token_count &&
                                strcmp(tokens[current_token].type, "IDENTIFIER") == 0 &&
                                strcmp(tokens[current_token + 1].type, "OPERATOR") == 0 &&
                                tokens[current_token + 1].value[0] == '='){
                            } if (strcmp(properties->members[i].type, "int") == 0) return (void *)properties->members[i].value.int_value;
                            else if (strcmp(properties->members[i].type, "float") == 0) return &properties->members[i].value.float_value;
                            else if (strcmp(properties->members[i].type, "char") == 0) return (void *)properties->members[i].value.char_value;
                            else if (strcmp(properties->members[i].type, "string") == 0) return (void *)properties->members[i].value.string_value;
                            else if (strcmp(properties->members[i].type, "bool") == 0) return (void *)properties->members[i].value.bool_value;
                            else if (strcmp(properties->members[i].type, "empty") == 0) return NULL;
                            else if (strcmp(properties->members[i].type, "function") == 0) {
                                Function * func = properties->members[i].value.func_value;
                                void * args[MAX_PARAMS];
                                int arg_count = 0;
                                if (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") == 0 && strcmp(tokens[current_token].value, "(") == 0) {
                                    current_token++;
                                    while (current_token < token_count && strcmp(tokens[current_token].type, "PAREN") != 0) {
                                        args[arg_count++] = evaluate_expression();
                                        if (current_token < token_count && strcmp(tokens[current_token].type, "COMMA") == 0)
                                            current_token++;
                                    }
                                    if (current_token >= token_count || strcmp(tokens[current_token].value, ")") != 0){
                                        printf("Error: Expected ')' after function arguments\n");
                                        exit(ERROR);
                                    }
                                    current_token++;
                                }
                                return call_function(func, &args, arg_count);
                            } else {
                                printf("Error: Cannot evaluate member '%s' of propeties '%s'\n", member_name, token.value);
                                exit(ERROR);
                            }
                        } else {
                            printf("Error: Cannot access at member '%s' of properties '%s' this is a 'priv'\n", member_name, token.value);
                            exit(ERROR);
                        }
                    }
                }
                printf("Error: Member '%s' not found in propeties'%s'\n", member_name, token.value);
                exit(ERROR);
            } else {
                printf("Error: '%s' is not a object\n", token.value);
                exit(ERROR);
            }
        }
        else {
            if (!is_variable(token.value)) invalid_var(token.value, ERROR);
            Variable * variable = get_variable(token.value);
            if (strcmp(variable->type, "int") == 0) return &variable->value.int_value;
            else if (strcmp(variable->type, "string") == 0) return &variable->value.string_value;
            else if (strcmp(variable->type, "char") == 0) return &variable->value.char_value;
            else if (strcmp(variable->type, "float") == 0) return &variable->value.float_value;
            else if (strcmp(token.type, "CHAR") == 0) return &variable->value.char_value;
            else if (strcmp(token.type, "STRING") == 0){
                char result_string[MAX_TOKEN_LENGTH];
                removeQuotes(token.value, result_string);
                return &result_string;
            } else {
                printf("Error: Cannot evaluate variable '%s' of type '%s'\n", token.value, variable->type);
                exit(ERROR);
            }
        }
    }
    else if (strcmp(token.type, "GET_INPUT") == 0) {
        void * value;
        expected("(", "getInput", "L_PAREN");
        if (strcmp(tokens[current_token].type, "STRING_PARAM") == 0){
            char str[MAX_TOKEN_LENGTH];
            scanf("%s", &str);
            current_token++;
            expected(")", "getInput", "R_PAREN");
            value = &str;
            type_value_return = "string";
        } else if (strcmp(tokens[current_token].type, "INTEGER_PARAM") == 0){
            int int_value;
            scanf("%i", &int_value);
            current_token++;
            expected(")", "getInput", "R_PAREN");
            value = &int_value;
            type_value_return = "int";
        } else if (strcmp(tokens[current_token].type, "FLOAT_PARAM") == 0){
            float float_value;
            scanf("%f", &float_value);
            current_token++;
            expected(")", "getInput", "R_PAREN");
            value = &float_value;
            type_value_return = "float";
        }
        else show_errors("Invalid type param in getInput", 1);
        return value;
    } else if(strcmp(token.type, "PUTS") == 0) {
        print();
        return 0;
    } else if(strcmp(token.type, "CAT_STR") == 0){
       type_value_return = "string";
       return (void *)str_cat();
    } else if (strcmp(token.type, "CAT") == 0) {
       current_token-=2;
       if (!verify_token("IDENTIFIER")) show_errors("Expected identifier en '.=' operator", ERROR);
       if (!is_variable(get_token_value())) invalid_var(get_token_value(), ERROR);
       Variable * id1 = get_variable(get_token_value());
       if (id1->is_const) const_var(get_token_value());
       if(strcmp(id1->type, "string") != 0) show_errors("Invalid type of variable, expected a string", ERROR);
       char var1[MAX_TOKEN_LENGTH];
       char result_str[MAX_TOKEN_LENGTH];
       strcpy(var1, get_token_value());
       current_token+=3;
       if (verify_token("IDENTIFIER")){
          if (!is_variable(get_token_value())) invalid_var(get_token_value(), ERROR);
          Variable * id2 = get_variable(get_token_value());
          if (id2->is_const) const_var(get_token_value());
          if(strcmp(id2->type, "int") == 0){
             snprintf(result_str, MAX_TOKEN_LENGTH, "%s%i", id1->value.string_value, id2->value.int_value);
             strcpy(id1->value.string_value, result_str);
          } else if(strcmp(id2->type, "float") == 0){
             snprintf(result_str, MAX_TOKEN_LENGTH, "%s%.2f", id1->value.string_value, id2->value.float_value);
             strcpy(id1->value.string_value, result_str);
          } else if(strcmp(id2->type, "char") == 0){
             snprintf(result_str, MAX_TOKEN_LENGTH, "%s%c", id1->value.string_value, id2->value.char_value);
             strcpy(id1->value.string_value, result_str);
          } else if(strcmp(id2->type, "string") == 0){
             snprintf(result_str, MAX_TOKEN_LENGTH, "%s%i", id1->value.string_value, id2->value.string_value);
             strcpy(id1->value.string_value, result_str);
          } else if(strcmp(id2->type, "empty") == 0){
             snprintf(result_str, MAX_TOKEN_LENGTH, "%s(empty)", id1->value.string_value);
             strcpy(id1->value.string_value, result_str);
          } else if(strcmp(id2->type, "bool") == 0){
             snprintf(result_str, MAX_TOKEN_LENGTH, "%s%s", id1->value.string_value, id2->value.int_value == 1 ? "true" : "false");
             strcpy(id1->value.string_value, result_str);
          } else invalid_var(get_token_value(), ERROR);
       } else if (verify_token("NUMBER")){
          snprintf(result_str, MAX_TOKEN_LENGTH, "%s%i", id1->value.string_value, get_int_token_value());
          strcpy(id1->value.string_value, result_str);
       } else if (verify_token("FLOAT")){
          snprintf(result_str, MAX_TOKEN_LENGTH, "%s%.2f", id1->value.string_value, get_float_token_value());
          strcpy(id1->value.string_value, result_str);
       } else if (verify_token("CHAR")){
          snprintf(result_str, MAX_TOKEN_LENGTH, "%s%c", id1->value.string_value, get_char_token_value());
          strcpy(id1->value.string_value, result_str);
       } else if (verify_token("STRING")){
          char str[MAX_TOKEN_LENGTH];
          removeQuotes(get_token_value(), str);
          snprintf(result_str, MAX_TOKEN_LENGTH, "%s%s", id1->value.string_value, str);
          strcpy(id1->value.string_value, result_str);
       } else if (verify_token("EMPTY")){
          snprintf(result_str, MAX_TOKEN_LENGTH, "%s(empty)", id1->value.string_value);
          strcpy(id1->value.string_value, result_str);
       } else if (verify_token("TRUE")){
          snprintf(result_str, MAX_TOKEN_LENGTH, "%s%s", id1->value.string_value, get_token_value());
          strcpy(id1->value.string_value, result_str);
       } else if (verify_token("FALSE")){
          snprintf(result_str, MAX_TOKEN_LENGTH, "%s%s", id1->value.string_value, get_token_value());
          strcpy(id1->value.string_value, result_str);
       } else show_errors("Expected token in '.=' operator", ERROR);
       current_token++;
    } else if (strcmp(token.type, "EXIT") == 0){
       int code;
       expected("(", "exit", "L_PAREN");
       if (!verify_token("NUMBER")) show_errors("Expected a number in exit function", ERROR);
       code = get_int_token_value();
       current_token++;
       expected(")", "exit", "R_PAREN");
       exit(code);
    } else if (strcmp(token.type, "INCREMENT") == 0) {
       current_token -= 2;
       if (verify_token("IDENTIFIER")) {
            char * var_name = get_token_value();
            current_token += 3;
            if (!is_variable(var_name)) indefined_var(var_name);
            Variable * var = get_variable(var_name);
            if(var->is_const) const_var(var_name);
            if (strcmp(var->type, "int") == 0) var->value.int_value++;
            else if (strcmp(var->type, "float") == 0) var->value.float_value++;
            else show_errors("Invalid type for variable in '++' operator", ERROR);
            return 0;
        }
        show_errors("Expected a indetifier in '++' operator", ERROR);
    } else if (strcmp(token.type, "DECREMENT") == 0) {
        current_token -= 2;
        if (verify_token("IDENTIFIER")) {
            char *var_name = tokens[current_token].value;
            current_token += 3;
            if (!is_variable(var_name)) indefined_var(var_name);
            Variable * var= get_variable(var_name);
            if (strcmp(var->type, "int") == 0) var->value.int_value--;
            else if (strcmp(var->type, "float") == 0) var->value.float_value--;
            else show_errors("Invalid type for variable in '--' operator", ERROR_SINTAXIS);
            return 0;
        }
        show_errors("Expected a idenrifier in '--' operator", ERROR);
    } else if (strcmp(token.type, "AUMM") == 0) {
        current_token -= 2;
        float old_value;
        char * var_name = get_token_value();
        if (!is_variable(get_token_value())) indefined_var(var_name);
        Variable * variable = get_variable(get_token_value());
        if (variable->is_const) const_var(get_token_value());
        if (strcmp(variable->type, "int") == 0) old_value = variable->value.int_value;
        else if (strcmp(variable->type, "float") == 0) old_value = variable->value.float_value;
        else notEvaluateVariable(variable->name, variable->type, ERROR);
        current_token += 3;
        if (verify_token("IDENTIFIER")){
            if (!is_variable(get_token_value())) indefined_var(get_token_value());
            Variable * var = get_variable(get_token_value());
            if (strcmp(var->type, "int") == 0 && strcmp(variable->type, "int") == 0) variable->value.int_value+=var->value.int_value;
            else if (strcmp(var->type, "float") == 0 && strcmp(variable->type, "float") == 0) variable->value.float_value+=var->value.float_value;
            else show_errors("Invalid type variable in '+=' operator", ERROR);
            return 0;
        } else if (verify_token("NUMBER")) {
           if (strcmp(variable->type, "int") == 0) variable->value.int_value+=get_int_token_value();
           else if (strcmp(variable->type, "float") == 0) variable->value.float_value+=get_int_token_value();
           else notEvaluateVariable(variable->name, variable->type, ERROR);
           return 0;
        } else if (verify_token("FLOAT")) {
           if (strcmp(variable->type, "int") == 0) variable->value.int_value+=get_float_token_value();
           else if (strcmp(variable->type, "float") == 0) variable->value.float_value+=get_float_token_value();
           else notEvaluateVariable (variable->name, variable->type, ERROR);
           return 0;
        } else show_errors("Invalid type in '+=' operator", ERROR);
   } else if (strcmp(token.type, "SUDD") == 0){
      current_token -= 2;
        float old_value;
        char * var_name = get_token_value();
        if (!is_variable(get_token_value())) indefined_var(var_name);
        Variable * variable = get_variable(get_token_value());
        if (variable->is_const) const_var(get_token_value());
        if (strcmp(variable->type, "int") == 0) old_value = variable->value.int_value;
        else if (strcmp(variable->type, "float") == 0) old_value = variable->value.float_value;
        else notEvaluateVariable(variable->name, variable->type, ERROR);
        current_token += 3;
        if (verify_token("IDENTIFIER")){
            if (!is_variable(get_token_value())) indefined_var(get_token_value());
            Variable * var = get_variable(get_token_value());
            if (strcmp(var->type, "int") == 0 && strcmp(variable->type, "int") == 0) variable->value.int_value-=var->value.int_value;
            else if (strcmp(var->type, "float") == 0 && strcmp(variable->type, "float") == 0) variable->value.float_value-=var->value.float_value;
            else show_errors("Invalid type variable in '-=' operator", ERROR);
            return 0;
        } else if (verify_token("NUMBER")) {
           if (strcmp(variable->type, "int") == 0) variable->value.int_value-=get_int_token_value();
           else if (strcmp(variable->type, "float") == 0) variable->value.float_value-=get_int_token_value();
           else notEvaluateVariable(variable->name, variable->type, ERROR);
           return 0;
        } else if (verify_token("FLOAT")) {
           if (strcmp(variable->type, "int") == 0) variable->value.int_value-=get_float_token_value();
           else if (strcmp(variable->type, "float") == 0) variable->value.float_value-=get_float_token_value();
           else notEvaluateVariable (variable->name, variable->type, ERROR);
           return 0;
        } else show_errors("Invalid type in '-=' operator", ERROR);
   } else if (strcmp(token.type, "MUL") == 0) {
      current_token -= 2;
        float old_value;
        char * var_name = get_token_value();
        if (!is_variable(get_token_value())) indefined_var(var_name);
        Variable * variable = get_variable(get_token_value());
        if (variable->is_const) const_var(get_token_value());
        if (strcmp(variable->type, "int") == 0) old_value = variable->value.int_value;
        else if (strcmp(variable->type, "float") == 0) old_value = variable->value.float_value;
        else notEvaluateVariable(variable->name, variable->type, ERROR);
        current_token += 3;
        if (verify_token("IDENTIFIER")){
            if (!is_variable(get_token_value())) indefined_var(get_token_value());
            Variable * var = get_variable(get_token_value());
            if (strcmp(var->type, "int") == 0 && strcmp(variable->type, "int") == 0) variable->value.int_value*=var->value.int_value;
            else if (strcmp(var->type, "float") == 0 && strcmp(variable->type, "float") == 0) variable->value.float_value*=var->value.float_value;
            else show_errors("Invalid type variable in '*=' operator", ERROR);
            return 0;
        } else if (verify_token("NUMBER")) {
           if (strcmp(variable->type, "int") == 0) variable->value.int_value*=get_int_token_value();
           else if (strcmp(variable->type, "float") == 0) variable->value.float_value*=get_int_token_value();
           else notEvaluateVariable(variable->name, variable->type, ERROR);
           return 0;
        } else if (verify_token("FLOAT")) {
           if (strcmp(variable->type, "int") == 0) variable->value.int_value*=get_float_token_value();
           else if (strcmp(variable->type, "float") == 0) variable->value.float_value*=get_float_token_value();
           else notEvaluateVariable (variable->name, variable->type, ERROR);
           return 0;
        } else show_errors("Invalid type in '*=' operator", ERROR);
   } else if (strcmp(token.type, "DIV") == 0) {
      current_token -= 2;
        float old_value;
        char * var_name = get_token_value();
        if (!is_variable(get_token_value())) indefined_var(var_name);
        Variable * variable = get_variable(get_token_value());
        if (variable->is_const) const_var(get_token_value());
        if (strcmp(variable->type, "int") == 0) old_value = variable->value.int_value;
        else if (strcmp(variable->type, "float") == 0) old_value = variable->value.float_value;
        else notEvaluateVariable(variable->name, variable->type, ERROR);
        current_token += 3;
        if (verify_token("IDENTIFIER")){
            if (!is_variable(get_token_value())) indefined_var(get_token_value());
            Variable * var = get_variable(get_token_value());
            if (strcmp(var->type, "int") == 0 && strcmp(variable->type, "int") == 0) variable->value.int_value/=var->value.int_value;
            else if (strcmp(var->type, "float") == 0 && strcmp(variable->type, "float") == 0) variable->value.float_value/=var->value.float_value;
            else show_errors("Invalid type variable in '/=' operator", ERROR);
            return 0;
        } else if (verify_token("NUMBER")) {
           if (strcmp(variable->type, "int") == 0) variable->value.int_value/=get_int_token_value();
           else if (strcmp(variable->type, "float") == 0) variable->value.float_value/=get_int_token_value();
           else notEvaluateVariable(variable->name, variable->type, ERROR);
           return 0;
        } else if (verify_token("FLOAT")) {
           if (strcmp(variable->type, "int") == 0) variable->value.int_value/=get_float_token_value();
           else if (strcmp(variable->type, "float") == 0) variable->value.float_value/=get_float_token_value();
           else notEvaluateVariable (variable->name, variable->type, ERROR);
           return 0;
        } else show_errors("Invalid type in '/=' operator", ERROR);
   } else if (strcmp(token.type, "ROW") == 0){
       current_token -= 2;
       char * new_var = NULL;
       if (verify_token("IDENTIFIER")){
          char * var_name = get_token_value();
          current_token+=3;
          if(!verify_token("IDENTIFIER")) show_errors("Expected a identifier in => operator", ERROR);
          new_var = get_token_value();
          if (is_variable(new_var)) show_errors("variable are declared. '=>' operator no create a new variable", ERROR);
          if (!is_variable(var_name)) indefined_var(var_name);
          Variable * var = get_variable(var_name);
          if (strcmp(var->type, "int") == 0) set_variable(new_var, var->type, &var->value.int_value);
          else if (strcmp(var->type, "float")  == 0) set_variable(new_var, var->type, &var->value.float_value);
          else if (strcmp(var->type, "string") == 0) set_variable(new_var, var->type, &var->value.string_value);
          else if (strcmp(var->type, "char")   == 0) set_variable(new_var, var->type, &var->value.char_value);
          else if (strcmp(var->type, "bool")   == 0) set_variable(new_var, var->type, &var->value.bool_value);
          else if (strcmp(var->type, "empty")  == 0) set_variable(new_var, var->type, "(empty)");
          else notEvaluateVariable(var->name, var->type, ERROR);
          return 0;
       }
       void * value = evaluate_expression();
       current_token+=3;
       if(!verify_token("IDENTIFIER")) show_errors("Expected a identifier in => operator", ERROR);
       new_var = get_token_value();
       if (is_variable(new_var)) show_errors("variable are declared. '=>' operator no create a new variable", ERROR);
       set_variable(new_var, type_value_return, &value);
       return 0;
    } else if (strcmp(token.type, "MKDIR") == 0) {
        char nameDir[MAX_TOKEN_LENGTH];
        int mode, result;
        expected("(", "mkdir", "L_PAREN");
        if (verify_token("STRING")){
          char tmp[MAX_TOKEN_LENGTH];
          removeQuotes(get_token_value(), tmp);
          strcpy(nameDir, tmp);
        } else if (verify_token("IDENTIFIER")){
          if (is_variable(get_token_value())){
            Variable * var = get_variable(get_token_value());
            if (strcmp(var->type, "string") != 0)
              show_errors("Expected a string in mkdir function", ERROR);
            strcpy(nameDir, var->value.string_value);
          } else invalid_var(get_token_value(), ERROR);
        } else show_errors("Expected a string in mkdir function", ERROR);
        current_token++;
        expected(",", "mkdir", "COMMA");
        if (verify_token("NUMBER")) mode = get_int_token_value();
        else if (verify_token("IDENTIFIER")){
          if(is_variable(get_token_value())){
            Variable * var = get_variable(get_token_value());
            if (strcmp(var->type, "int") != 0) show_errors("Expected a number in mkdir function", ERROR);
            mode = var->value.int_value;
          } else invalid_var(get_token_value(), ERROR);
        } else show_errors("Expected a number in mkdir function", ERROR);
        type_value_return = "bool";
        current_token++;
        expected(")", "mkdir", "R_PAREN");
        if (mkdir(nameDir, mode) == 0) return (void *)true;
        return (void *)false;
    }
    else if (strcmp(token.type, "EXISTS_DIR") == 0) {
        int r = 0;
        expected("(", "exists_dir", "L_PAREN");
        if (verify_token("STRING")) show_errors("Expected a string value in exists_dir function", ERROR_SINTAXIS);
        else {
            char result_string[MAX_TOKEN_LENGTH];
            removeQuotes(tokens[current_token].value, result_string);
            struct stat buffer;
            r = (stat(result_string, &buffer) == 0 && S_ISDIR(buffer.st_mode));
        }
        current_token++;
        expected(")", "exists_dir", "R_PAREN");
        type_value_return = "bool";
        return &r;
    } else if (strcmp(token.type, "PI") == 0){
      float pi = 3.1415;
      type_value_return = "float";
      return &pi;
    } else if (strcmp(token.type, "RANDOM") == 0){
      int va;
      expected("(", "random", "L_PAREN");
      if (verify_token("NUMBER")) va = get_int_token_value();
      else if (verify_token("IDENTIFIER")){
        if(is_variable(get_token_value())){
          Variable * v = get_variable(get_token_value());
          if (strcmp(v->type, "int") != 0) show_errors("Invalid argument for random function. Expected a number value", ERROR);
          va = v->value.int_value;
        } else invalid_var(get_token_value(), ERROR);
      } else show_errors("Invalid agument for random function. Expected a number value", ERROR);
      current_token++;
      type_value_return = "int";
      expected(")", "random", "R_PAREN");
      srand(time(NULL));
      int r = rand()%va;
      return &r;
    } else if (strcmp(token.type, "DREAM") == 0){
      int t;
      expected("(", "dream", "L_PAREN");
      if(verify_token("NUMBER")) t = get_int_token_value();
      else if(verify_token("IDENTIFIER")){
        if(is_variable(get_token_value())){
          Variable * var = get_variable(get_token_value());
          if(strcmp(var->type, "int") != 0) show_errors("Invalid type in dream function. Expected number", ERROR);
          t = var->value.int_value;
        } else invalid_var(get_token_value(), ERROR);
      } else show_errors("Invalid type in dream function. Expected number", ERROR);
      current_token++;
      expected(")", "dream", "R_PAREN");
      type_value_return = "empty";
      usleep(t * t);
      return NULL;
    } else if (strcmp(token.type, "SETCOLORTEXT") == 0){
      int color;
      expected("(", "setColorText", "L_PAREN");
      if(verify_token("NUMBER")) color = get_int_token_value();
      else if(verify_token("IDENTIFIER")){
        if(is_variable(get_token_value())){
          Variable * var = get_variable(get_token_value());
          if (strcmp(var->type, "int") != 0) show_errors("Expected number in setColorText", ERROR);
          color = var->value.int_value;
        } else invalid_var(get_token_value(), ERROR);
      } else show_errors("Expected number in setColorText", ERROR);
      current_token++;
      expected(")", "setColorText", "R_PAREN");
      type_value_return = "empty";
      switch (color) {
        case 0:
            printf("\033[30m");
            break;
        case 1:
            printf("\033[31m");
            break;
        case 2:
            printf("\033[32m");
            break;
        case 3:
            printf("\033[33m");
            break;
        case 4:
            printf("\033[34m");
            break;
        case 5:
            printf("\033[35m");
            break;
        case 6:
            printf("\033[36m");
            break;
        case 7:
            printf("\033[37m");
            break;
        case 8:
            printf("\033[38m");
            break;
        case 9:
            printf("\033[39m");
            break;
        case 10:
            printf("\033[40m");
            break;
        case 11:
            printf("\033[41m");
            break;
        case 12:
            printf("\033[42m");
            break;
        case 13:
            printf("\033[43m");
            break;
        case 14:
            printf("\033[44m");
            break;
        case 15:
            printf("\033[45m");
            break;
        case 16:
            printf("\033[46m");
            break;
        case 17:
            printf("\033[47m");
            break;
        case 18:
            printf("\033[48m");
            break;
        case 19:
            printf("\033[49m");
            break;
        default:
            printf("\033[31m");
        }
    } else if (strcmp(token.type, "GOTOXY") == 0){
        int x, y;
        expected("(", "gotoxy", "L_PAREN");
        if(verify_token("NUMBER")) x = get_int_token_value();
        else if(verify_token("IDENTIFIER")){
          if(is_variable(get_token_value())){
            Variable * var = get_variable(get_token_value());
            if (strcmp(var->type, "int") != 0) show_errors("Expected a number in gotoxy function", ERROR);
            x = var->value.int_value;
          } else invalid_var(get_token_value(), ERROR);
        } else show_errors("Expected a numer in gotoxy function", ERROR);
        current_token++;
        expected(",", "gotoxy", "COMMA");
        if(verify_token("NUMBER")) y = get_int_token_value();
        else if(verify_token("IDENTIFIER")){
          if(is_variable(get_token_value())){
            Variable * var = get_variable(get_token_value());
            if (strcmp(var->type, "int") != 0) show_errors("Expected a number in gotoxy function", ERROR);
            y = var->value.int_value;
          } else invalid_var(get_token_value(), ERROR);
         } else show_errors("Expected a numer in gotoxy function", ERROR);
        current_token++;
        expected(")", "gotoxy", "R_PAREN");
        printf("\033[%d;%dH", y + 1, x + 1);
        type_value_return = "empty";
        return 0;
    } else if (strcmp(token.type, "CLEAR") == 0){
        system("clear");
        type_value_return = "empty";
        return 0;
    } else if(strcmp(token.type, "GETCHAR") == 0){
      expected("(", "getchar", "L_PAREN");
      expected(")", "getchar", "R_PAREN");
      type_value_return = "char";
      char key = getch();
      return &key;
    } else if (strcmp(token.type, "REMOVE") == 0){
      char namefile[MAX_TOKEN_LENGTH];
      expected("(", "remove", "L_PAREN");
      if (verify_token("STRING")){
         char str[MAX_TOKEN_LENGTH];
         removeQuotes(get_token_value(), str);
         strcpy(namefile, str);
      } else if (verify_token("IDENTIFIER")){
         if (is_variable(get_token_value())){
            Variable * var = get_variable(get_token_value());
            if (strcmp(var->type, "string") != 0) notEvaluateVariable (var->name, var->type, ERROR);
            strcpy(namefile, var->value.string_value);
         } else notDefined(get_token_value(), get_token_type(), ERROR);
      } else show_errors("Expected a string value in 'remove' function", ERROR);
      current_token++;
      expected(")", "remove", "R_PAREN");
      type_value_return = "bool";
      if (remove(namefile) != -1) return (void *)true;
      return (void *)false;
    } else if (strcmp(token.type, "GETTYPE") == 0){
     char type[MAX_TOKEN_LENGTH];
     expected("(", "gettype", "L_PAREN");
     if (verify_token("IDENTIFIER")){
        if (is_variable(get_token_value())) strcpy(type, get_type_variable(get_token_value()));
        else indefined_var(get_token_value());
     } else show_errors("Expected a indentifier in gettype function", ERROR);
     current_token++;
     expected(")", "gettype", "R_PAREN");
     type_value_return = "string";
     return &type;
  } else {
      unexpected_token(token.value,0,ERROR);
  }
}
