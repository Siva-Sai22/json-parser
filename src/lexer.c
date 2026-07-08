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

void lexer_feed(lexer *lexer, char *buffer, int n) {
	int idx = 0;
	while (idx < n) {
		if (lexer->in_string) {
			if (buffer[idx] == '\"') {
				lexer->in_string = 0;

				token_t *token = lexer_create_token(lexer, STRING);
				token->value.string_value = strdup(lexer->string);
				lexer_add_token(lexer, token);

				lexer->string_len = 0;
			} else {
				lexer->string[lexer->string_len++] = buffer[idx];
			}
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
			case '\"':
				lexer->in_string = 1;
				lexer->string_len = 0;
				break;
			default:
				break;
			}
		}
		idx++;
	}
}
