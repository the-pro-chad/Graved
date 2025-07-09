/*
 * function.c : archivo encargado de manejar
 * las funciones en el interprete*/
#include "../include/function.h"
#include "../include/error.h"
#include "../include/variable.h"

void define_function() {
    char * type_param = "int";
    if (function_count >= MAX_FUNCTIONS) show_errors ("Too many functions",-1);
    Function * func = &functions[function_count++];
    if (current_token >= token_count || verify_token("IDENTIFIER")) show_errors ("Expeced function name", ERROR);
    strcpy(func->name, get_token_value());
    current_token++;
    if (current_token >= token_count || !verify_token("L_PAREN")) show_errors ("Expeced '(' after function name", ERROR);
    current_token++;
    func->param_count = 0;
    while (current_token < token_count && !verify_token("L_PAREN")) {
        if (!verify_token("IDENTIFIER")) show_errors ("Expected parameter name", ERROR);
        strcpy(func->params[func->param_count++], get_token_value());
        current_token++;
        if (!verify_token("COLON")) show_errors ("Expected ':' for type variable", ERROR);
        current_token++;
        if (verify_token("STRING_PARAM")) type_param = "string";
        else if (verify_token("FLOAT_PARAM")) type_param = "float";
        else if (verify_token("INTEGER_PARAM")) type_param = "int";
        else if (verify_token("CHAR_PARAM")) type_param = "char";
        else if (verify_token("BOOL_PARAM")) type_param = "bool";
        else show_errors("Expected type of param in function definition", ERROR);
        strcpy(func->params_type[func->type_param_count++], type_param);
        current_token++;
        if (current_token < token_count && verify_token("COMMA")) current_token++;
    }
    if (current_token >= token_count || !verify_token("R_PAREN")) show_errors ("Expeced ')' after function parameters", ERROR);
    current_token++;
    if (current_token >= token_count || !verify_token("THEN")) show_errors ("Expeced 'then' after function declaration",-1);
    current_token++;
    func->body_token_count = 0;
    while (current_token < token_count && !verify_token("END")) {
        func->body[func->body_token_count++] = tokens[current_token];
        current_token++;
    }
    if (current_token >= token_count || !verify_token("END")) show_errors ("Expeced 'end' at the end of function",-1);
    current_token++;
}

void * call_function(Function* func, void * args[], int arg_count) {
    if (arg_count != func->param_count) error_funcs_args(func->name, func->param_count, arg_count);
    Variable saved_vars[MAX_VARIABLES];
    int saved_var_count = variable_count;
    memcpy(saved_vars, variables, sizeof(variables));
    for (int i = 0; i < func->param_count; i++) set_variable(func->params[i], func->params_type[i], &args[i]);
    int saved_current_token = current_token;
    Token saved_tokens[MAX_TOKENS];
    int saved_token_count = token_count;
    memcpy(saved_tokens, tokens, sizeof(tokens));
    memcpy(tokens, func->body, sizeof(Token) * func->body_token_count);
    token_count = func->body_token_count;
    current_token = 0;
    void * result = 0;
    while (current_token < token_count) {
        if (verify_token("RETURN")) {
            current_token++;
            result = evaluate_expression();
            break;
        }
        evaluate_expression();
        //interpret();
    }
    memcpy(tokens, saved_tokens, sizeof(tokens));
    token_count = saved_token_count;
    current_token = saved_current_token;
    memcpy(variables, saved_vars, sizeof(variables));
    variable_count = saved_var_count;
    return result;
}

Function * get_function(const char* name) {
    for (int i = 0; i < function_count; i++) {
        if (strcmp(functions[i].name, name) == 0) return &functions[i];
    }
    return NULL;
}

