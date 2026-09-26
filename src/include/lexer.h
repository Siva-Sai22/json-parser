#pragma once

#include "token.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct {
	bool in_string;
	bool in_bool_or_null;
	bool in_number;
	
	char data[4096];
	size_t data_len;

	token_t *tokens;
} lexer;

void lexer_init(lexer *lexer);
int lexer_feed(lexer *lexer, char *buffer, int n);
void lexer_free_token(token_t *token);
