#pragma once
#include "lexer.h"

typedef enum {
	EXPECT_OPENING_BRACE,
	EXPECT_KEY_OR_CLOSING_BRACE,
	EXPECT_KEY,
	EXPECT_COLON,
	EXPECT_VALUE_OR_OPENING_BRACKET_OR_OPENING_BRACE,
	EXPECT_VALUE_IN_ARRAY_OR_CLOSING_BRACKET,
	EXPECT_VALUE_IN_ARRAY,
	EXPECT_COMMA_OR_CLOSING_BRACKET,
	EXPECT_COMMA_OR_CLOSING_BRACE,
} parser_mode_t;

typedef struct {
	lexer lexer;
	parser_mode_t mode;
	int depth;
	int error;
} parser_state_t;

void parser_init(parser_state_t *state);
void parser_feed(parser_state_t *parser, char *buffer, int n);
