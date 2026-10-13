#include "parser.h"
#include "lexer.h"
#include "token.h"

void parser_init(parser_state_t *state) {
	lexer_init(&state->lexer);
	state->mode = EXPECT_OPENING_BRACE;
	state->depth = 0;
	state->error = 0;
}

static void parser_process_opening_brace(parser_state_t *parser,
										 token_t **token) {
	if ((*token)->type == OPENING_BRACE) {
		parser->mode = EXPECT_KEY_OR_CLOSING_BRACE;
		parser->depth++;
	} else {
		parser->error = 1;
	}
}

static void parser_process_key_or_closing_brace(parser_state_t *parser,
												token_t **token) {
	if ((*token)->type == STRING) {
		parser->mode = EXPECT_COLON;
	} else if ((*token)->type == CLOSING_BRACE) {
		parser->depth--;
		if (parser->depth == 0) {
			parser->mode = END;
		} else {
			parser->mode = EXPECT_COMMA_OR_CLOSING_BRACE;
		}
	} else {
		parser->error = 1;
	}
}

static void parser_process_key(parser_state_t *parser, token_t **token) {
	if ((*token)->type == STRING) {
		parser->mode = EXPECT_COLON;
	} else {
		parser->error = 1;
	}
}

static void parser_process_colon(parser_state_t *parser, token_t **token) {
	if ((*token)->type == COLON) {
		parser->mode = EXPECT_VALUE_OR_OPENING_BRACKET_OR_OPENING_BRACE;
	} else {
		parser->error = 1;
	}
}

static void
parser_process_value_or_opening_bracket_or_opening_brace(parser_state_t *parser,
														 token_t **token) {
	if ((*token)->type == STRING || (*token)->type == NUMBER ||
		(*token)->type == BOOLEAN || (*token)->type == NULL_T) {
		parser->mode = EXPECT_COMMA_OR_CLOSING_BRACE;
	} else if ((*token)->type == OPENING_BRACKET) {
		parser->mode = EXPECT_VALUE_IN_ARRAY_OR_CLOSING_BRACKET;
	} else if ((*token)->type == OPENING_BRACE) {
		parser->mode = EXPECT_KEY_OR_CLOSING_BRACE;
		parser->depth++;
	} else {
		parser->error = 1;
	}
}

static void
parser_process_value_in_array_or_closing_bracket(parser_state_t *parser,
												 token_t **token) {
	if ((*token)->type == STRING || (*token)->type == NUMBER ||
		(*token)->type == BOOLEAN || (*token)->type == NULL_T) {
		parser->mode = EXPECT_COMMA_OR_CLOSING_BRACKET;
	} else if ((*token)->type == CLOSING_BRACKET) {
		parser->mode = EXPECT_COMMA_OR_CLOSING_BRACE;
	} else {
		parser->error = 1;
	}
}

static void parser_process_value_in_array(parser_state_t *parser,
										  token_t **token) {
	if ((*token)->type == STRING || (*token)->type == NUMBER ||
		(*token)->type == BOOLEAN || (*token)->type == NULL_T) {
		parser->mode = EXPECT_COMMA_OR_CLOSING_BRACKET;
	} else {
		parser->error = 1;
	}
}

static void parser_process_comma_or_closing_bracket(parser_state_t *parser,
													token_t **token) {
	if ((*token)->type == COMMA) {
		parser->mode = EXPECT_VALUE_IN_ARRAY;
	} else if ((*token)->type == CLOSING_BRACKET) {
		parser->mode = EXPECT_COMMA_OR_CLOSING_BRACE;
	} else {
		parser->error = 1;
	}
}

static void parser_process_comma_or_closing_brace(parser_state_t *parser,
												  token_t **token) {
	if ((*token)->type == COMMA) {
		parser->mode = EXPECT_KEY;
	} else if ((*token)->type == CLOSING_BRACE) {
		parser->depth--;
		if (parser->depth == 0) {
			parser->mode = END;
		} else {
			parser->mode = EXPECT_COMMA_OR_CLOSING_BRACE;
		}
	} else {
		parser->error = 1;
	}
}

static void parser_process_token(parser_state_t *parser, token_t **token) {
	switch (parser->mode) {
	case EXPECT_OPENING_BRACE:
		parser_process_opening_brace(parser, token);
		break;
	case EXPECT_KEY_OR_CLOSING_BRACE:
		parser_process_key_or_closing_brace(parser, token);
		break;
	case EXPECT_KEY:
		parser_process_key(parser, token);
		break;
	case EXPECT_COLON:
		parser_process_colon(parser, token);
		break;
	case EXPECT_VALUE_OR_OPENING_BRACKET_OR_OPENING_BRACE:
		parser_process_value_or_opening_bracket_or_opening_brace(parser, token);
		break;
	case EXPECT_VALUE_IN_ARRAY_OR_CLOSING_BRACKET:
		parser_process_value_in_array_or_closing_bracket(parser, token);
		break;
	case EXPECT_VALUE_IN_ARRAY:
		parser_process_value_in_array(parser, token);
		break;
	case EXPECT_COMMA_OR_CLOSING_BRACKET:
		parser_process_comma_or_closing_bracket(parser, token);
		break;
	case EXPECT_COMMA_OR_CLOSING_BRACE:
		parser_process_comma_or_closing_brace(parser, token);
		break;
	default:
		parser->error = 1;
		break;
	}
}

void parser_feed(parser_state_t *parser, char *buffer, int n) {
	int res = lexer_feed(&parser->lexer, buffer, n);
	if (res == 1) {
		parser->error = 1;
		return;
	}

	token_t *token = parser->lexer.tokens;
	while (token != NULL) {
		parser_process_token(parser, &token);

		token_t *temp = token;
		token = token->next;
		lexer_free_token(temp);

		if (parser->error) {
			return;
		}
	}
}
