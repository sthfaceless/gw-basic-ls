//
// Created by danil on 25.04.2021.
//

#include "language_server.h"

static char *send_message_buf;

static json_value *get_by_name(const json_value *val, const char *name) {
    size_t length = val->u.object.length;
    for (int i = 0; i < length; ++i)
        if (strcmp(val->u.object.values[i].name, name) == 0) {
            return val->u.object.values[i].value;
        }
    return json_null_new();
}

static json_value *prepare_response_object(const json_value* val, json_value * result){
    json_value * response = json_object_new(2);
    json_object_push(response, "id", json_integer_new(get_by_name(val, "id")->u.integer));
    json_object_push(response, "result", result);
    return response;
}

static json_value *prepare_diagnostics_object(const json_value *document, json_value *diagnostics_list){


    json_value* diagnostics_params = json_object_new(3);
    json_object_push(diagnostics_params, "uri", json_string_new(strdup(get_by_name(document, "uri")->u.string.ptr)));
    json_object_push(diagnostics_params, "version", json_integer_new(get_by_name(document, "version")->u.integer));
    json_object_push(diagnostics_params, "diagnostics", diagnostics_list);


    json_value * diagnostics = json_object_new(2);
    json_object_push(diagnostics, "method", json_string_new("textDocument/publishDiagnostics"));
    json_object_push(diagnostics, "params", diagnostics_params);

    return diagnostics;
}

static json_value *get_validation_response(const json_value * val){

    json_value *document = get_by_name(get_by_name(val, "params"), "textDocument");
    const char * text = get_by_name(document, "text")->u.string.ptr;


    return prepare_diagnostics_object(document, json_array_new(0));
}

static json_value *get_completion_response(const json_value * val){
    return prepare_response_object(val, json_null_new());
}

static json_value *get_symbols_response(const json_value * val){
    return prepare_response_object(val, json_null_new());
}

static json_value *get_initialization_response(const json_value *val) {

    /*
     * Опции сохранения документа
     * */
    json_value *text_document_sync_options_save = json_object_new(1);
    json_object_push(text_document_sync_options_save, "includeText", json_boolean_new(1));

    /*
     * Сообщать о открытии-закрытии документа, изменении и сохранение
     * */
    json_value *text_document_sync_options = json_object_new(3);
    json_object_push(text_document_sync_options, "openClose", json_boolean_new(1));
    json_object_push(text_document_sync_options, "change", json_integer_new(1));
    json_object_push(text_document_sync_options, "save", text_document_sync_options_save);

    json_value *completion_options = json_object_new(1);
    /*
     * Предостовлять автодополнение
     * */
    json_object_push(completion_options, "resolveProvider", json_boolean_new(1));

    /*
     * Предлагать автодополнение на конкретные слова
     * */
//    json_object_push(completion_options, "triggerCharacters", json_object_new(0));

    json_value* workspaceFolders = json_object_new(1);
    json_object_push(workspaceFolders, "supported", json_boolean_new(1));

    json_value * workspace = json_object_new(1);
    json_object_push(workspace, "workspaceFolders", workspaceFolders);
    /*
     * Предоставляем опции
     * 1) Синхронизация с документом для валидации
     * 2) Автодополнение
     * 3) Определение где данный элемент был определен
     * 4) Подсветка синтаксиса
     * 5) Workspace
     * */
    json_value *capabilities = json_object_new(5);
    json_object_push(capabilities, "textDocumentSync", text_document_sync_options);
    json_object_push(capabilities, "completionProvider", completion_options);
    json_object_push(capabilities, "definitionProvider", json_boolean_new(1));
    json_object_push(capabilities, "documentSymbolProvider", json_boolean_new(1));
    json_object_push(capabilities, "workspace", workspace);

    json_value *initialize_result = json_object_new(1);
    json_object_push(initialize_result, "capabilities", capabilities);

    return prepare_response_object(val, initialize_result);
}

static void send_message(json_value *val) {

    json_object_push(val, "jsonrpc", json_string_new("2.0"));

    char *buf = send_message_buf;
    json_serialize(buf, val);
    remove_spaces(buf);
    size_t json_size = strlen(buf) * sizeof (char);

    Logger->log(log_response, buf);

    fprintf(stdout, "Content-Length: %lu\r\n", json_size);
    fprintf(stdout, "Content-Type: application/vscode-jsonrpc;charset=utf-8\r\n\r\n");
    fprintf(stdout, "%s", buf);

    fflush(stdout);
}

static _Bool process(const language_server *self, const char *str, const size_t size) {

    Logger->log(log_request, str);

    json_value *val = json_parse(str, size);
    json_value *response = json_null_new();

    json_value *method_value = get_by_name(val, "method");
    if (method_value->type != json_null) {
        const char *method = method_value->u.string.ptr;
        if (!strcmp(method, "initialize")) {
            response = get_initialization_response(val);
        } else if (!strcmp(method, "initialized")) {
            Logger->log(log_info, "initialized");
        } else if(!strcmp(method, "textDocument/didChange") || !strcmp(method, "textDocument/didOpen")){
            response = get_validation_response(val);
        }else if(!strcmp(method, "textDocument/completion")){
            response = get_completion_response(val);
        }else if(!strcmp(method, "textDocument/documentSymbol")){
            response = get_symbols_response(val);
        }else if (!strcmp(method, "shutdown") || !strcmp(method, "exit")) {
            return 0;
        }
    }

    if (response->type != json_null)
        send_message(response);

    json_value_free(response);
    json_value_free(val);

    return 1;
}

language_server *create_language_server() {
    language_server *ls = malloc(sizeof(language_server));

    ls->process = process;

    send_message_buf = malloc(MAX_LSP_MESSAGE_SIZE);

    return ls;
}

void free_language_server(language_server *self) {
    free(send_message_buf);
    free(self);
}