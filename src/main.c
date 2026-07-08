#include "parser.h"
#include <stdio.h>
#include <string.h>

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
