#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define SLANG_LEX_C
#define SLANG_PARSE_C
#define STRING_C

#include "lex.c"
#include "parse.c"
#include "string.c"

int main(int argc, char* argv[]) {
	char *filename;
	size_t count, i;
    Lexer lexer;
    Parser parser;
	FILE* fp;
    Token token;

	if (argc != 2) {
		fprintf(stderr, "[Usage]: slang <filename>\n");
        exit(1);
	}

	filename = argv[1]; 

    lexer = create_lexer(filename);
    parser = create_parser(lexer);


    Ast_Module module = parse_module(&parser);

    print_module(module);

	return 0;
}
	
