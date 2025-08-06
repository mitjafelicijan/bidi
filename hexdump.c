#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <input_file> <array_name>\n", argv[0]);
		return 1;
	}

	const char *filename = argv[1];
	const char *array_name = argv[2];

	FILE *f = fopen(filename, "rb");
	if (!f) {
		perror("fopen");
		return 1;
	}

	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	rewind(f);

	unsigned char *buffer = malloc(size);
	if (!buffer) {
		perror("malloc");
		fclose(f);
		return 1;
	}

	if (fread(buffer, 1, size, f) != (size_t)size) {
		perror("fread");
		free(buffer);
		fclose(f);
		return 1;
	}

	// Generate include guard
	printf("#ifndef %s_H\n", array_name);
	printf("#define %s_H\n\n", array_name);

	printf("unsigned char %s[] = {\n", array_name);
	for (long i = 0; i < size; i++) {
		if (i % 12 == 0) printf("\t");
		printf("0x%02x", buffer[i]);
		if (i != size - 1) printf(", ");
		if ((i + 1) % 12 == 0) printf("\n");
	}
	if (size % 12 != 0) printf("\n");
	printf("};\n");
	printf("unsigned int %s_len = %ld;\n", array_name, size);

	printf("\n#endif // %s_H\n", array_name);

	free(buffer);
	fclose(f);
	return 0;
}
