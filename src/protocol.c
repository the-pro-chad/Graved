/*
 * protocol.c : archivo encargado de manejar
 * la estructura protocol y todo lo referente
 * al manejo de los sockets en el interprete*/
#include "../include/protocol.h"
#include "../include/error.h"
#include "../include/variable.h"

void protocol_http (Protocol * pro){
   pro->client_len = sizeof(pro->client_addr);
   int a, b, c;
   while (current_token < token_count && strcmp(tokens[current_token].type, "END") != 0) {
      if (!verify_token("TYPE_PROTOCOL")) show_errors("Expected 'HTTP' in body protocol", ERROR_SINTAXIS);
      if (!verify_token_value("HTTP")) show_errors("Invalid protocol", ERROR_SINTAXIS);
      current_token++;
      if (!verify_token("COLON")) show_errors("Expeced ':' in protocol definition", ERROR_SINTAXIS);
      current_token++;
      if (!verify_token("COLON")) show_errors("Expeced ':' in protocol definition", ERROR_SINTAXIS);
      current_token++;
      if (verify_token_value("socket")) current_token++;
      if (!verify_token("L_PAREN")) show_errors("Expected '(' in socket funtion", ERROR_SINTAXIS);
      current_token++;
      if (verify_token("NUMBER")) a = get_int_token_value();
      else if (verify_token("AF_INET")) a = AF_INET;
      else show_errors("Expected a number in socket protocol", ERROR_SINTAXIS);
      current_token++;
      if (verify_token("COMMA")) show_errors("Expected ',' in socket funtion", ERROR_SINTAXIS);
      current_token++;
      if (verify_token("NUMBER")) b = atoi(tokens[current_token].value);
      else if (verify_token("SOCK_STREAM")) b = SOCK_STREAM;
      else show_errors("Expected a number in socket protocol", ERROR_SINTAXIS);
      current_token++;
      if (verify_token("COMMA")) show_errors("Expected ',' in socket funtion", ERROR_SINTAXIS);
      current_token++;
      if (verify_token("NUMBER")) show_errors("Expected a number value in socket funtion", ERROR_SINTAXIS);
      current_token++;
      c = get_int_token_value();
      if (verify_token("R_PAREN")) show_errors("Expected ')' in socket http protocol", ERROR_SINTAXIS);
      current_token++;
      pro->server_socket = socket(a, b, c);
      if (pro->server_socket < 0) show_errors("Error at create socket", ERROR_SINTAXIS);
      memset(&pro->server_addr, 0, sizeof(pro->server_addr));
   }
      if (verify_token_value("sin_family")){
         current_token++;
         int a = 0;
         if (current_token >= token_count || strcmp(tokens[current_token].type, "OPERATOR") != 0) show_errors ("Expeced '=' after http sin_family protocol", ERROR_SINTAXIS);
            if (!verify_token_value("=")) show_errors("Expected '=' after http sin_family protocol", ERROR_SINTAXIS);
            current_token++;
            if (verify_token("NUMBER")) a = get_int_token_value();
            else if(verify_token("AF_INET")) a = AF_INET;
            else show_errors("Expected a number value in http sin_family protocol", ERROR_SINTAXIS);
            pro->server_addr.sin_family = a;
            current_token++;
      }
      if(verify_token_value("url")){
         current_token++;
         if (current_token >= token_count || strcmp(tokens[current_token].type, "OPERATOR") != 0) show_errors ("Expeced '=' after url http protocol", ERROR_SINTAXIS);
            if (tokens[current_token].value[0] != '=') show_errors("Expected '=' after url http protocol", ERROR_SINTAXIS);
            current_token++;
            if (strcmp(tokens[current_token].type,"STRING") != 0) show_errors("Expected a string value in url http protocol", ERROR_SINTAXIS);
            pro->server_addr.sin_addr.s_addr = INADDR_ANY;
            current_token++;
      }
      if(strcmp(tokens[current_token].value,"port") == 0){
         current_token++;
         if (current_token >= token_count || strcmp(tokens[current_token].type, "OPERATOR") != 0) show_errors ("Expeced '=' after port http protocol", ERROR_SINTAXIS);
         if (tokens[current_token].value[0] != '=') show_errors("Expected '=' after port http protocol", ERROR_SINTAXIS);
         current_token++;
         if (strcmp(tokens[current_token].type,"NUMBER") != 0) show_errors("Expected a number value in port protocol", ERROR_SINTAXIS);
         pro->server_addr.sin_port = htons(atoi(tokens[current_token].value));
         current_token++;
      }
      if(strcmp(tokens[current_token].value,"dir") == 0){
         current_token++;
         if (current_token >= token_count || strcmp(tokens[current_token].type, "OPERATOR") != 0) show_errors ("Expeced '=' after dir http protocol", ERROR_SINTAXIS);
            if (tokens[current_token].value[0] != '=') show_errors("Expected '=' after dir http protocol", ERROR_SINTAXIS);
            current_token++;
            if (strcmp(tokens[current_token].type,"STRING") != 0) show_errors("Expected a string value in dir http protocol", ERROR_SINTAXIS);
            char aux[MAX_TOKEN_LENGTH];
            removeQuotes(tokens[current_token].value,aux);
            strcpy(pro->dir,aux);
            current_token++;
      }
      if(strcmp(tokens[current_token].value,"bind") == 0){
         current_token++;
         if (bind(pro->server_socket, (struct sockaddr *)&pro->server_addr, sizeof(pro->server_addr)) < 0) {
            perror("");
            exit(ERROR);
         }
      }
      if(strcmp(tokens[current_token].value,"listen") == 0){
         current_token++;
         if (current_token >= token_count || strcmp(tokens[current_token].type, "OPERATOR") != 0) show_errors ("Expeced '=' after listen http protocol", ERROR_SINTAXIS);
         if (tokens[current_token].value[0] != '=') show_errors("Expected '=' after listen http protocol", ERROR_SINTAXIS);
         current_token++;
         if (strcmp(tokens[current_token].type,"NUMBER") != 0) show_errors("Expected a number value in listen http protocol", 1);
         if (listen(pro->server_socket, atoi(tokens[current_token].value)) < 0) {
            perror("");
            exit(1);
         }
         current_token++;
      }
      if (strcmp(tokens[current_token].value,"files_accept") == 0){
         current_token++;
         if (current_token >= token_count || strcmp(tokens[current_token].type, "OPERATOR") != 0) show_errors ("Expeced '=' after files_accept http protocol", ERROR_SINTAXIS);
         if (tokens[current_token].value[0] != '=') show_errors("Expected '=' after files_accept http protocol", ERROR_SINTAXIS);
         current_token++;
         if (strcmp(tokens[current_token].type,"STRING") != 0) show_errors("Expected a string in files_accept http protocol", ERROR_SINTAXIS);
         char aux[MAX_TOKEN_LENGTH];
         removeQuotes(tokens[current_token].value,aux);
         strcpy(pro->files_accept,aux);
         current_token++;
      }
      if(strcmp(tokens[current_token].value,"mimes_accept") == 0){
         current_token++;
         if (current_token >= token_count || strcmp(tokens[current_token].type, "OPERATOR") != 0) show_errors ("Expeced '=' after mimes_accept http protocol",ERROR_SINTAXIS);
         if (tokens[current_token].value[0] != '=') show_errors("Expected '=' after mimes_accept http protocol", ERROR_SINTAXIS);
         current_token++;
         if (strcmp(tokens[current_token].type,"STRING") != 0) show_errors("Expected a string in mimes_accept http protocol", ERROR_SINTAXIS);
         char aux[MAX_TOKEN_LENGTH];
         removeQuotes(tokens[current_token].value,aux);
         strcpy(pro->mimes_accept,aux);
         current_token++;
      }
}

/* Funciones para manejar la definicion
 * algunos protocolos
 * de forma mas "facil" en el interprete*/
void protocol_ftp (){
}
void protocol_smtp (){
}
void protocol_dns (){
}
void protocol_tcp (){
}
void protocol_pop3 (){
}
void protocol_imap (){
}
void protocol_ssh (){
}
void protocol_snmp (){
}
void protocol_https (){
}
void protocol_sftp (){
}
void protocol_web_socket (){
}
void protocol_mttq (){
}
void protocol_udp (){
}

Protocol * get_protocol (const char * name){
   for (int i = 0; i < protocol_count; i++) {
      if (strcmp(protocol[i].name, name) == 0) return &protocol[i];
   }
   return NULL;
}

void define_protocol (){
    char * type_protocol = "http";
    if (protocol_count >= MAX_VARIABLES) show_errors ("Too many protocols",-1);
    Protocol * pro = &protocol[protocol_count++];
    if (current_token >= token_count || strcmp(tokens[current_token].type, "IDENTIFIER") != 0) show_errors ("Expeced protocol name",-1);
    strcpy(pro->name, tokens[current_token].value);
    current_token++;
    if(strcmp(tokens[current_token].type,"COLON") != 0) show_errors("Expeced ':' in protocol definition",-1);
    current_token++;
    if(strcmp(tokens[current_token].type,"COLON") != 0) show_errors("Expeced ':' in protocol definition",-1);
    current_token++;
    if(strcmp(tokens[current_token].type,"TYPE_PROTOCOL") != 0) show_errors("Expeced protocol type",-1);
    type_protocol = tokens[current_token].value;
    current_token++;
    if (current_token >= token_count || strcmp(tokens[current_token].type, "THEN") != 0) show_errors ("Expeced 'then' before protocol definition",-1);
    current_token++;
    if(strcmp(type_protocol,"HTTP") == 0){
    	pro->type_protocol = 0;
    	strcpy(pro->protocol_type,"HTTP");
    	protocol_http(pro);
    } else if(strcmp(type_protocol,"FTP") == 0){
    	protocol_ftp();
    } else if(strcmp(type_protocol,"SMTP") == 0){
    	protocol_smtp();
    } else if(strcmp(type_protocol,"DNS") == 0){
    	protocol_dns();
    } else if(strcmp(type_protocol,"POP3") == 0){
    	protocol_pop3();
    } else if(strcmp(type_protocol,"IMAP") == 0){
    	protocol_imap();
    } else if(strcmp(type_protocol,"SSH") == 0){
    	protocol_ssh();
    } else if(strcmp(type_protocol,"SNMP") == 0){
    	protocol_snmp();
    } else if(strcmp(type_protocol,"HTTPS") == 0){
    	protocol_https();
    } else if(strcmp(type_protocol,"SFTP") == 0){
    	protocol_sftp();
    } else if(strcmp(type_protocol,"MTTQ") == 0){
    	protocol_mttq();
    } else if(strcmp(type_protocol,"UDP") == 0){
    	protocol_udp();
    } else if(strcmp(type_protocol,"TCP") == 0){
    	protocol_tcp();
    } else show_errors("Invalid protocol type",-1);
    if (current_token >= token_count || strcmp(tokens[current_token].type, "END") != 0) show_errors ("Expected 'end' at the end of protocol struture",-1);
    current_token++;
}

//Funciones para manejar el protcol::HTTP->servidor web simple
void send_response(int client_socket, int status_code, const char *status_text, const char *content_type, const char *content, int content_length) {
    char response_header[BUFFER_SIZE];
    snprintf(response_header, BUFFER_SIZE,
             "HTTP/1.1 %d %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n"
             "\r\n",
             status_code, status_text, content_type, content_length);
    write(client_socket, response_header, strlen(response_header));
    write(client_socket, content, content_length);
}

void send_file(int client_socket, const char * file_path) {
    FILE * file = fopen(file_path, "rb");
    if (!file) {
        send_response(client_socket, 404, "Not Found", "text/plain", "404 Not Found", 12);
        return;
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *file_content = malloc(file_size);
    fread(file_content, 1, file_size, file);
    fclose(file);
    const char *content_type = "text/plain";
    if (strstr(file_path, ".html")) content_type = "text/html";
    else if (strstr(file_path, ".css")) content_type = "text/css";
    else if (strstr(file_path, ".js")) content_type = "application/javascript";
    else if (strstr(file_path, ".png")) content_type = "image/png";
    else if (strstr(file_path, ".jpg") || strstr(file_path, ".jpeg")) content_type = "image/jpeg";
    else if (strstr(file_path, ".gif")) content_type = "image/gif";
    else if (strstr(file_path, ".ico")) content_type = "image/x-icon";
    send_response(client_socket, 200, "OK", content_type, file_content, file_size);
    free(file_content);
}

void handle_request(Protocol * pro,const char *request) {
    char method[16], path[256];
    sscanf(request, "%s %s", method, path);
    if (strcmp(path, "/") == 0) {
        char file_path[512];
        snprintf(file_path, sizeof(file_path), "%s/index.html", pro->dir);
        send_file(pro->client_socket, file_path);
        return;
    }
    char file_path[512];
    snprintf(file_path, sizeof(file_path), "%s%s", pro->dir, path);
    if (access(file_path, F_OK) == -1) send_response(pro->client_socket, 404, "Not Found", "text/plain", "404 Not Found", 12);
    else send_file(pro->client_socket, file_path);
}

void * evalue_http_protocol (Protocol * pro, const char * member_name){
  current_token++;
  if(strcmp(member_name,"accept") == 0){
    expected("(","accept","L_PAREN");
    pro->client_socket = accept(pro->server_socket, (struct sockaddr *)&pro->client_addr, &pro->client_len);
    if (pro->client_socket < 0) {
      perror("");
      exit(ERROR_INTERPRETER);
    }
    expected(")","accept","R_PAREN");
  } else if(strcmp(member_name,"read") == 0){
    expected("(","read","L_PAREN");
    int r  = read(pro->client_socket, http_buffer, BUFFER_SIZE - 1);
    if (r < 0) {
      perror("");
      exit(ERROR_INTERPRETER);
    }
    http_buffer[r] = '\0';
    expected(")","read","R_PAREN");
  } else if(strcmp(member_name,"handle_request") == 0){
    expected("(","L_handle_request","PAREN");
    expected(")","R_handle_request","PAREN");
    handle_request(pro,http_buffer);
  } else if(strcmp(member_name,"close") == 0){
    expected("(","L_close","PAREN");
    expected(")","R_close","PAREN");
    close(pro->client_socket);
  } else if(strcmp(member_name,"show_buffer") == 0){
    expected("(","L_show_buffer","PAREN");
    expected(")","R_show_buffer","PAREN");
    puts(http_buffer);
  } else {
     int before_token = current_token;
     char * name = tokens[before_token-=2].value;
     undefined_member(tokens[current_token].value,name,ERROR_INTERPRETER);
  }
  return 0;
}

