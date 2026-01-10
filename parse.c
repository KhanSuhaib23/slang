#ifndef SLANG_PARSE_H
#define SLANG_PARSE_H

#include "lex.c"

typedef struct Ast_Node Ast_Node;

typedef enum {
    Ast_Operator = 0,
    Ast_Literal,
    Ast_Count
} Ast_Node_Kind;

typedef enum {
    Operator_Add = 0,
    Operator_Sub,
    Operator_Mul,
    Operator_Div,
    Operator_Count
} Operator_Kind;

typedef enum {
    Literal_Int = 0,
    Literal_Count
} Literal_Type;

typedef struct {
    Operator_Kind kind;
    Ast_Node* left;
    Ast_Node* right;
} Binary_Operator_Node;

typedef struct {
    Literal_Type type;
    union {
        int64_t integer;
    };
} Literal_Node;

struct Ast_Node {
    Ast_Node_Kind kind;
    union {
        Binary_Operator_Node binary_operator;
        Literal_Node literal;
    };
};

Ast_Node* parse(Token_Array tokens);
void print_ast(Ast_Node* root);

#ifdef SLANG_PARSE_C

Operator_Kind operator_map[Token_Count] = {
    [Token_Add] = Operator_Add,
    [Token_Sub] = Operator_Sub,
    [Token_Mul] = Operator_Mul,
    [Token_Div] = Operator_Div
};

Ast_Node* new_integer(int64_t i) {
    Ast_Node* node = malloc(sizeof(Ast_Node));
    node->kind = Ast_Literal;
    node->literal.type = Literal_Int;
    node->literal.integer = i;

    return node;
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
