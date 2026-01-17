#ifndef SLANG_PARSE_H
#define SLANG_PARSE_H

#include <assert.h>

#include "array.c"
#include "lex.c"

typedef struct Ast_Expression Ast_Expression;

typedef Array(Ast_Expression*) Ast_Expression_Array;

typedef struct { 
    Ast_Expression_Array statements;
} Ast_Module;


typedef enum {
    Ast_Statement_Expression,
    Ast_Statement_Count
} Ast_Statement_Kind;

struct Ast_Statement {
    Ast_Statement_Kind kind;
    union {
        Ast_Expression* expression;
    };
};

typedef enum {
    Ast_Binary = 0, 
    Ast_Literal,
    Ast_Identifier,
    Ast_Function_Call,
    Ast_Count
} Ast_Expression_Kind;

typedef enum {
    Ast_Binary_Assign = Token_Assign,
	Ast_Binary_Add = Token_Add,
	Ast_Binary_Sub = Token_Sub,
	Ast_Binary_Mul = Token_Mul, 
	Ast_Binary_Div = Token_Div,
    Ast_Binary_Mod = Token_Mod,
    Ast_Binary_And = Token_And,
    Ast_Binary_Or = Token_Or
} Ast_Binary_Kind;

typedef struct {
    Ast_Binary_Kind kind;
    Ast_Expression* left;
    Ast_Expression* right;
} Ast_Expression_Binary;

typedef struct {
    String identifier;
    Ast_Expression_Array parameters;
} Ast_Expression_Function_Call;

typedef enum {
    Ast_Expression_Integer = Token_Int,
} Ast_Expression_Literal_Kind;

typedef struct {
    Ast_Expression_Literal_Kind kind;
    union {
        int64_t integer;
    }; 
} Ast_Expression_Literal;


struct Ast_Expression {
    Ast_Expression_Kind kind;
    union {
        Ast_Expression_Binary binary;
        Ast_Expression_Function_Call function_call;
        String identifier;
        Ast_Expression_Literal literal;
    };
};

typedef struct {
    Token current;
    Lexer lexer;
} Parser;


Parser create_parser(Lexer lex);
Ast_Module parse_module(Parser* parser);

#ifdef SLANG_PARSE_C

Parser create_parser(Lexer lex) {
    Parser parser = (Parser) {0};
    parser.current = next_token(&lex);
    parser.lexer = lex;
    
    return parser;
}

Ast_Expression* new_binary_operator(Token_Kind kind, Ast_Expression* left, Ast_Expression* right) {
    Ast_Expression* node = calloc(sizeof(Ast_Expression), 1);
    node->kind = Ast_Binary;
    node->binary.kind = kind;
    node->binary.left = left;
    node->binary.right = right;

    return node;
}

Ast_Expression* new_literal(Token token) {
    Ast_Expression* node = calloc(sizeof(Ast_Expression), 1);
    node->kind = Ast_Literal;
    node->literal.kind = token.kind;
    node->literal.integer = token.integer;

    return node;
}

Ast_Expression* new_identifier(Token token) {
    Ast_Expression* node = calloc(sizeof(Ast_Expression), 1);
    node->kind = Ast_Identifier;
    node->identifier = token.identifier;

    return node;
}


Token parser_current_token(Parser *parser) {
    return parser->current;
}

void parser_next_token(Parser *parser) {
    parser->current = next_token(&parser->lexer);
}

void parser_expect_token(Parser *parser, Token_Kind kind) {
    assert(parser->current.kind == kind);
    parser_next_token(parser);
}

int parser_is_token(Parser* parser, Token_Kind kind) {
    return parser_current_token(parser).kind == kind;
}


#define curr() (parser_current_token(parser))
#define next() (parser_next_token(parser))
#define expect(kind) (parser_expect_token(parser, kind))
#define is(kind) (parser_is_token(parser, kind))

Ast_Expression* parse_expression(Parser *parser);

Ast_Expression* parse_leaf(Parser *parser) {
    Ast_Expression* node;

    switch (curr().kind) {
        case Token_Int:
            node = new_literal(curr());
            next();
            break;
        case Token_Ident:
            Token token = curr();
            next();

            node = new_identifier(token);

            if (is(Token_Lparen)) {
                next();
                node->kind = Ast_Function_Call;
                node->function_call.identifier = token.identifier;

                int expect_comma = 0;

                while (!is(Token_Rparen)) {
                    if (expect_comma) {
                        expect(Token_Comma);
                    }
                    Ast_Expression* param = parse_expression(parser);
                    array_push_back(&node->function_call.parameters, param);

                    expect_comma = 1;
                }
                expect(Token_Rparen);

            }
            break;
        default:
            fprintf(stderr, "[ERROR]: expected either Integer Or Identifier got something else.\n");
            exit(1);
    }

    return node;

}


Ast_Expression* parse_paren(Parser *parser) {
    Token token;
    Ast_Expression* node;

    token = curr();

    switch (token.kind) {
        case Token_Lparen:
            next();
            node = parse_expression(parser);
            expect(Token_Rparen);
            break;
        default:
            node = parse_leaf(parser);
            break;
    }

    return node;
}

Ast_Expression* parse_and(Parser* parser) {
    Ast_Expression *left, *right;
    left = parse_paren(parser);

    while (curr().kind == Token_And) {
        next();

        right = parse_paren(parser);

        left = new_binary_operator(Token_And, left, right);
    }

    return left;
}


Ast_Expression* parse_or(Parser* parser) {
    Ast_Expression *left, *right;
    left = parse_and(parser);

    while (curr().kind == Token_Or) {
        next();

        right = parse_and(parser);

        left = new_binary_operator(Token_Or, left, right);
    }

    return left;
}


Ast_Expression* parse_mul(Parser* parser) {
    Token operator;
    Ast_Expression *left, *right;
    left = parse_or(parser);

    while (curr().kind == Token_Mul || curr().kind == Token_Div || curr().kind == Token_Mod) {
        operator = curr();
        next();

        right = parse_or(parser);

        left = new_binary_operator(operator.kind, left, right);
    }

    return left;
}

Ast_Expression* parse_add(Parser* parser) {
    Token operator;
    Ast_Expression *left, *right;
    left = parse_mul(parser);

    while (curr().kind == Token_Add || curr().kind == Token_Sub) {
        operator = curr();
        next();

        right = parse_mul(parser);

        left = new_binary_operator(operator.kind, left, right);
    }

    return left;
}

Ast_Expression* parse_assign(Parser *parser) {
    Token operator;
    Ast_Expression *left, *right;
    left = parse_add(parser);

    if (curr().kind == Token_Assign) {
        next();

        right = parse_add(parser);

        left = new_binary_operator(Token_Assign, left, right);
    }

    return left;
}


Ast_Expression* parse_expression(Parser* parser) {
    return parse_assign(parser);
}



Ast_Module parse_module(Parser* parser) {
    Ast_Module module = (Ast_Module) {0};

    while (!is(Token_Eof)) {
        while (is(Token_Semicolon)) {
            next();
        }
        Ast_Expression* expression = parse_expression(parser);

        array_push_back(&module.statements, expression);

        expect(Token_Semicolon);
    }

    return module;
}

void print_expression(Ast_Expression* exp, int tabs) {
    for (int i = 0; i < tabs; ++i) {
        printf("    ");
    }
    char *str;
    switch (exp->kind) {
        case Ast_Binary:
            switch (exp->binary.kind) {
                case Ast_Binary_Assign:
                    str = "ASSIGN";
                    break;
                case Ast_Binary_Add:
                    str = "ADD";
                    break;
                case Ast_Binary_Sub:
                    str = "SUB";
                    break;
                case Ast_Binary_Mul:
                    str = "MUL";
                    break;
                case Ast_Binary_Div:
                    str = "DIV";
                    break;
                case Ast_Binary_Mod:
                    str = "MOD";
                    break;
                case Ast_Binary_And:
                    str = "AND";
                    break;
                case Ast_Binary_Or:
                    str = "OR";
                    break;
                default:
                    str = "UNKNOWN";
                    break;
            }

            printf("[BINARY; %s]\n", str);
            print_expression(exp->binary.left, tabs + 1);
            print_expression(exp->binary.right, tabs + 1);

            break;
        case Ast_Literal:
            printf("[LITERAL; %ld]\n", exp->literal.integer);
            break;
        case Ast_Identifier:
            printf("[IDENTIFIER; %s]\n", exp->identifier.str);
            break;
        case Ast_Function_Call:
            printf("[FUNCTION_CALL; %s]\n", exp->function_call.identifier.str);

            for (size_t i = 0; i < exp->function_call.parameters.sz; ++i) {
                print_expression(exp->function_call.parameters.arr[i], tabs + 1);
            }
            break;
        default:
            fprintf(stderr, "UNKNOWN expression");
            exit(1);
    }
}

void print_module(Ast_Module module) {
    printf("[MODULE] name=module\n");

    for (size_t i = 0; i < module.statements.sz; ++i) {
        print_expression(module.statements.arr[i], 1);
    }
}


#undef curr
#undef next
#undef expect
#undef is

#endif // SLANG_PARSE_C

#endif // SLANG_PARSE_H
