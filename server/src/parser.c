//
// Created by danil on 01.05.2021.
//

#include "parser.h"

static completionItem* get_completion_item(const char *name,const token_t kind){
	completionItem *item = malloc(sizeof (completionItem));
	item->name = copystr(name);
	item->kind = kind;
	return item;
}
void free_completion_item(completionItem* item){
	free(item->name);
	free(item);
}
void free_completion_items(vector* items){
	for (int i = 0; i<items->size; ++i) {
		free_completion_item(items->get(items, i));
	}
	free_vector_no_values(items);
}

static vector* completion_bfs(wtree* tree, char* str) {

	int root = tree->find_ind(tree, str);
	if (root==-1)
		return create_vector_reserved(0);

	int strend = strlen(str)-1;

	lvector* queue = create_lvector();
	vector* vect = create_vector();
	queue->push(queue, &root);
	while (queue->size) {
		int vertex = *(int*)queue->get_last(queue);
		queue->pop(queue);
		if (vertex!=root && tree->terminate[vertex]) {
			gwkeyword *keyword = tree->terminate[vertex];
			vect->push(vect, get_completion_item(keyword->name, keyword->kind));
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

static vector* make_completions(gwparser* self, char* str, int line, int pos) {

	vector* completions = NULL;
	vector* tokens = self->_tokenizer->make_tokens_with_lines(self->_tokenizer, str);
	vector* tokens_line = tokens->get(tokens, line);

	if (tokens_line) {
		/*
		 * pos-1 указывает на конец слова, которое хотим продлить
		 * */
		pos = pos-1;
		if (pos>=0) {
			token* tkn = find_token(tokens_line, pos);
			if (tkn && tkn->l<=pos && pos<=tkn->r)
				completions = completion_bfs(self->_tokenizer->keywords, substr(tkn->str, 0, pos-tkn->l));
		}
		else {
			int prev_line_number = 0;
			int prev_line = line-1;
			while (prev_line>=0) {
				tokens_line = tokens->get(tokens, prev_line);
				if (tokens_line->size) {
					token* tkn = tokens_line->get(tokens_line, 0);
					if (tkn->kind==Number) {
						prev_line_number = atoi(tkn->str);
						break;
					}
				}
				prev_line--;
			}

			int next_line = line+1;
			int next_line_number = -1;
			while (next_line<tokens->size) {
				tokens_line = tokens->get(tokens, next_line);
				if (tokens_line->size) {
					token* tkn = tokens_line->get(tokens_line, 0);
					if (tkn->kind==Number) {
						next_line_number = atoi(tkn->str);
						break;
					}
				}
				next_line++;
			}

			int line_number =
					next_line_number==-1 ? (prev_line_number/10+1)*10 : (prev_line_number+next_line_number) >> 1;

			char index_str[16];
			sprintf(index_str, "%d", line_number);
			token* tok = create_token(index_str, 0, 0, 0, 0);
			tok->kind = Number;

			completions = create_vector_from(1, get_completion_item(index_str, Number));
		}
	}
	free_tokens(tokens);

	return completions ? completions : create_vector_reserved(0);
}

vector* validate(gwparser* self, char* text) {

	if (!strlen(text))
		return create_vector_reserved(0);

	vector* messages = create_vector();

	vector* tokens = self->_tokenizer->make_tokens_with_lines(self->_tokenizer, text);
	for (int i = 0; i<tokens->size; ++i) {

		vector* tokens_line = tokens->get(tokens, i);

		int last_index = 0;
		if (tokens_line->size) {
			last_index = ((token*)tokens_line->last(tokens_line))->r;
			if (last_index>255) {
				messages->push(messages, too_long_line(i, last_index));
			}
		}

		if (!tokens_line->size || ((token*)tokens_line->get(tokens_line, 0))->kind!=Number) {
			messages->push(messages, must_begin_number(i, last_index));
		}

	}
	free_tokens(tokens);

	return messages;
}


gwparser* init_parser(json_value* config) {
	gwparser* self = malloc(sizeof(gwparser));

	self->_tokenizer = create_tokenizer(config);
	self->make_completions = make_completions;
	self->validate = validate;

	return self;
}

void free_parser(gwparser* self) {
	free_tokenizer(self->_tokenizer);
	free(self);
}

