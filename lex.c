#ifndef SLANG_LEX_H
#define SLANG_LEX_H

#include <stdint.h>
#include <stdlib.h>

#include "string.c"

typedef enum {
	Token_Eof = 0,

    // arithmetic operators
	Token_Add,
	Token_Sub,
	Token_Mul, 
	Token_Div,
    Token_Mod,

    // boolean operators
    Token_And,
    Token_Or,

    // assign
    Token_Assign,

    // structure
	Token_Lparen,
	Token_Rparen,
    Token_Semicolon,

    // literals
	Token_Int,

    // identifiers
    Token_Identifier,

	Token_Count
} Token_Kind;

const Token_Kind char_map[256] = {
    ['+'] = Token_Add,
    ['-'] = Token_Sub,
    ['*'] = Token_Mul,
    ['/'] = Token_Div,
    ['%'] = Token_Mod,
    ['&'] = Token_And,
    ['|'] = Token_Or,
    ['='] = Token_Assign,
    ['('] = Token_Lparen,
    [')'] = Token_Rparen,
    [';'] = Token_Semicolon
};


const char *token_strings[Token_Count] = {
    // arithmetic operators
    [Token_Add] = "+",
    [Token_Sub] = "-",
    [Token_Mul] = "*", 
    [Token_Div] = "/",
    [Token_Mod] = "%",

    // boolean operators
    [Token_And] = "&",
    [Token_Or] = "|",

    // assignment operators
    [Token_Assign] = "=",

    // grouping operators
    [Token_Lparen] = "(",
    [Token_Rparen] = ")",
    [Token_Semicolon] = ";",
};

typedef struct {
	Token_Kind kind;
	union {
		int64_t integer;
        String identifier;
	};
} Token;

typedef struct {
    size_t col, line, pos;
    const char *filename;
    const char *st, *curr;
} Lexer;

Lexer create_lexer(const char* filename);
Token next_token(Lexer* lexer);
void print_token(Token token);

#ifdef SLANG_LEX_C

Lexer create_lexer(const char* filename) {
	FILE *fp = fopen(filename, "r");

	if (fp == NULL) {
		fprintf(stderr, "[ERROR] File %s doesn't exist\n", filename);
	}

	fseek(fp, 0, SEEK_END);
	long file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* file_buff = malloc(file_size + 1);
	file_buff[file_size] = '\0';

	fread(file_buff, file_size, 1, fp);

    return (Lexer) {
        .col = 1, .line = 1, .pos = 0,
        .filename = filename, 
        .st = file_buff, .curr = file_buff
    };
}

#define lex_next() (lexer->col++, lexer->pos++, lexer->curr++)
#define lex_line() (lexer->line++, lexer->col = 0)
#define lex_curr() (lexer->curr[0])
Token next_token(Lexer* lexer) {

    Token token;
    size_t st, ed;
    int64_t i;

    while (lex_curr() == ' ' || lex_curr() == '\t' || lex_curr() == '\r' || lex_curr() == '\n') {
        if (lex_curr() == '\n') {
            lex_line();
        }
        lex_next();
    }

    switch (lex_curr()) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            i = 0;
            token.kind = Token_Int;

            while (lex_curr() >= '0' && lex_curr() <= '9') {
                i = i * 10 + (lex_curr() - '0');
                lex_next();
            }
            token.integer = i;
            break;

        case '_': case 'a': case 'b': case 'c': case 'd': 
        case 'e': case 'f': case 'g': case 'h': case 'i':
        case 'j': case 'k': case 'l': case 'm': case 'n':
        case 'o': case 'p': case 'q': case 'r': case 's':
        case 't': case 'u': case 'v': case 'w': case 'x':
        case 'y': case 'z': case 'A': case 'B': case 'C':
        case 'D': case 'E': case 'F': case 'G': case 'H': 
        case 'I': case 'J': case 'K': case 'L': case 'M':
        case 'N': case 'O': case 'P': case 'Q': case 'R':
        case 'S': case 'T': case 'U': case 'V': case 'W':
        case 'X': case 'Y': case 'Z': 
            st = lexer->pos;
            ed = 0;

            lex_next();

            while (  lex_curr() == '_' || 
                    (lex_curr() >= 'a' && lex_curr() <= 'z') ||
                    (lex_curr() >= 'A' && lex_curr() <= 'Z') ||
                    (lex_curr() >= '0' && lex_curr() <= '9')) {
                lex_next();
            }

            ed = lexer->pos - 1;

            token.kind = Token_Identifier;
            token.identifier = from_slice(lexer->st, st, ed);
            break;

        case '+': case '-': case '*': case '/': case '%':
        case '=': case '&': case '|': case ')': case '(':
        case ';':
            token.kind = char_map[lex_curr()];
            lex_next();

            break;
        case '\0':
            token.kind = Token_Eof;
            break;
        default:
            fprintf(stderr, "ERROR: Found unknown char %c at line %zd col %zd\n", lex_curr(), lexer->line, lexer->col);
            break;

    }


	return token;
}

#undef lex_next
#undef lex_line
#undef lex_curr

void print_token(Token token) {
	switch (token.kind) {
        case Token_Eof: printf("TOKEN[Eof]\n"); break;

        case Token_Add: printf("TOKEN[Add]\n"); break;
        case Token_Sub: printf("TOKEN[Sub]\n"); break;
        case Token_Mul: printf("TOKEN[Mul]\n"); break; 
        case Token_Div: printf("TOKEN[Div]\n"); break;
        case Token_Mod: printf("TOKEN[Mod]\n"); break;

        case Token_And: printf("TOKEN[And]\n"); break;
        case Token_Or: printf("TOKEN[Or]\n"); break;


        case Token_Assign: printf("TOKEN[Assign]\n"); break;

        case Token_Lparen: printf("TOKEN[Lparen]\n"); break;
        case Token_Rparen: printf("TOKEN[Rparen]\n"); break;
        case Token_Semicolon: printf("TOKEN[Semicolon]\n"); break;

        case Token_Int: printf("TOKEN[Int] = %ld\n", token.integer); break;

        case Token_Identifier: printf("TOKEN[Identifier] = %s\n", token.identifier.str); break;

        default:
			printf("Token[Unknown]\n");
			break;
	}
}

#endif // SLANG_LEX_C 
#endif // SLANG_LEX_H
