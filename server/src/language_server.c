//
// Created by danil on 25.04.2021.
//

#include "language_server.h"

static document* create_document_item(language_server *server, char *text){
	document *self = malloc(sizeof (document));
	self->text = copystr(text);
	self->variables = create_wtree_sized(0);
	self->tokens = server->parser->_tokenizer->make_tokens(server->parser->_tokenizer, self);
	return self;
}


static json_value* prepare_response_object(const json_value* val, json_value* result) {
	json_value* response = json_object_new(2);
	json_object_push(response, "id", json_integer_new(get_by_name(val, "id")->u.integer));
	json_object_push(response, "result", result);
	return response;
}

static json_value* prepare_tokens_object(const json_value* val, json_value* arr) {

	json_value* result = json_object_new(1);
	json_object_push(result, "data", arr);

	return prepare_response_object(val, result);
}

static json_value* get_location_item(const json_value* val,const range *_range){
	json_value *location = json_object_new(2);
	json_object_push(location, "uri", json_string_new(copystr(
		get_by_name(get_by_name(get_by_name(val, "params"), "textDocument"), "uri")->u.string.ptr)));
	json_object_push(location, "range", create_range(_range->line_l, _range->l, _range->line_r, _range->r));
	return location;
}

static json_value* prepare_diagnostics_object(const json_value* doc_json, json_value* diagnostics_list) {

	json_value* diagnostics_params = json_object_new(3);
	json_object_push(diagnostics_params, "uri", json_string_new(copystr(get_by_name(doc_json, "uri")->u.string.ptr)));
	json_object_push(diagnostics_params, "version", json_integer_new(get_by_name(doc_json, "version")->u.integer));
	json_object_push(diagnostics_params, "diagnostics", diagnostics_list);

	json_value* diagnostics = json_object_new(2);
	json_object_push(diagnostics, "method", json_string_new("textDocument/publishDiagnostics"));
	json_object_push(diagnostics, "params", diagnostics_params);

	return diagnostics;
}

static json_value* get_validation_response(language_server* self, const json_value* val) {

	json_value* diagnostics = json_array_new(0);

	json_value* doc_json = get_by_name(get_by_name(val, "params"), "textDocument");
	char* uri = get_by_name(doc_json, "uri")->u.string.ptr;
	document* doc = self->documents->get(self->documents, uri);
	if (!doc)
		return prepare_diagnostics_object(val, diagnostics);

	vector* result = self->parser->validate(self->parser, doc);
	iterator* it = result->iterator(result);
	while (it->has_next(it)) {
		diagnostic* dgn = (diagnostic*)it->get_next(it);

		json_value* json_dgn = json_object_new(0);
		json_object_push(json_dgn, "range", create_range(dgn->line, dgn->l, dgn->line, dgn->r));
		json_object_push(json_dgn, "severity", json_integer_new(dgn->t+1));
		json_object_push(json_dgn, "message", json_string_new(copystr(dgn->message)));
		json_object_push(json_dgn, "source", json_string_new("gw-basic language server"));

		json_array_push(diagnostics, json_dgn);
	}
	free_diagnostic_items(result);

	return prepare_diagnostics_object(doc_json, diagnostics);
}

static json_value* get_documentation_by_keyword(gwkeyword *keyword){

	json_value* content = json_object_new(2);
	json_object_push(content, "kind", json_string_new("markdown"));


	/*
	 * ### name
	 * #### purpose
	 * ```
	 * syntax
	 * ```
	 * */
	string* markdown = get_string_from("### ");
	markdown->conc(markdown, get_string_from(keyword->name));

	markdown->conc(markdown, get_string_from("\n#### "));
	markdown->conc(markdown, get_string_from(keyword->purpose));

	markdown->conc(markdown, get_string_from("\n```\n"));
	markdown->conc(markdown, get_string_from(keyword->syntax));
	markdown->conc(markdown, get_string_from("\n```"));

	json_object_push(content, "value", json_string_new(markdown->get_chars_and_terminate(markdown)));

	return content;
}

static json_value* get_label_detail(char *detail){
	json_value* detail_object = json_object_new(1);
	json_object_push(detail_object, "type", json_string_new(copystr(detail)));
	return detail_object;
}

static json_value* get_completion_response(language_server* self, const json_value* val) {
	json_value* doc_json = get_by_name(get_by_name(val, "params"), "textDocument");
	document* doc = self->documents->get(self->documents, get_by_name(doc_json, "uri")->u.string.ptr);
	if (!doc)
		return prepare_response_object(val, json_null_new());

	json_value* pos = get_by_name(get_by_name(val, "params"), "position");
	int line = get_by_name(pos, "line")->u.integer, character = get_by_name(pos, "character")->u.integer;

	json_value* result = json_array_new(0);
	vector* completions = self->parser->make_completions(self->parser, doc, line, character);
	for (int i = 0; i<completions->size; ++i) {
		completionItem* item = (completionItem*)completions->get(completions, i);
		json_value* completion_item = json_object_new(2);
		json_object_push(completion_item, "label", json_string_new(copystr(item->name)));
		json_object_push(completion_item, "kind", json_integer_new(item->kind));
		json_object_push(completion_item, "detail", json_string_new(copystr(item->detail)));
		if(item->keyword){
			json_object_push(completion_item, "documentation", get_documentation_by_keyword(item->keyword));
		}

		json_array_push(result, completion_item);
	}
	free_completion_items(completions);

	return prepare_response_object(val, result);
}

json_value* get_completion_resolve_response(language_server *self, json_value* val){

	json_value* result = json_object_new(3);
	json_value* params = get_by_name(val, "params");

	json_object_push(result, "label", json_string_new(copystr(get_by_name(params, "label")->u.string.ptr)));
	json_object_push(result, "detail", json_string_new(copystr(get_by_name(params, "detail")->u.string.ptr)));
	json_object_push(result, "kind", json_integer_new(get_by_name(params, "kind")->u.integer));

	json_value *_docs = get_by_name(params, "documentation");
	if(_docs->type != json_null){
		json_value* docs = json_object_new(2);
		json_object_push(docs, "kind", json_string_new(copystr(get_by_name(_docs, "kind")->u.string.ptr)));
		json_object_push(docs, "value", json_string_new(copystr(get_by_name(_docs, "value")->u.string.ptr)));
		json_object_push(result, "documentation", docs );
	}


	return prepare_response_object(val, result);
}

static json_value* get_tokens_response(language_server* self, const json_value* val) {

	json_value* params = get_by_name(val, "params");
	json_value* doc_json = get_by_name(params, "textDocument");
	document* doc = self->documents->get(self->documents, get_by_name(doc_json, "uri")->u.string.ptr);
	if (!doc)
		return prepare_tokens_object(val, json_array_new(0));

	json_value* jsonDocumentRange = get_by_name(params, "range");
	range* documentRange = create_range_object(0, 0, INT32_MAX, INT32_MAX);
	if (jsonDocumentRange->type!=json_null) {
		json_value* start = get_by_name(jsonDocumentRange, "start");
		documentRange->line_l = get_by_name(start, "line")->u.integer;
		documentRange->l = get_by_name(start, "character")->u.integer;
		json_value* end = get_by_name(jsonDocumentRange, "end");
		documentRange->line_r = get_by_name(end, "line")->u.integer;
		documentRange->r = get_by_name(end, "character")->u.integer;
	}

	lvector* tokens = make_tokens_with_range(doc->tokens, documentRange);
	json_value* arr = json_array_new(0);
	iterator *it = tokens->iterator(tokens);

	token* previous_token = NULL;
	while(it->has_next(it)) {
		token* tok = it->get_next(it);
		tok->kind = map_type_to_semantic_token(tok->kind);
		if (tok->kind!=Unknown) {

			json_array_push(arr, json_integer_new(previous_token ? tok->line_l - previous_token->line_r : tok->line_l));
			json_array_push(arr, json_integer_new(previous_token && previous_token->line_r == tok->line_l ?
				tok->l - previous_token->l : tok->l));
			json_array_push(arr, json_integer_new(tok->r-tok->l+1));
			json_array_push(arr, json_integer_new(tok->kind));
			json_array_push(arr, json_integer_new(0));

			previous_token = tok;
		}
	}
	free_lvector_no_values(tokens);

	return prepare_tokens_object(val, arr);
}

static json_value* prepare_hover_response(const json_value* val, hoverItem *item){

	json_value* result = json_object_new(1);
	if(item){
		json_object_push(result, "contents", get_documentation_by_keyword(item->keyword));
		json_object_push(result, "range", create_range(item->_range->line_l, item->_range->l,
			item->_range->line_r, item->_range->r));
	}

	return prepare_response_object(val, result);
}

static json_value* get_hover_response(language_server* self, const json_value* val){

	json_value* params = get_by_name(val, "params");
	json_value* doc_json = get_by_name(params, "textDocument");
	document* doc = self->documents->get(self->documents, get_by_name(doc_json, "uri")->u.string.ptr);
	if(!doc)
		return prepare_hover_response(val, NULL);

	json_value* pos = get_by_name(get_by_name(val, "params"), "position");
	int line = get_by_name(pos, "line")->u.integer, character = get_by_name(pos, "character")->u.integer;

	return prepare_hover_response(val,
		self->parser->get_hover_item(self->parser, doc, line, character));
}

static json_value* get_declaration_response(language_server *self, const json_value *val){

	json_value* params = get_by_name(val, "params");
	json_value* doc_json = get_by_name(params, "textDocument");
	document* doc = self->documents->get(self->documents, get_by_name(doc_json, "uri")->u.string.ptr);
	if(!doc)
		return prepare_response_object(val, json_null_new());

	json_value* pos = get_by_name(get_by_name(val, "params"), "position");
	int line = get_by_name(pos, "line")->u.integer, character = get_by_name(pos, "character")->u.integer;

	range* _range = self->parser->get_declaration(self->parser, doc, line, character);
	if(!_range)
		return prepare_response_object(val, json_null_new());

	return prepare_response_object(val, get_location_item(val, _range));
}

static json_value* get_initialization_response(const json_value* val) {

	/*
	 * Опции сохранения документа
	 * */
	json_value* text_document_sync_options_save = json_object_new(1);
	json_object_push(text_document_sync_options_save, "includeText", json_boolean_new(1));

	/*
	 * Сообщать о открытии-закрытии документа, изменении и сохранение
	 * */
	json_value* text_document_sync_options = json_object_new(3);
	json_object_push(text_document_sync_options, "openClose", json_boolean_new(1));
	json_object_push(text_document_sync_options, "change", json_integer_new(1));
	json_object_push(text_document_sync_options, "save", text_document_sync_options_save);

	json_value* completion_options = json_object_new(1);
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

	/*
	 * Semantic tokens support
	 * */
	vector* t_types = get_token_types();
	json_value* tokenTypes = json_array_new(t_types->size);
	for (int i = 0; i<t_types->size; ++i) {
		json_array_push(tokenTypes, json_string_new((char*)t_types->get(t_types, i)));
	}
	vector* t_modifiers = get_token_modifiers();
	json_value* tokenModifiers = json_array_new(t_modifiers->size);
	for (int i = 0; i<t_modifiers->size; ++i) {
		json_array_push(tokenModifiers, json_string_new((char*)t_modifiers->get(t_modifiers, i)));
	}

	json_value* semanticTokensLegend = json_object_new(2);
	json_object_push(semanticTokensLegend, "tokenTypes", tokenTypes);
	json_object_push(semanticTokensLegend, "tokenModifiers", tokenModifiers);

	json_value* semanticTokensProvider = json_object_new(1);
	json_object_push(semanticTokensProvider, "legend", semanticTokensLegend);
	json_object_push(semanticTokensProvider, "range", json_boolean_new(1));
	json_value* semanticTokensProviderFullOption = json_object_new(1);
	json_object_push(semanticTokensProviderFullOption, "delta", json_boolean_new(1));
	json_object_push(semanticTokensProvider, "full", semanticTokensProviderFullOption);

	json_value* workspace = json_object_new(1);
	json_object_push(workspace, "workspaceFolders", workspaceFolders);
	/*
	 * Предоставляем опции
	 * 1) Синхронизация с документом для валидации
	 * 2) Автодополнение
	 * 3) Определение где данный элемент был определен
	 * 4) Описание элемента
	 * 5) Workspace
	 * 6) Разделение на семантические токены
	 * 7) Variables definition
	 * */
	json_value* capabilities = json_object_new(5);
	json_object_push(capabilities, "textDocumentSync", text_document_sync_options);
	json_object_push(capabilities, "completionProvider", completion_options);
	json_object_push(capabilities, "definitionProvider", json_boolean_new(1));
	json_object_push(capabilities, "hoverProvider", json_boolean_new(1));
	json_object_push(capabilities, "workspace", workspace);
	json_object_push(capabilities, "semanticTokensProvider", semanticTokensProvider);
	json_object_push(capabilities, "definitionProvider", json_boolean_new(1));
	json_object_push(capabilities, "declarationProvider", json_boolean_new(1));

	json_value* initialize_result = json_object_new(1);
	json_object_push(initialize_result, "capabilities", capabilities);

	return prepare_response_object(val, initialize_result);
}

static void send_message(char* send_message_buf, json_value* val) {

	json_object_push(val, "jsonrpc", json_string_new("2.0"));

	char* buf = send_message_buf;
	json_serialize(buf, val);
	size_t json_size = strlen(buf)*sizeof(char);

	Logger->log(log_response, buf);

	fprintf(stdout, "Content-Length: %lu\r\n", json_size);
	fprintf(stdout, "Content-Type: application/vscode-jsonrpc;charset=utf-8\r\n\r\n");
	fprintf(stdout, "%s", buf);

	fflush(stdout);
}

static void update_document(language_server* self, const json_value* val) {
	json_value* doc_json = get_by_name(get_by_name(val, "params"), "textDocument");
	char* uri = copystr(get_by_name(doc_json, "uri")->u.string.ptr);

	char* text = copystr(get_by_name(doc_json, "text")->u.string.ptr);
	int len = strlen(text);
	if (!len) {
		json_value* changes = get_by_name(get_by_name(val, "params"), "contentChanges");
		if (changes->type!=json_null)
			text = copystr(get_by_name(changes->u.array.values[0], "text")->u.string.ptr);
	}
	strlower(text);

	document *doc = self->documents->get(self->documents, uri);
	if(!doc) {
		doc = create_document_item(self, text);
		self->documents->put(self->documents, uri, doc);
	}
	else {
		doc->text = text;
		doc->tokens = self->parser->_tokenizer->make_tokens(self->parser->_tokenizer, doc);
	}
}

static _Bool process(language_server* self, const char* str, const size_t size) {

	Logger->log(log_request, str);

	json_value* val = json_parse(str, size);
	json_value* response = json_null_new();

	json_value* method_value = get_by_name(val, "method");
	if (method_value->type!=json_null) {
		const char* method = method_value->u.string.ptr;
		if (!strcmp(method, "initialize")) {
			response = get_initialization_response(val);
		}
		else if (!strcmp(method, "initialized")) {
			Logger->log(log_info, "initialized");
		}
		else if (!strcmp(method, "textDocument/didChange")
		|| !strcmp(method, "textDocument/didOpen")
		|| !strcmp(method, "textDocument/didSave")){
			update_document(self, val);
			response = get_validation_response(self, val);
		}
		else if (!strcmp(method, "textDocument/completion")) {
			response = get_completion_response(self, val);
		}
		else if (!strcmp(method, "textDocument/semanticTokens/range")
				|| !strcmp(method, "textDocument/semanticTokens/full/delta")
				|| !strcmp(method, "textDocument/semanticTokens/full")) {
			response = get_tokens_response(self, val);
		}else if(!strcmp(method, "textDocument/hover")){
			response = get_hover_response(self, val);
		}
		else if(!strcmp(method, "completionItem/resolve")){
			response = get_completion_resolve_response(self, val);
		}else if(!strcmp(method, "textDocument/definition") || !strcmp(method, "textDocument/declaration")){
			response = get_declaration_response(self, val);
		}
		else if (!strcmp(method, "shutdown") || !strcmp(method, "exit")) {
			return 0;
		}
	}

	if (response->type!=json_null)
		send_message(self->send_message_buf, response);

	json_value_free(response);
	json_value_free(val);

	return 1;
}

json_value* load_config_file(char* config_file) {

	FILE* f = fopen(config_file, "rb");

	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* config_str = malloc(fsize+1);
	fread(config_str, 1, fsize, f);
	fclose(f);

	config_str[fsize] = 0;

	return json_parse(config_str, fsize);
}

language_server* create_language_server(char* config_file) {
	language_server* ls = malloc(sizeof(language_server));

	ls->process = process;
	ls->send_message_buf = malloc(MAX_LSP_MESSAGE_SIZE);
	ls->config = load_config_file(config_file);
	ls->parser = init_parser(ls->config);
	ls->documents = create_map(STRING);

	return ls;
}

void free_document_item(document *doc){
	free(doc->text);
	free_wtree(doc->variables);
	free_token_items(doc->tokens);
	free(doc);
}

void free_language_server(language_server* self) {
	free(self->send_message_buf);

	lvector* docs = self->documents->entry_list(self->documents);
	iterator *it = docs->iterator(docs);
	while(it->has_next(it)){
		map_entry* entry = it->get_next(it);
		free_document_item(entry->val);
	}

	free_map(self->documents);
	free_parser(self->parser);
	json_value_free(self->config);
	free(self);
}