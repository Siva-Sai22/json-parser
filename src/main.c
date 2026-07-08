#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
	EXPECT_START,
	EXPECT_KEY_OR_END,
	EXPECT_KEY,
	EXPECT_COLON,
	EXPECT_VALUE,
	EXPECT_COMMA_OR_END,
	END
} parser_mode_t;

typedef enum {
	NULL_T,
	BOOLEAN,
	NUMBER,
	STRING,
	OPENING_BRACE,
	CLOSING_BRACE,
	COLON,
	COMMA
} token_type_t;

typedef struct token_t token_t;

struct token_t {
	token_type_t type;
	union {
		int boolean_value;
		double number_value;
		char *string_value;
	} value;

	token_t *next;
};

typedef struct {
	int in_string;
	char string[4096];
	size_t string_len;

	token_t *tokens;
} lexer;

typedef struct {
	lexer lexer;
	parser_mode_t mode;
	int depth;
	int error;
} parser_state_t;

void parser_process_token(parser_state_t *parser, token_t **token) {
	switch (parser->mode) {
	case EXPECT_START:
		if ((*token)->type == OPENING_BRACE) {
			parser->mode = EXPECT_KEY_OR_END;
		} else {
			parser->error = 1;
		}
		break;
	case EXPECT_KEY_OR_END:
		if ((*token)->type == STRING) {
			parser->mode = EXPECT_COLON;
		} else if ((*token)->type == CLOSING_BRACE) {
			parser->mode = END;
		} else {
			parser->error = 1;
		}
		break;
	case EXPECT_KEY:
		if ((*token)->type == STRING) {
			parser->mode = EXPECT_COLON;
		} else {
			parser->error = 1;
		}
		break;
	case EXPECT_COLON:
		if ((*token)->type == COLON) {
			parser->mode = EXPECT_VALUE;
		} else {
			parser->error = 1;
		}
		break;
	case EXPECT_VALUE:
		if ((*token)->type == STRING || (*token)->type == NUMBER ||
			(*token)->type == BOOLEAN || (*token)->type == NULL_T) {
			parser->mode = EXPECT_COMMA_OR_END;
		} else {
			parser->error = 1;
		}
		break;
	case EXPECT_COMMA_OR_END:
		if ((*token)->type == COMMA) {
			parser->mode = EXPECT_KEY;
		} else if ((*token)->type == CLOSING_BRACE) {
			parser->mode = END;
		} else {
			parser->error = 1;
		}
		break;
	default:
		parser->error = 1;
		break;
	}
}

void lexer_token_add(lexer *lexer, token_t *token) {
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

void lexer_feed(lexer *lexer, char *buffer, int n) {
	int idx = 0;
	while (idx < n) {
		if (lexer->in_string) {
			if (buffer[idx] == '\"') {
				lexer->in_string = 0;

				token_t *token = malloc(sizeof(token_t));
				token->type = STRING;
				token->value.string_value = strdup(lexer->string);

				lexer_token_add(lexer, token);
				lexer->string_len = 0;
			} else {
				lexer->string[lexer->string_len++] = buffer[idx];
			}
		} else {
			switch (buffer[idx]) {
			case '{': {
				token_t *token = malloc(sizeof(token_t));
				token->type = OPENING_BRACE;
				lexer_token_add(lexer, token);
				break;
			}
			case '}': {
				token_t *token = malloc(sizeof(token_t));
				token->type = CLOSING_BRACE;
				lexer_token_add(lexer, token);
				break;
			}
			case ':': {
				token_t *token = malloc(sizeof(token_t));
				token->type = COLON;
				lexer_token_add(lexer, token);
				break;
			}
			case ',': {
				token_t *token_comma = malloc(sizeof(token_t));
				token_comma->type = COMMA;
				lexer_token_add(lexer, token_comma);
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

void parser_feed(parser_state_t *parser, char *buffer, int n) {
	lexer_feed(&parser->lexer, buffer, n);

	token_t *token = parser->lexer.tokens;
	while (token != NULL) {
		parser_process_token(parser, &token);

		token_t *temp = token;
		token = token->next;
		free(temp);

		if (parser->error) {
			return;
		}
	}
}

void parser_init(parser_state_t *state) {
	state->lexer.in_string = 0;
	state->lexer.string_len = 0;
	state->lexer.tokens = NULL;
	state->mode = EXPECT_START;
	state->depth = 0;
	state->error = 0;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s [json_file]\n", argv[0]);
		return 1;
	}

	char *ext = strrchr(argv[1], '.');
	if (ext == NULL || strcmp(ext, ".json") != 0) {
		printf("Error: The file must have a .json extension.\n");
		return 1;
	}

	char *path = argv[1];
	FILE *fptr = fopen(path, "r");
	if (fptr == NULL) {
		printf("Error: Could not open file %s\n", path);
		return 1;
	}

	char buffer[1024];
	int n;
	int has_data = 0;

	parser_state_t parser;
	parser_init(&parser);

	while ((n = fread(buffer, 1, sizeof(buffer), fptr)) > 0) {
		parser_feed(&parser, buffer, n);
		has_data = 1;

		if (parser.error) {
			break;
		}
	}

	if (has_data && parser.error == 0) {
		printf("Valid JSON\n");
		return 0;
	} else {
		printf("Invalid JSON\n");
		return 1;
	}
}
