#pragma once

typedef enum {
	NULL_T,
	BOOLEAN,
	NUMBER,
	STRING,
	OPENING_BRACE,
	OPENING_BRACKET,
	CLOSING_BRACKET,
	CLOSING_BRACE,
	COLON,
	COMMA
} token_type_t;

typedef enum {
    STACK_OPENING_BRACE = OPENING_BRACE,
    STACK_OPENING_BRACKET = OPENING_BRACKET
} container_type_t;

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
