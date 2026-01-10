#ifndef SLANG_LEX_H
#define SLANG_LEX_H

#include <stdint.h>
#include <stdlib.h>

#include "data.c"

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
    Token_Xor,
    Token_Not,

    // conditional operators
    Token_Cnot,
    Token_Cand,
    Token_Cor,
    Token_Lt,
    Token_Le,
    Token_Gt,
    Token_Ge,
    Token_Eq,
    Token_Neq,

    // assignment operators
    Token_Assign,
    Token_Add_Assign,
    Token_Sub_Assign,
    Token_Mul_Assign,
    Token_Div_Assign,
    Token_Mod_Assign,
    Token_And_Assign,
    Token_Or_Assign,
    Token_Xor_Assign,

    // grouping operators
	Token_Lparen,
	Token_Rparen,
    Token_Lbracket,
    Token_Rbracket,
    Token_Lbrace,
    Token_Rbrace,
    Token_Comma,
    Token_Dot,
    Token_Semicolon,
    Token_Colon,

    // literals
	Token_Int,
    Token_String,
    Token_Decimal,

    // identifiers
    Token_Identifier,

    // keywords
    Token_Fn,
    Token_Data,
    Token_Return,
    Token_Loop,
    Token_Break,
    Token_Continue,
    Token_In,


	Token_Count
} Token_Kind;

const size_t ASSIGN_OFFSET = Token_Add_Assign - Token_Add;

const Token_Kind char_map[256] = {
    ['+'] = Token_Add,
    ['-'] = Token_Sub,
    ['*'] = Token_Mul,
    ['/'] = Token_Div,
    ['%'] = Token_Mod,
    ['&'] = Token_And,
    ['|'] = Token_Or,
    ['^'] = Token_Xor,
    ['~'] = Token_Not,
    ['='] = Token_Eq,
    ['('] = Token_Lparen,
    [')'] = Token_Rparen,
    ['['] = Token_Lbracket,
    [']'] = Token_Rbracket,
    ['{'] = Token_Lbrace,
    ['}'] = Token_Rbrace,
    [','] = Token_Comma,
    ['.'] = Token_Dot,
    [';'] = Token_Semicolon,
    [';'] = Token_Colon
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
    [Token_Xor] = "^",
    [Token_Not] = "~",

    // conditional operators
    [Token_Cand] = "&&",
    [Token_Cor] = "||",
    [Token_Lt] = "<",
    [Token_Le] = "<=",
    [Token_Gt] = ">",
    [Token_Ge] = ">=",
    [Token_Eq] = "==",
    [Token_Neq] = "!=",

    // assignment operators
    [Token_Assign] = "=",
    [Token_Add_Assign] = "+=",
    [Token_Sub_Assign] = "-=",
    [Token_Mul_Assign] = "*=",
    [Token_Div_Assign] = "/=",
    [Token_Mod_Assign] = "%=",
    [Token_And_Assign] = "&=",
    [Token_Or_Assign] = "|=",
    [Token_Xor_Assign] = "^=",

    // grouping operators
    [Token_Lparen] = "(",
    [Token_Rparen] = ")",
    [Token_Lbracket] = "[",
    [Token_Rbracket] = "]",
    [Token_Lbrace] = "{",
    [Token_Rbrace] = "}",
    [Token_Comma] = ",",
    [Token_Dot] = ".",
    [Token_Semicolon] = ";",
    [Token_Colon] = ":",

    // keywords
    [Token_Fn] = "fn",
    [Token_Data] = "data",
    [Token_Return] = "return",
    [Token_Loop] = "loop",
    [Token_Break] = "break",
    [Token_Continue] = "continue",
    [Token_In] = "in"
};

typedef struct {
	Token_Kind kind;
	union {
		int64_t integer;
        String identifier;
        String string;
        double decimal;
	};
} Token;

typedef struct {
	Token* buff;
	size_t sz;
	size_t cp;
} Token_Array;

typedef struct {
    size_t col, line, pos;
    const char *filename;
    const char *st, *curr;
} Lexer;

Lexer create_lexer(const char* filename);
Token_Array lex(Lexer* lexer);
void print_token(Token token);
Token_Array token_array_new();
void token_array_push(Token_Array* array, Token token);

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
Token_Array lex(Lexer* lexer) {

	Token_Array tokens;
	Token token;
    size_t st, ed;
    int64_t i;
    double d, e;

	tokens = token_array_new();

    while (lex_curr()) {
        switch (lex_curr()) {
            case '\n':
                lex_line();
            case ' ': case '\t': case '\r':
                lex_next();
                break;

            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                i = 0;
                d = 0.0;
                e = 0.1;
                token.kind = Token_Int;

                while (lex_curr() >= '0' && lex_curr() <= '9') {
                    i = i * 10 + (lex_curr() - '0');
                    lex_next();
                }
                token.integer = i;

                if (lex_curr() == '.') {
                    lex_next();

                    while (lex_curr() >= '0' && lex_curr() <= '9') {
                        d += (lex_curr() - '0') * e;
                        e = e * 0.1;
                        lex_next();
                    }

                    d += (double) i;

                    token.kind = Token_Decimal;
                    token.decimal = d;
                }

                token_array_push(&tokens, token);
                break;

            case '"':
                lex_next();
                st = lexer->pos;
                ed = 0;

                while (lex_curr() != '"') {
                    if (lex_curr() == '\\') {
                        lex_next();
                    }
                    lex_next();
                }

                ed = lexer->pos - 1;
                lex_next();

                token.kind = Token_String;
                token.string = from_slice(lexer->st, st, ed);

                token_array_push(&tokens, token);
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

                if (strcmp(token.string.str, "fn") == 0) {
                    token.kind = Token_Fn;
                } else if (strcmp(token.string.str, "data") == 0) {
                    token.kind = Token_Data;
                } else if (strcmp(token.string.str, "return") == 0) {
                    token.kind = Token_Return;
                } else if (strcmp(token.string.str, "loop") == 0) {
                    token.kind = Token_Loop;
                } else if (strcmp(token.string.str, "break") == 0) {
                    token.kind = Token_Break;
                } else if (strcmp(token.string.str, "continue") == 0) {
                    token.kind = Token_Continue;
                } else if (strcmp(token.string.str, "in") == 0) {
                    token.kind = Token_In;
                }
                token_array_push(&tokens, token);
                break;

            case '+': case '-': case '*': case '/': case '%':
                token.kind = char_map[lex_curr()];
                lex_next();

                if (lex_curr() == '=') {
                    token.kind += ASSIGN_OFFSET;
                    lex_next();
                }
                token_array_push(&tokens, token);
                break;
            case '~':
                token.kind = Token_Not;
                lex_next();
                token_array_push(&tokens, token);
                break;
            case '!':
                token.kind = Token_Cnot;
                lex_next();

                if (lex_curr() == '=') {
                    token.kind = Token_Neq;
                    lex_next();
                }
                token_array_push(&tokens, token);
                break;
            case '=':
                token.kind = Token_Assign;
                lex_next();

                if (lex_curr() == '=') {
                    token.kind = Token_Eq;
                    lex_next();
                }
                token_array_push(&tokens, token);
                break;
            case '&':
                token.kind = Token_And;
                lex_next();
                if (lex_curr() == '=') {
                    token.kind = Token_And_Assign;
                    lex_next();
                } else if (lex_curr() == '&') {
                    token.kind = Token_Cand;
                    lex_next();
                }
                token_array_push(&tokens, token);
                break;
            case '|':
                token.kind = Token_Or;
                lex_next();
                if (lex_curr() == '=') {
                    token.kind = Token_Or_Assign;
                    lex_next();
                } else if (lex_curr() == '|') {
                    token.kind = Token_Cor;
                    lex_next();
                }
                token_array_push(&tokens, token);
                break;
            case '^':
                token.kind = Token_Xor;
                lex_next();
                if (lex_curr() == '=') {
                    token.kind = Token_Xor_Assign;
                    lex_next();
                } 
                token_array_push(&tokens, token);
                break;
            case '<':
                token.kind = Token_Lt;
                lex_next();
                if (lex_curr() == '=') {
                    token.kind = Token_Le;
                    lex_next();
                }
                token_array_push(&tokens, token);
                break;
            case '>':
                token.kind = Token_Gt;
                lex_next();
                if (lex_curr() == '=') {
                    token.kind = Token_Ge;
                    lex_next();
                }
                token_array_push(&tokens, token);
                break;

            case '(': case ')': case '[': case ']': case '{': 
            case '}': case '.': case ',': case ';': case ':':
                token.kind = char_map[lex_curr()];
                lex_next();
                token_array_push(&tokens, token);
                break;
        }

    }

	token.kind = Token_Eof;

	token_array_push(&tokens, token);

	return tokens;
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
        case Token_Xor: printf("TOKEN[Xor]\n"); break;
        case Token_Not: printf("TOKEN[Not]\n"); break;

        case Token_Cnot: printf("TOKEN[Cnot]\n"); break;
        case Token_Cand: printf("TOKEN[Cand]\n"); break;
        case Token_Cor: printf("TOKEN[Cor]\n"); break;
        case Token_Lt: printf("TOKEN[Lt]\n"); break;
        case Token_Le: printf("TOKEN[Le]\n"); break;
        case Token_Gt: printf("TOKEN[Gt]\n"); break;
        case Token_Ge: printf("TOKEN[Ge]\n"); break;
        case Token_Eq: printf("TOKEN[Eq]\n"); break;
        case Token_Neq: printf("TOKEN[Neq]\n"); break;

        case Token_Assign: printf("TOKEN[Assign]\n"); break;
        case Token_Add_Assign: printf("TOKEN[Add_Assign]\n"); break;
        case Token_Sub_Assign: printf("TOKEN[Sub_Assign]\n"); break;
        case Token_Mul_Assign: printf("TOKEN[Mul_Assign]\n"); break;
        case Token_Div_Assign: printf("TOKEN[Div_Assign]\n"); break;
        case Token_Mod_Assign: printf("TOKEN[Mod_Assign]\n"); break;
        case Token_And_Assign: printf("TOKEN[And_Assign]\n"); break;
        case Token_Or_Assign: printf("TOKEN[Or_Assign]\n"); break;
        case Token_Xor_Assign: printf("TOKEN[Xor_Assign]\n"); break;

        case Token_Lparen: printf("TOKEN[Lparen]\n"); break;
        case Token_Rparen: printf("TOKEN[Rparen]\n"); break;
        case Token_Lbracket: printf("TOKEN[Lbracket]\n"); break;
        case Token_Rbracket: printf("TOKEN[Rbracket]\n"); break;
        case Token_Lbrace: printf("TOKEN[Lbrace]\n"); break;
        case Token_Rbrace: printf("TOKEN[Rbrace]\n"); break;
        case Token_Comma: printf("TOKEN[Comma]\n"); break;
        case Token_Dot: printf("TOKEN[Dot]\n"); break;
        case Token_Semicolon: printf("TOKEN[Semicolon]\n"); break;
        case Token_Colon: printf("TOKEN[Colon]\n"); break;


        case Token_Int: printf("TOKEN[Int] = %ld\n", token.integer); break;
        case Token_String: printf("TOKEN[String] = %s\n", token.string.str); break;
        case Token_Decimal: printf("TOKEN[Decimal] = %lf\n", token.decimal); break;

        case Token_Identifier: printf("TOKEN[Identifier] = %s\n", token.identifier.str); break;

        case Token_Fn: printf("TOKEN[Fn]\n"); break;
        case Token_Data: printf("TOKEN[Data]\n"); break;
        case Token_Return: printf("TOKEN[Return]\n"); break;
        case Token_Loop: printf("TOKEN[Loop]\n"); break;
        case Token_Break: printf("TOKEN[Break]\n"); break;
        case Token_Continue: printf("TOKEN[Continue]\n"); break;
        case Token_In: printf("TOKEN[In]\n"); break;
        default:
			printf("Token[Unknown]\n");
			break;
	}
}

Token_Array token_array_new() {
	return (Token_Array) {
		.buff = (Token*) malloc(sizeof(Token) * 8),
		.sz = 0,
		.cp = 8
	};
}


void token_array_push(Token_Array* array, Token token) {
	if (array->sz == array->cp) { 
		array->cp *= 2;
		array->buff = (Token*) realloc(array->buff, sizeof(Token) * array->cp);
	}

	array->buff[array->sz++] = token;
}

#endif // SLANG_LEX_C 
#endif // SLANG_LEX_H
