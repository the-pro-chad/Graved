/*
 * str.c : Archivo encargado de trabajar con los strings
 * del interpreter.
 */
#include "../include/str.h"
#include "../include/error.h"
#include "../include/variable.h"
#include <stdbool.h>

typedef struct {
    char ** matches;
    size_t count;
} format_string;

static char * cat_stf (char * str1, char * str2){
  size_t len1 = strlen(str1);
  size_t len2 = strlen(str2);
  char * resultado = (char*)malloc(len1 + len2 + 1);
  if (!resultado) return NULL;
  strcpy(resultado, str1);
  strcat(resultado, str2);
  return resultado;
}

static void cat_plus (char *result, const char *format, ...){
    va_list args;
    va_start(args, format);
    va_list args_copy;
    va_copy(args_copy, args);
    int length = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);
    vsnprintf(result, length + 1, format, args);
    va_end(args);
}

char * str_cat (){
  current_token-=4;
    char * var = get_token_value();
    current_token+=2;
    char token1[MAX_TOKEN_LENGTH];
    char token2[MAX_TOKEN_LENGTH];
    char result_cat[MAX_TOKEN_LENGTH];
    if (verify_token("STRING")){
        char s[MAX_TOKEN_LENGTH];
        removeQuotes(get_token_value(), s);
        strcpy(token1,s);
    } else if (verify_token("NUMBER")) strcpy(token1,get_token_value());
    else if (verify_token("FLOAT"))    strcpy(token1,get_token_value());
    else if (verify_token("char"))     strcpy(token1,get_token_value());
    else show_errors("Invalid value", ERROR_SINTAXIS);
    current_token+=3;
    if (verify_token("STRING")){
        char s[MAX_TOKEN_LENGTH];
        removeQuotes(get_token_value(), s);
        strcpy(token2,s);
    } else if (verify_token("NUMBER")) strcpy(token2,get_token_value());
    else if (verify_token("FLOAT"))    strcpy(token2,get_token_value());
    else if (verify_token("char"))     strcpy(token2,get_token_value());
    else show_errors("Invalid value", ERROR_SINTAXIS);
    type_value_return = "string";
    current_token++;
    cat_plus(result_cat,"%s%s",token1, token2);
    set_variable(var, "string", &result_cat);
    return result_cat;
}

static bool verify_type_id (const char * str1, const char * str2){
  if (strcmp(str1, str2) == 0) return true;
  return false;
}

void print (){
  expected("(", "puts", "L_PAREN");
  if(verify_token("STRING")){
    char str[MAX_TOKEN_LENGTH];
    removeQuotes(get_token_value(), str);
    char * strings = string_format(str);
    if (strings == NULL || strcmp(strings, "") == 0) printf("%s",str);
    else printf("%s", strings);
    current_token++;
  } else if (verify_token("NUMBER")){
    printf("%s", get_token_value());
    current_token++;
  } else if (verify_token("FLOAT")){
    printf("%.2f", atof(get_token_value()));
    current_token++;
  } else if (verify_token("CHAR")){
    printf("%c", get_char_token_value());
    current_token++;
  } else if (verify_token("IDENTIFIER")){
    if (is_variable(get_token_value())){
      Variable * variable = get_variable(get_token_value());
      if (verify_type_id(variable->type, "int")) printf("%i", variable->value.int_value);
      else if (verify_type_id(variable->type, "float")) printf("%.2f", variable->value.float_value);
      else if (verify_type_id(variable->type, "char")) printf("%c", variable->value.char_value);
      else if (verify_type_id(variable->type, "string")) printf("%s", variable->value.string_value);
      else if (verify_type_id(variable->type, "bool")) printf("%i", variable->value.bool_value);
      else if (verify_type_id(variable->type, "empty")) printf("(empty)");
      else invalid_var(get_token_value(), ERROR);
      current_token++;
    }
  } else{
    void * ex = evaluate_expression();
    if (strcmp(type_value_return, "int") == 0) printf("%i", *(int *)ex);
    else if (strcmp(type_value_return, "float") == 0) printf("%.2f", *(double *)ex);
    else if (strcmp(type_value_return, "string") == 0) printf("%s", *(char *)ex);
    else if (strcmp(type_value_return, "char") == 0) printf("%c", (char)ex);
    else if (strcmp(type_value_return, "bool") == 0) printf("%i", *(bool *)ex);
    else if (strcmp(type_value_return, "empty") == 0) printf("(empty)");
    else show_errors("Expected valid value in puts function", ERROR);
  }
  expected(")", "puts", "R_PAREN");
}

void free_format_string (format_string * str) {
    if (str == NULL) return;
    for (size_t i = 0; i < str->count; i++) free(str->matches[i]);
    free(str->matches);
    str->count = 0;
}

format_string get_format_string (const char* input) {
    format_string result = {NULL, 0};
    if (input == NULL || strlen(input) == 0) return result;
    const char *start = input;
    const char *ptr;
    size_t capacity = 2;
    size_t count = 0;
    result.matches = malloc(capacity * sizeof(char*));
    if (result.matches == NULL) return result;
    while ((ptr = strstr(start, "&{")) != NULL) {
        ptr += 2;
        const char * end = strchr(ptr, '}');
        if (end == NULL) break;
        size_t length = end - ptr;
        if (length == 0) {
            start = end + 1;
            continue;
        }
        if (count >= capacity) {
            capacity *= 2;
            char ** temp = realloc(result.matches, capacity * sizeof(char*));
            if (temp == NULL) {
                free_format_string(&result);
                return result;
            }
            result.matches = temp;
        }
        result.matches[count] = malloc(length + 1);
        if (result.matches[count] == NULL) {
            free_format_string(&result);
            return result;
        }
        strncpy(result.matches[count], ptr, length);
        result.matches[count][length] = '\0';
        count++;
        start = end + 1;
    }
    result.count = count;
    return result;
}

char * replace_format_string (const char * input) {
    if (input == NULL || strlen(input) == 0) return NULL;
    size_t new_length = strlen(input) + 1;
    const char *ptr = input;
    const char *last_pos = input;
    while ((ptr = strstr(last_pos, "&{")) != NULL) {
        const char * end = strchr(ptr + 2, '}');
        if (end == NULL) break;
        new_length -= (end - ptr + 1) - 2;
        last_pos = end + 1;
    }
    char * result = malloc(new_length);
    if (result == NULL) return NULL;
    char * dest = result;
    last_pos = input;
    ptr = input;
    while ((ptr = strstr(last_pos, "&{")) != NULL) {
        size_t copy_len = ptr - last_pos;
        strncpy(dest, last_pos, copy_len);
        dest += copy_len;
        const char * end = strchr(ptr + 2, '}');
        if (end == NULL) {
            strcpy(dest, ptr);
            return result;
        }
        strcpy(dest, "%s");
        dest += 2;
        last_pos = end + 1;
    }
    strcpy(dest, last_pos);
    return result;
}

char * fatos (float value) {
    char * buffer = malloc(32 * sizeof(char));
    if (buffer == NULL) return NULL;
    snprintf(buffer, 32, "%.2f", value);
    return buffer;
}

char * catos (char value) {
    char * buffer = malloc(2 * sizeof(char));
    if (buffer == NULL) return NULL;
    buffer[0] = value;
    buffer[1] = '\0';
    return buffer;
}

char * batos (bool value) {
    return value ? "true" : "false";
}

char * string_format (const char * str){
   format_string format_str = get_format_string(str);
   if (format_str.count < 0) return NULL;
   char * formated = replace_format_string(str);
   if (formated == NULL) return NULL;
   char result_str[MAX_TOKEN_LENGTH];
   for (int i = 0; i < format_str.count; i++){
      char tmp[MAX_TOKEN_LENGTH];
      if (is_variable(format_str.matches[i])){
         Variable * var = get_variable(format_str.matches[i]);
         if (strcmp_(var->type, "int")) snprintf(tmp, sizeof(tmp), formated, atos(var->value.int_value));
         else if (strcmp_(var->type, "float")) snprintf(tmp, sizeof(tmp), formated, fatos(var->value.float_value));
         else if (strcmp_(var->type, "string")) snprintf(tmp, sizeof(tmp), formated, var->value.string_value);
         else if (strcmp_(var->type, "char")) snprintf(tmp, sizeof(tmp), formated, catos(var->value.char_value));
         else if (strcmp_(var->type, "empty")) snprintf(tmp, sizeof(tmp), formated, "(empty)");
         else if (strcmp_(var->type, "bool")) snprintf(tmp, sizeof(tmp), formated, batos(var->value.bool_value));
         else notEvaluateVariable(var->name, var->type, ERROR);
      } else show_errors("Invalid value in formated string", ERROR);
      strcat(result_str, tmp);
   }
   return result_str;
}

bool strcmp_ (const char * str1, const char * str2) {
   if (strcmp(str1, str2) == 0) return true;
   return false;
}

