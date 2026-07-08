#pragma once

#include "token.h"
#include <stdio.h>

typedef struct {
	int in_string;
	char string[4096];
	size_t string_len;

	token_t *tokens;
} lexer;

void lexer_init(lexer *lexer);
void lexer_feed(lexer *lexer, char *buffer, int n);
void lexer_free_token(token_t *token);
