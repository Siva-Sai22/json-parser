#pragma once
#include "lexer.h"

typedef enum {
	EXPECT_START,
	EXPECT_KEY_OR_END,
	EXPECT_KEY,
	EXPECT_COLON,
	EXPECT_VALUE,
	EXPECT_COMMA_OR_END,
	END
} parser_mode_t;

typedef struct {
	lexer lexer;
	parser_mode_t mode;
	int depth;
	int error;
} parser_state_t;

void parser_init(parser_state_t *state);
void parser_feed(parser_state_t *parser, char *buffer, int n);
