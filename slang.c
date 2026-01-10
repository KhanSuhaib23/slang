#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define SLANG_LEX_C
#define SLANG_PARSE_C
#define SLANG_DATA_C

#include "lex.c"
#include "parse.c"
#include "data.c"

int main(int argc, char* argv[]) {
	char *filename;
	size_t count, i;
    Lexer lexer;
	FILE* fp;
	Token_Array tokens;
    Ast_Node* node;

	if (argc != 2) {
		fprintf(stderr, "[Usage]: slang <filename>\n");
        exit(1);
	}

	filename = argv[1]; 

    lexer = create_lexer(filename);

	tokens = lex(&lexer);

	for (i = 0; i < tokens.sz; ++i) {
		print_token(tokens.buff[i]);
	}

	return 0;
}
	
