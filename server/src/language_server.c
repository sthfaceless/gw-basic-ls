//
// Created by danil on 25.04.2021.
//

#include "language_server.h"

static char *send_message_buf;

json_value *get_by_name(json_value *val, const char *name) {
    size_t length = val->u.object.length;
    for (int i = 0; i < length; ++i)
        if (strcmp(val->u.object.values[i].name, name) == 0) {
            return val->u.object.values[i].value;
        }
    return json_null_new();
}

json_value *get_initialization_response(json_value *val) {

    json_value *response = json_object_new(2);

    /*
     * Опции сохранения документа
     * */
    json_value *text_document_sync_options_save = json_object_new(1);
    json_object_push(text_document_sync_options_save, "includeText", json_boolean_new(1));

    /*
     * Сообщать о открытии-закрытии документа, изменении и сохранение
     * */
//    json_value *text_document_sync_options = json_object_new(2);
//    json_object_push(text_document_sync_options, "openClose", json_boolean_new(1));
//    json_object_push(text_document_sync_options, "change", json_integer_new(1));

    json_value *completion_options = json_object_new(1);
    /*
     * Предостовлять автодополнение
     * */
    json_object_push(completion_options, "resolveProvider", json_boolean_new(1));

    /*
     * Предлагать автодополнение на конкретные слова
     * */
//    json_object_push(completion_options, "triggerCharacters", json_object_new(0));

    /*
     * Предоставляем опции
     * 1) Синхронизация с документом для валидации
     * 2) Автодополнение
     * 3) Определение элементов
     * */
    json_value *capabilities = json_object_new(3);
    json_object_push(capabilities, "textDocumentSync", json_integer_new(1));
    json_object_push(capabilities, "completionProvider", completion_options);
    json_object_push(capabilities, "definitionProvider", json_boolean_new(1));

    json_value *initialize_result = json_object_new(1);
    json_object_push(initialize_result, "capabilities", capabilities);

    json_object_push(response, "id", json_integer_new(get_by_name(val, "id")->u.integer));
    json_object_push(response, "result", initialize_result);

    return response;
}

void send_message(json_value *val) {

    json_object_push(val, "jsonrpc", json_string_new("2.0"));

    char *buf = send_message_buf;
    json_serialize(buf, val);
    size_t json_size = strlen(buf);

    Logger->log(log_info, buf);

    fprintf(stdout, "Content-Length: %lu\r\n", json_size);
    fprintf(stdout, "Content-Type: application/vscode-jsonrpc;charset=utf-8\r\n\r\n");
    fprintf(stdout, "%s", buf);

    fflush(stdout);
}

_Bool _process(const language_server *self, const char *str, const size_t size) {

    json_value *val = json_parse(str, size);
    json_value *response = json_null_new();

    json_value *method_value = get_by_name(val, "method");
    if (method_value->type != json_null) {
        const char *method = method_value->u.string.ptr;
        if (!strcmp(method, "initialize")) {
            response = get_initialization_response(val);
        } else if (!strcmp(method, "initialized")) {
            Logger->log(log_info, "initialized");
        } else if (!strcmp(method, "shutdown") || !strcmp(method, "exit")) {
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

    ls->process = _process;

    send_message_buf = malloc(MAX_LSP_MESSAGE_SIZE);

    return ls;
}

void free_language_server(language_server *self) {
    free(send_message_buf);
    free(self);
}