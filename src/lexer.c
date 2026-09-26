#include "lexer.h"
#include "token.h"
#include <stdlib.h>
#include <string.h>

static void lexer_add_token(lexer *lexer, token_t *token) {
	if (lexer->tokens == NULL) {
		lexer->tokens = token;
	} else {
		token_t *temp = lexer->tokens;
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = token;
	}
}

void lexer_free_token(token_t *token) {
	if (token == NULL) {
		return;
	}

	if (token->type == STRING) {
		free(token->value.string_value);
	}

	free(token);
}

static token_t *lexer_create_token(lexer *lexer, token_type_t type) {
	token_t *token = malloc(sizeof(token_t));
	token->type = type;
	return token;
}

void lexer_init(lexer *lexer) {
	lexer->in_string = 0;
	lexer->data_len = 0;
	lexer->tokens = NULL;
}

static void lexer_manage_in_string(lexer *lexer, char c) {
	if (c == '\"') {
		lexer->in_string = 0;

		token_t *token = lexer_create_token(lexer, STRING);
		token->value.string_value = strdup(lexer->data);
		lexer_add_token(lexer, token);

		lexer->data_len = 0;
	} else {
		lexer->data[lexer->data_len++] = c;
	}
}

static int lexer_manage_bool_null(lexer *lexer, char c) {
	lexer->in_bool_or_null = 1;
	lexer->data[lexer->data_len++] = c;

	if (lexer->data_len == 4) {
		if (strncmp(lexer->data, "true", 4) == 0) {
			token_t *token = lexer_create_token(lexer, BOOLEAN);
			token->value.boolean_value = 1;
			lexer_add_token(lexer, token);

			lexer->data_len = 0;
			lexer->in_bool_or_null = 0;

			return 0;
		} else if (strncmp(lexer->data, "null", 4) == 0) {
			token_t *token = lexer_create_token(lexer, NULL_T);
			lexer_add_token(lexer, token);

			lexer->data_len = 0;
			lexer->in_bool_or_null = 0;

			return 0;
		}
	} else if (lexer->data_len == 5 && strncmp(lexer->data, "false", 5) == 0) {
		token_t *token = lexer_create_token(lexer, BOOLEAN);
		token->value.boolean_value = 0;
		lexer_add_token(lexer, token);

		lexer->data_len = 0;
		lexer->in_bool_or_null = 0;

		return 0;
	}

	if (lexer->data_len > 5) {
		return 1;
	}

	return 0;
}

static void lexer_manage_number(lexer *lexer, char c, int *idx) {
	lexer->in_number = 1;

	if ((c >= '0' && c <= '9') || c == '.') {
		lexer->data[lexer->data_len++] = c;
	} else {
		lexer->data[lexer->data_len] = '\0';

		token_t *token = lexer_create_token(lexer, NUMBER);
		token->value.number_value = atof(lexer->data);
		lexer_add_token(lexer, token);

		lexer->data_len = 0;
		lexer->in_number = 0;

		(*idx)--; // Decrement idx to reprocess the current character
	}
}

int lexer_feed(lexer *lexer, char *buffer, int n) {
	int idx = 0, res = 0;
	while (idx < n) {
		if (lexer->in_string) {
			lexer_manage_in_string(lexer, buffer[idx]);
		} else if (lexer->in_bool_or_null) {
			res = lexer_manage_bool_null(lexer, buffer[idx]);
		} else if (lexer->in_number) {
			lexer_manage_number(lexer, buffer[idx], &idx);
		} else if ((buffer[idx] >= 'a' && buffer[idx] <= 'z') ||
				   (buffer[idx] >= 'A' && buffer[idx] <= 'Z')) {
			res = lexer_manage_bool_null(lexer, buffer[idx]);
		} else if (buffer[idx] >= '0' && buffer[idx] <= '9') {
			lexer_manage_number(lexer, buffer[idx], &idx);
		} else {
			switch (buffer[idx]) {
			case '{': {
				token_t *token = lexer_create_token(lexer, OPENING_BRACE);
				lexer_add_token(lexer, token);
				break;
			}
			case '}': {
				token_t *token = lexer_create_token(lexer, CLOSING_BRACE);
				lexer_add_token(lexer, token);
				break;
			}
			case ':': {
				token_t *token = lexer_create_token(lexer, COLON);
				lexer_add_token(lexer, token);
				break;
			}
			case ',': {
				token_t *token = lexer_create_token(lexer, COMMA);
				lexer_add_token(lexer, token);
				break;
			}
			case '\"': {
				lexer->in_string = 1;
				lexer->data_len = 0;
				break;
			}
			case ' ':
			case '\n':
			case '\r':
			case '\t':
				break;
			default: {
				res = 1;
				break;
			}
			}
		}
		idx++;
		if (res == 1) {
			return res;
		}
	}

	return res;
}
