#include <stdio.h>
#include <string.h>

typedef struct {
	int in_braces;
} parser_struct_t;

parser_struct_t state = {0};

int parse(char *buffer, int n) {
	for (int i = 0; i < n; i++) {
		if (buffer[i] == '{') {
			state.in_braces++;
		} else if (buffer[i] == '}') {
			state.in_braces--;
			if (state.in_braces < 0) {
				return 1;
			}
		} else if (buffer[i] != ' ') {
			return 1;
		}
	}

	return state.in_braces == 0 ? 0 : 1;
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
	while ((n = fread(buffer, 1, sizeof(buffer), fptr)) > 0) {
		valid = parse(buffer, n);
		has_data = 1;
	}

	valid = !has_data;

	if (valid == 0) {
		printf("Valid JSON\n");
	} else {
		printf("Invalid JSON\n");
	}

	return valid;
}
