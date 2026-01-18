#ifndef SLANG_LEX_H
#define SLANG_LEX_H

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include "error.c"
#include "string.c"

#define Token_List(X)\
    X(Eof,        0,    "<eof>")\
    X(Add,        '+',  "+")\
    X(Sub,        '-',  "-")\
    X(Mul,        '*',  "*")\
    X(Div,        '/',  "/")\
    X(Mod,        '%',  "%")\
    X(And,        '&',  "&")\
    X(Or,         '|',  "|")\
    X(Assign,     '=',  "=")\
    X(Lparen,     '(',  "(")\
    X(Rparen,     ')',  ")")\
    X(Comma,      ',',  ",")\
    X(Semicolon,  ';',  ";")\
    X(Int,        256,  "integer")\
    X(Ident,      266,  "identifier")\
    X(Unknown,    512,  "unknown")

#define Token_Enum(suffix, value, name) Token_##suffix = value,
#define Token_String(suffix, value, name) [Token_##suffix] = name,

typedef enum {
    Token_List(Token_Enum)
} Token_Kind;

typedef enum {
    Character_Other = 0,
    Character_Numeric = 1,
    Character_Alphabet = 2,
    Character_Underscore = 4,
    Character_Whitespace = 8,
    Character_Alphanumeric = Character_Numeric | Character_Alphabet,
    Character_Ident_Start = Character_Alphabet | Character_Underscore,
    Character_Ident_Continue = Character_Alphanumeric | Character_Underscore
} Character_Kind;

const uint32_t character_kind[256] = {
    ['_'] = Character_Underscore,
    ['0'] = Character_Numeric,
    ['1'] = Character_Numeric,
    ['2'] = Character_Numeric,
    ['3'] = Character_Numeric,
    ['4'] = Character_Numeric,
    ['5'] = Character_Numeric,
    ['6'] = Character_Numeric,
    ['7'] = Character_Numeric,
    ['8'] = Character_Numeric,
    ['9'] = Character_Numeric,
    ['a'] = Character_Alphabet,
    ['b'] = Character_Alphabet,
    ['c'] = Character_Alphabet,
    ['d'] = Character_Alphabet,
    ['e'] = Character_Alphabet,
    ['f'] = Character_Alphabet,
    ['g'] = Character_Alphabet,
    ['h'] = Character_Alphabet,
    ['i'] = Character_Alphabet,
    ['j'] = Character_Alphabet,
    ['k'] = Character_Alphabet,
    ['l'] = Character_Alphabet,
    ['m'] = Character_Alphabet,
    ['n'] = Character_Alphabet,
    ['o'] = Character_Alphabet,
    ['p'] = Character_Alphabet,
    ['q'] = Character_Alphabet,
    ['r'] = Character_Alphabet,
    ['s'] = Character_Alphabet,
    ['t'] = Character_Alphabet,
    ['u'] = Character_Alphabet,
    ['v'] = Character_Alphabet,
    ['w'] = Character_Alphabet,
    ['x'] = Character_Alphabet,
    ['y'] = Character_Alphabet,
    ['z'] = Character_Alphabet,
    ['A'] = Character_Alphabet,
    ['B'] = Character_Alphabet,
    ['C'] = Character_Alphabet,
    ['D'] = Character_Alphabet,
    ['E'] = Character_Alphabet,
    ['F'] = Character_Alphabet,
    ['G'] = Character_Alphabet,
    ['H'] = Character_Alphabet,
    ['I'] = Character_Alphabet,
    ['J'] = Character_Alphabet,
    ['K'] = Character_Alphabet,
    ['L'] = Character_Alphabet,
    ['M'] = Character_Alphabet,
    ['N'] = Character_Alphabet,
    ['O'] = Character_Alphabet,
    ['P'] = Character_Alphabet,
    ['Q'] = Character_Alphabet,
    ['R'] = Character_Alphabet,
    ['S'] = Character_Alphabet,
    ['T'] = Character_Alphabet,
    ['U'] = Character_Alphabet,
    ['V'] = Character_Alphabet,
    ['W'] = Character_Alphabet,
    ['X'] = Character_Alphabet,
    ['Y'] = Character_Alphabet,
    ['Z'] = Character_Alphabet,
    ['\n'] = Character_Whitespace,
    ['\r'] = Character_Whitespace,
    ['\t'] = Character_Whitespace,
    [' '] = Character_Whitespace
};

const char* token_display_name[] = {
    Token_List(Token_String)
};

typedef struct {
    size_t pos, col, lineno;
} File_Coordinates;

typedef struct {
	Token_Kind kind;
    File_Coordinates st, ed;
	union {
		int64_t integer;
        String identifier;
	};
} Token;

typedef struct {
    size_t col, lineno, pos;
    const char *filename;
    const char *line;
    const char *st, *curr;
} Lexer;

Lexer lexer_from_file(FILE *file, const char *name);
Token next_token(Lexer *lexer);
void print_token(Token token);

#ifdef SLANG_LEX_C

Lexer lexer_from_file(FILE *file, const char* name) {
    assert(file != NULL);

	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* file_buff = malloc(file_size + 1);
	file_buff[file_size] = '\0';

	fread(file_buff, file_size, 1, file);

    return (Lexer) {
        .col = 1, .lineno = 1, .pos = 0,
        .filename = name, 
        .line = file_buff,
        .st = file_buff, .curr = file_buff
    };
}

#define next() (lexer->col++, lexer->pos++, lexer->curr++)
#define newline() (lexer->lineno++, lexer->col = 0, lexer->line = lexer->st + lexer->pos + 1)
#define curr() (lexer->curr[0])
#define record_st() (token.st.pos = lexer->pos, token.st.lineno = lexer->lineno, token.st.col = lexer->col)
#define record_ed() (token.ed.pos = lexer->pos - 1, token.ed.lineno = lexer->lineno, token.ed.col = lexer->col - 1)

Token next_token(Lexer* lexer) {
    Token token;
    int64_t i;

    while (character_kind[curr()] & Character_Whitespace) {
        if (curr() == '\n') {
            newline();
        }
        next();
    }

    switch (curr()) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            record_st();
            i = 0;
            token.kind = Token_Int;

            while (character_kind[curr()] & Character_Numeric) {
                i = i * 10 + (curr() - '0');
                next();
            }
            token.integer = i;
            record_ed();
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
            record_st();
            next();

            while (character_kind[curr()] & Character_Ident_Continue) {
                next();
            }

            record_ed();

            token.kind = Token_Ident;
            token.identifier = from_slice(lexer->st, token.st.pos, token.ed.pos);

            break;
        case '+': case '-': case '*': case '/': case '%':
        case '=': case '&': case '|': case ')': case '(':
        case ',': case ';':
            record_st();
            token.kind = curr();
            next();
            record_ed();

            break;
        case '\0':
            record_st();
            token.kind = Token_Eof;
            next();
            record_st();
            break;
        default:
            print_error(lexer->filename, lexer->lineno, lexer->col,
                    "found unknown character %c", curr());

            print_line_with_pointer(lexer->line, lexer->lineno, lexer->col, lexer->col);
            exit(1);

            break;

    }

	return token;
}

#undef next
#undef newline
#undef curr

void print_token(Token token) {
    printf("Token[%s;%zd:%zd (%zd) - %zd:%zd (%zd)]", token_display_name[token.kind], 
            token.st.lineno, token.st.col, token.st.pos,
            token.ed.lineno, token.ed.col, token.ed.pos);
    switch(token.kind) {
        case Token_Int:
            printf(": %ld", token.integer);
            break;
        case Token_Ident:
            printf(": %s", token.identifier.str);
            break;
    }

    printf("\n");
}

#endif // SLANG_LEX_C 
#endif // SLANG_LEX_H
