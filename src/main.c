#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int in_braces;
	int in_key;
	int in_value;

	int valid;
} parser_struct_t;

int parse_whitespace(int idx, char *buffer, int n, parser_struct_t *state) {
	while (idx < n &&
		   (buffer[idx] == ' ' || buffer[idx] == '\n' || buffer[idx] == '\t')) {
		idx++;
	}
	return idx;
}

int parse_value(int idx, char *buffer, int n, parser_struct_t *state) {
	state->in_value = 1;
	while (idx < n && buffer[idx] != '\"' && buffer[idx] != ',' &&
		   buffer[idx] != '}') {
		idx++;
	}
	state->in_value = 0;
	return idx;
}

int parse_key(int idx, char *buffer, int n, parser_struct_t *state) {
	if (!state->in_key) {
		idx++;
		state->in_key = 1;
	}

	while (idx < n && buffer[idx] != '\"') {
		idx++;
	}
	idx = parse_whitespace(idx, buffer, n, state);

	if (idx < n && buffer[idx] == ':') {
		state->in_key = 0;
		idx = parse_whitespace(idx, buffer, n, state);
		if (idx < n && buffer[idx] == '\"') {
			idx = parse_value(idx, buffer, n, state);
		}
	}

	return idx;
}

void parse(char *buffer, int n, parser_struct_t *state) {
	for (int i = 0; i < n; i++) {
		if (buffer[i] == '{') {
			state->in_braces++;
		} else if (buffer[i] == '}') {
			state->in_braces--;
			if (state->in_braces < 0) {
				state->valid = 1;
				break;
			}
			state->valid = 0;
		} else if (state->in_braces && buffer[i] == '\"') {
			i = parse_key(i, buffer, n, state);
		} else if (state->in_key) {
			i = parse_key(i, buffer, n, state);
			i--;
		} else if (buffer[i] == ' ' || buffer[i] == '\n' || buffer[i] == '\t') {
			i = parse_whitespace(i, buffer, n, state);
			i--;
		} else {
			state->valid = 1;
		}
	}
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
	int valid = 0, has_data = 0;
	parser_struct_t *state = calloc(1, sizeof(parser_struct_t));
	while ((n = fread(buffer, 1, sizeof(buffer), fptr)) > 0) {
		parse(buffer, n, state);
		valid = state->valid;
		has_data = 1;
	}
	free(state);

	if (has_data && valid == 0) {
		printf("Valid JSON\n");
	} else {
		printf("Invalid JSON\n");
	}

	return valid;
}
