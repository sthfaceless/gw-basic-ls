//
// Created by danil on 01.05.2021.
//

#include "parser.h"

completion_t map_token_type_to_completion(token_t type) {

	static completion_t* mapper = NULL;
	if (mapper == NULL) {

		mapper = malloc(sizeof(completion_t) * 64);

		/*
		 * Default types
		 * */
		mapper[Namespace] = CompletionModule;
		mapper[Type] = CompletionTypeParameter;
		mapper[Enum] = CompletionEnum;
		mapper[Interface] = CompletionInterface;
		mapper[Struct] = CompletionStruct;
		mapper[TypeParameter] = CompletionTypeParameter;
		mapper[Parameter] = CompletionVariable;
		mapper[Variable] = CompletionVariable;
		mapper[Property] = CompletionProperty;
		mapper[EnumMember] = CompletionEnumMember;
		mapper[Event] = CompletionEvent;
		mapper[Function] = CompletionFunction;
		mapper[Method] = CompletionMethod;
		mapper[Macro] = CompletionSnippet;
		mapper[Keyword] = CompletionKeyword;
		mapper[Modifier] = CompletionUnit;
		mapper[Comment] = CompletionText;
		mapper[String] = CompletionText;
		mapper[Number] = CompletionUnit;
		mapper[Regexp] = CompletionText;
		mapper[Operator] = CompletionOperator;
		mapper[Unknown] = CompletionUnknown;
	}

	return mapper[map_type_to_semantic_token(type)];
}

char* map_kind_to_string(token_t type) {

	static char** mapper = NULL;
	if (mapper == NULL) {

		mapper = malloc(sizeof(char*) * 64);

		/*
		 * Default types
		 * */
		mapper[Namespace] = "Namespace";
		mapper[Type] = "Type";
		mapper[Enum] = "Enum";
		mapper[Interface] = "Interface";
		mapper[Struct] = "Struct";
		mapper[TypeParameter] = "TypeParameter";
		mapper[Parameter] = "Parameter";
		mapper[Variable] = "Variable";
		mapper[Property] = "Property";
		mapper[EnumMember] = "EnumMember";
		mapper[Event] = "Event";
		mapper[Function] = "Function";
		mapper[Method] = "Method";
		mapper[Macro] = "Macro";
		mapper[Keyword] = "Keyword";
		mapper[Modifier] = "Modifier";
		mapper[Comment] = "Comment";
		mapper[String] = "String";
		mapper[Number] = "Number";
		mapper[Regexp] = "Regexp";
		mapper[Operator] = "Operator";
		mapper[Unknown] = "Unknown";
	}

	return mapper[map_type_to_semantic_token(type)];
}

static completionItem* get_completion_item(const char* name, const token_t kind, gwkeyword* keyword) {

	completionItem* item = malloc(sizeof(completionItem));
	item->name = strupper(copystr(name));
	item->kind = map_token_type_to_completion(kind);
	item->detail = map_kind_to_string(kind);
	item->keyword = keyword;
	return item;
}

void free_completion_item(completionItem* item) {

	free(item->name);
	free(item);
}

void free_completion_items(vector* items) {

	for (int i = 0; i < items->size; ++i) {
		free_completion_item(items->get(items, i));
	}
	free_vector_no_values(items);
}

static vector* completion_bfs(wtree* tree, char* str) {

	int root = tree->find_ind(tree, str);
	if (!root)
		return create_vector_reserved(0);

	lvector* queue = create_lvector();
	vector* vect = create_vector();
	queue->push(queue, &root);
	while (queue->size) {
		int vertex = *(int*)queue->get_last(queue);
		queue->pop(queue);
		if (vertex != root && tree->terminate[vertex]) {
			gwkeyword* keyword = tree->terminate[vertex];
			vect->push(vect, get_completion_item(keyword->name, keyword->kind, keyword));
		}
		lvector* edges = tree->edges[vertex];
		iterator* it = edges->iterator(edges);
		while (it->has_next(it))
			queue->push(queue, (int*)it->get_next(it));
	}
	free_lvector(queue);
	free(str);

	return vect;
}

static vector* make_completions(gwparser* self, document* doc, int line, int pos) {

	vector* completions = NULL;
	vector* tokens = make_tokens_with_lines(doc->tokens);
	vector* tokens_line = tokens->get(tokens, line);

	if (tokens_line) {
		/*
		 * pos-1 указывает на конец слова, которое хотим продлить
		 * */
		pos = pos - 1;
		if (pos >= 0) {
			token* tkn = find_token(tokens_line, pos);
			if (tkn && tkn->l <= pos && pos <= tkn->r) {
				vector* keywords_completions =
					completion_bfs(self->_tokenizer->keywords, substr(tkn->str, 0, pos - tkn->l));
				vector* variables_completions = completion_bfs(doc->variables, substr(tkn->str, 0, pos - tkn->l));
				completions = merge_vectors(keywords_completions, variables_completions);
			}
		}
		else {
			int prev_line_number = 0;
			int prev_line = line - 1;
			while (prev_line >= 0) {
				tokens_line = tokens->get(tokens, prev_line);
				if (tokens_line->size) {
					token* tkn = tokens_line->get(tokens_line, 0);
					if (tkn->kind == Number) {
						prev_line_number = atoi(tkn->str);
						break;
					}
				}
				prev_line--;
			}

			int next_line = line + 1;
			int next_line_number = -1;
			while (next_line < tokens->size) {
				tokens_line = tokens->get(tokens, next_line);
				if (tokens_line->size) {
					token* tkn = tokens_line->get(tokens_line, 0);
					if (tkn->kind == Number) {
						next_line_number = atoi(tkn->str);
						break;
					}
				}
				next_line++;
			}

			int line_number =
				next_line_number == -1 ? (prev_line_number / 10 + 1) * 10 : (prev_line_number + next_line_number) >> 1;
			char index_str[16];
			sprintf(index_str, "%d", line_number);

			completions = create_vector_reserved(4);
			completions->push(completions, get_completion_item(index_str, Number, NULL));
			if (next_line_number == -1) {
				gwkeyword* run_sentence_keyword = self->_tokenizer->keywords->find(self->_tokenizer->keywords, "run");
				completions
					->push(completions, get_completion_item(run_sentence_keyword->name, Keyword, run_sentence_keyword));
			}
		}
	}
	free_tokens(tokens);

	return completions ? completions : create_vector_reserved(0);
}

static vector* validate(gwparser* self, document* doc) {

	if (!*doc->text)
		return create_vector_reserved(0);

	vector* messages = create_vector();
	vector* tokens = make_tokens_with_lines(doc->tokens);
	for (int i = 0; i < tokens->size; ++i) {

		vector* tokens_line = tokens->get(tokens, i);

		/*
		 * Too long message
		 * */
		int last_index = 0;
		if (tokens_line->size) {
			last_index = ((token*)tokens_line->last(tokens_line))->r;
			if (last_index > 255) {
				messages->push(messages, too_long_line(i, last_index));
			}
		}

		/*
		 * Begin with not number
		 * */
		if (tokens_line->size) {
			token* first_line_token = tokens_line->get(tokens_line, 0);
			if (first_line_token->kind != LineNumber && first_line_token->kind != RunStatement)
				messages->push(messages, must_begin_number(i, last_index));
			else if (atoll(first_line_token->str) > 65529)
				messages->push(messages, begin_number_too_large(i, first_line_token->l, first_line_token->r));
		}
	}
	free_tokens(tokens);

	return messages;
}

hoverItem* create_hover_item(gwkeyword* keyword, range* _range) {

	hoverItem* item = malloc(sizeof(item));
	item->keyword = keyword;
	item->_range = _range;
	return item;
}

void free_hover_item(hoverItem* self) {

	free(self->_range);
	free(self);
}

hoverItem* get_hover_item(gwparser* self, document* doc, int line, int pos) {

	token* tok = find_token_on_line(doc->tokens, line, pos);

	if (tok) {
		gwkeyword* result = self->_tokenizer->keywords->find(self->_tokenizer->keywords, tok->str);
		if (!result) result = doc->variables->find(doc->variables, tok->str);

		if (result) {
			return create_hover_item(result,
				create_range_object(tok->line_l, tok->l, tok->line_r, tok->r));
		}
	}

	return NULL;
}

static range* get_declaration(gwparser* self, document* doc, int line, int pos) {

	token* tok = find_token_on_line(doc->tokens, line, pos);

	if (tok) {
		gwkeyword* result = doc->variables->find(doc->variables, tok->str);
		if (result)
			return create_range_object(tok->line_l, tok->l, tok->line_r, tok->r);
	}

	return NULL;
}

gwparser* init_parser(json_value* config) {

	gwparser* self = malloc(sizeof(gwparser));

	self->_tokenizer = create_tokenizer(config);
	self->make_completions = make_completions;
	self->validate = validate;
	self->get_hover_item = get_hover_item;
	self->get_declaration = get_declaration;

	return self;
}

void free_parser(gwparser* self) {

	free_tokenizer(self->_tokenizer);
	free(self);
}

