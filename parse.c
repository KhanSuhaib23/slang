#ifndef SLANG_PARSE_H
#define SLANG_PARSE_H

#include "array.c"
#include "data.c"
#include "lex.c"

typedef struct Ast_Definition Ast_Definition;
typedef struct Ast_Statement Ast_Statement;
typedef struct Ast_If Ast_If;

typedef Array(Ast_Statement*) Ast_Statement_Array;
typedef Array(Ast_Definition*) Ast_Definition_Array;
typedef Array(String) String_Array;

typedef struct { 
    Ast_Definition_Array definitions;
} Ast_Module;

typedef enum {
    Ast_Definition_Function = 0,
    Ast_Definition_Data,
    Ast_Definition_Literal,
    Ast_Definition_Count
} Ast_Definition_Kind;

typedef struct {
    String_Array parameters;
    Ast_Statement_Array statements;
} Ast_Function;

typedef struct {
    String_Array fields;
} Ast_Data;

struct Ast_Definition {
    Ast_Definition_Kind kind;
    String name;
    union {
        Ast_Function function;
        Ast_Data data;
    };
};

typedef enum {
    Ast_Statement_Assign,
    Ast_Statement_Loop,
    Ast_Statement_Condition,
    Ast_Statement_Expression,
    Ast_Statement_Break,
    Ast_Statement_Continue,
    Ast_Statement_Count
} Ast_Statement_Kind;

typedef struct {
    Ast_Expression* lhs;
    Ast_Expression* rhs;
} Ast_Assign;

typedef struct {
    Ast_Statement* init;
    Ast_Expression* end;
    Ast_Expression* update;
    Ast_Statement_Array statements;
} Ast_Loop;

typedef struct {
    Ast_Expression* condition;
    Ast_Statement_Array statements;
} Ast_If;

typedef struct {
    Ast_If* iff;
    Ast_If_Array elifs;
    Ast_Statement_Array els;
} Ast_Condition;


struct Ast_Statement {
    Ast_Statement_Kind kind;
    union {
        Ast_Assign assign;
        Ast_Loop loop;
        Ast_Condition condition;
        Ast_Expression* expression;
    };
};

typedef enum {
    Ast_Expression_Binary = 0, 
    Ast_Expression_Unary,
    Ast_Expression_Function_Call,
    Ast_Expression_Accessor,
    Ast_Expression_Count
} Ast_Expression_Kind;

typedef enum {
    Ast_Unary_Negative = Token_Sub,
    Ast_Unary_Not = Token_Not,
    Ast_Unary_Cnot = Token_Cnot
} Ast_Unary_Kind;

typedef struct {
    Ast_Unary_Kind kind;
    Ast_Expression* expression;
} Ast_Expression_Unary;

typedef enum {
	Ast_Binary_Add = Token_Add,
	Ast_Binary_Sub = Token_Sub,
	Ast_Binary_Mul = Token_Mul, 
	Ast_Binary_Div = Token_Div,
    Ast_Binary_Mod = Token_Mod,

    // boolean operators
    Ast_Binary_And = Token_And,
    Ast_Binary_Or = Token_Or,
    Ast_Binary_Xor = Token_Xor,
    Ast_Binary_Not = Token_Not,

    // conditional operators
    Ast_Binary_Cnot = Token_Cnot,
    Ast_Binary_Cand = Token_Cand,
    Ast_Binary_Cor = Token_Cor,
    Ast_Binary_Lt = Token_Lt,
    Ast_Binary_Le = Token_Le,
    Ast_Binary_Gt = Token_Gt,
    Ast_Binary_Ge = Token_Ge,
    Ast_Binary_Eq = Token_Eq,
    Ast_Binary_Neq = Token_Neq
} Ast_Binary_Kind;

typedef struct {
    Ast_Binary_Kind kind;
    Ast_Expression* left;
    Ast_Expression* right;
} Ast_Expression_Binary;

typedef struct {
    Ast_Expression* function;
    Ast_Expression_Array parameters;
} Ast_Expression_Function_Call;

typedef enum {
    Ast_Expression_Integer = Token_Int,
    Ast_Expression_String = Token_String,
    Ast_Expression_Decimal = Token_Decimal
} Ast_Expression_Literal_Kind;

typedef struct {
    Ast_Expression_Literal_Kind kind;
    union {
        String string;
        int64_t integer;
        double decimal;
    }; 
} Ast_Expression_Literal;


struct Ast_Expression {
    Ast_Expression_Kind kind;
    union {
        Ast_Expression_Unary unary;
        Ast_Expression_Binary binary;
        Ast_Expression_Function_Call function_call;
        String identifier;
        Ast_Expression_Literal;
};

typedef struct {
    Token* current;
    Token* start;
    size_t index;
} Parser;


Ast_Module parse_module(Parser* parser);

#ifdef SLANG_PARSE_C

int parser_consume_if(Parser* parser, Token_Kind kind) {
    if (parser->current->kind == kind) {
        parser->current++;
        parser->pos++;

        return 1;
    }

    return 0;
}

Token parser_is(Parser* parser, Token_Kind kind) {
    return parser->current->kind == kind;
}

Token parser_assert_is(Parser* parser, Token_Kind kind) {
    assert(parser->current->kind == kind);
    Token token = *parser->current;
    parser->current++;
    parser->pos++;
    return token;
}

void parser_consume(Parser* parser) {
    parser->current++;
    parser->pos++;
}

Ast_Statement parse_loop(Parser* parser) {
}

Ast_Statement parse_statement(Parser* parser) {
    if (parser_is(parser, Token_Loop)) {
    } else if (parser_is(parser, Token_If)) {
    } else {
    }

} 

Ast_Statement_Array parse_block(Parser* parser) {
    Ast_Statement_Array statements = (Ast_Statement_Array) {0};
    parser_assert_is(parser, Token_Lbrace);

    while (!parser_is(parser, Token_Rbrace)) {
        Ast_Statement statement = parse_statement(parser);
        array_push_back(&statements, statement);
    }

    return statements;
}

Ast_Module parse_module(Parser* parser) {
    switch (parser->current-kind) {
        case Token_Fn:
            Ast_Definition function_definition = (Ast_Definition) {0};
            function_definition.kind = Ast_Definition_Function;

            parser_consume(parser);
            Token name = parser_assert_is(parser, Token_Identifier);
            function_definition.name = name.identifier;

            parser_assert_is(parser, Token_Lparen);

            while (!parser_consume_if(parser, Token_Rparen)) {
                name = parser_assert_is(parser, Token_Identifier);

                array_push_back(&function_definition.function.parameters, name.identifier);

                parser_assert_is(parser, Token_Comma);
            }



    }
}

Ast_Node* new_binary_operator(Token_Kind kind, Ast_Node* left, Ast_Node* right) {
    Ast_Node* node = malloc(sizeof(Ast_Node));
    node->kind = Ast_Operator;
    node->binary_operator.kind = operator_map[kind];
    node->binary_operator.left = left;
    node->binary_operator.right = right;

    return node;
}

Ast_Node* parse_expression(Token_Array* tokens);

Ast_Node* parse_paren(Token_Array* tokens) {
    Token token;
    Ast_Node* node;

    token = tokens->buff[0];

    switch (token.kind) {
        case Token_Lparen:
            tokens->buff++;
            node = parse_expression(tokens);
            token = tokens->buff[0];

            if (token.kind != Token_Rparen) {
                fprintf(stderr, "[ERROR]: Expected Rparen, got something else.\n");
                exit(1);
            }

            tokens->buff++;
            break;
        case Token_Int:
            tokens->buff++;
            node = new_integer(token.integer);
            break;
        default:
            if (token.kind != Token_Rparen) {
                fprintf(stderr, "[ERROR]: Expected Lparen or Integer, got something else.\n");
                exit(1);
            }
    }

    return node;
}

Ast_Node* parse_mul_div(Token_Array* tokens) {
    Token operator;
    Ast_Node *left, *right;
    left = parse_paren(tokens);

    while (tokens->buff[0].kind == Token_Mul || tokens->buff[0].kind == Token_Div) {
        operator = tokens->buff[0];
        tokens->buff++;

        right = parse_paren(tokens);

        left = new_binary_operator(operator.kind, left, right);
    }

    return left;
}

Ast_Node* parse_add_sub(Token_Array* tokens) {
    Token operator;
    Ast_Node *left, *right;
    left = parse_mul_div(tokens);

    while (tokens->buff[0].kind == Token_Add || tokens->buff[0].kind == Token_Sub) {
        operator = tokens->buff[0];
        tokens->buff++;

        right = parse_mul_div(tokens);

        left = new_binary_operator(operator.kind, left, right);
    }

    return left;
}

Ast_Node* parse_expression(Token_Array* tokens) {
    return parse_add_sub(tokens);
}

Ast_Node* parse(Token_Array tokens) {
    return parse_expression(&tokens);
}

void print_ast_recursive(Ast_Node *node);

void print_operator(Operator_Kind kind) {
    switch (kind) {
        case Operator_Add:
            printf("+");
            break;
        case Operator_Sub:
            printf("-");
            break;
        case Operator_Mul:
            printf("*");
            break;
        case Operator_Div:
            printf("/");
            break;
    }
}

void print_ast_recursive(Ast_Node *node) {
    if (node->kind == Ast_Literal) {
        printf("%ld", node->literal.integer);
    } else if (node->kind == Ast_Operator) {
        printf("(");
        print_ast_recursive(node->binary_operator.left);
        print_operator(node->binary_operator.kind);
        print_ast_recursive(node->binary_operator.right);
        printf(")");
    }

}

void print_ast(Ast_Node* root) {
    print_ast_recursive(root);
    printf("\n");
}

int64_t eval(Ast_Node* node);

int64_t eval(Ast_Node* node) {
    if (node->kind == Ast_Literal) {
        return node->literal.integer;
    } else if (node->kind == Ast_Operator) {
        int64_t left = eval(node->binary_operator.left);
        int64_t right = eval(node->binary_operator.right);

        switch (node->binary_operator.kind) {
            case Operator_Add:
                return left + right;
            case Operator_Sub:
                return left - right;
            case Operator_Mul:
                return left * right;
            case Operator_Div:
                return left / right;
            default:
                fprintf(stderr, "[ERROR]: error evaluating expression.\n");
                exit(1);
        }
    }
}

#endif // SLANG_PARSE_C

#endif // SLANG_PARSE_H
