#ifndef SLANG_EXEC_H
#define SLANG_EXEC_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef enum {
    Value_Kind_Void,
    Value_Kind_Int
} Value_Kind;

typedef struct {
    Value_Kind kind;
    union {
        int64_t integer;
    };
} Value;

typedef struct {
    String name;
    Value value;
} Variable;

typedef Array(Variable) Variable_Array;
typedef Array(Value) Value_Array;

typedef struct {
    Variable_Array variables;
    Ast_Module module;
} Execution_Context;

void execute(Execution_Context* context);


#ifdef SLANG_EXEC_C

Value execute_expression(Execution_Context *context, Ast_Expression* expression);

size_t evaluate_lhs_expression(Execution_Context *context, Ast_Expression* expression) {
    if (expression->kind != Ast_Identifier) {
        fprintf(stderr, "[ERROR]: Expected identifier as LHS expression\n");
    }
    String new_variable = expression->identifier;

    Variable_Array variables = context->variables;

    for (size_t i = 0; i < variables.sz; ++i) {
        Variable variable = variables.arr[i];

        if (strcmp(variable.name.str, new_variable.str) == 0) {
            return i;
        }
    }

    size_t last = variables.sz;

    Variable var = (Variable) {0};
    var.name = new_variable;

    array_push_back(&context->variables, var);

    return last;

}

Value evaluate_operation(Token_Kind operation, Value left, Value right) {
    if (left.kind != Value_Kind_Int || right.kind != Value_Kind_Int) {
        fprintf(stderr, "[ERROR]: Can only evaluate expression between 2 integers\n");
        exit(1);
    }

    int64_t l = left.integer;
    int64_t r = right.integer;

    switch (operation) {
        case '+':
            return (Value) { .kind = Value_Kind_Int, .integer = l + r };
        case '-':
            return (Value) { .kind = Value_Kind_Int, .integer = l - r };
        case '*':
            return (Value) { .kind = Value_Kind_Int, .integer = l * r };
        case '/':
            return (Value) { .kind = Value_Kind_Int, .integer = l / r };
        case '%':
            return (Value) { .kind = Value_Kind_Int, .integer = l % r };
        case '&':
            return (Value) { .kind = Value_Kind_Int, .integer = l & r };
        case '|':
            return (Value) { .kind = Value_Kind_Int, .integer = l | r };
        default:
            assert(0);
    }
}

Value get_identifier_value(Execution_Context* context, String identifier) {
    Variable_Array variables = context->variables;

    for (size_t i = 0; i < variables.sz; ++i) {
        Variable variable = variables.arr[i];

        if (strcmp(variable.name.str, identifier.str) == 0) {
            return variable.value;
        }
    }

    fprintf(stderr, "[ERROR]: Unknown variable %s encountered.\n", identifier.str);
    exit(1);
}

Value handle_function_call(String name, Value_Array parameters) {
    if (strcmp(name.str, "print") == 0) { 
        for (size_t i = 0; i < parameters.sz; ++i) {
            Value v = parameters.arr[i];

            if (v.kind == Value_Kind_Int) {
                printf("%zd ", v.integer);
            } else if (v.kind == Value_Kind_Void) {
                printf("<void> ");
            }
        }
        printf("\n");

        return (Value) {
            .kind = Value_Kind_Void
        };
    }

    fprintf(stderr, "Unknown function %s\n", name.str);
}

Value execute_expression(Execution_Context *context, Ast_Expression* expression) {
    switch (expression->kind) {
        case Ast_Binary:
            switch (expression->binary.kind) {
                case '=':
                    size_t var_index = evaluate_lhs_expression(context, expression->binary.left);
                    Value value = execute_expression(context, expression->binary.right);

                    context->variables.arr[var_index].value = value;

                    return value;
                    
                case '+': case '-': case '*': case '/': case '%':
                case '&': case '|':
                    Value left = execute_expression(context, expression->binary.left);
                    Value right = execute_expression(context, expression->binary.right);

                    return evaluate_operation(expression->binary.kind, left, right);

            }
            break;
        case Ast_Literal:
            if (expression->literal.kind != Token_Int) {
                assert(0);
            }
            return (Value) {
                .kind = Value_Kind_Int, 
                .integer = expression->literal.integer
            };
        case Ast_Identifier:
            return get_identifier_value(context, expression->identifier);
        case Ast_Function_Call:
            Value_Array param_values = (Value_Array) {0};
            Ast_Expression_Function_Call function_call = expression->function_call;
            Ast_Expression_Array parameters = function_call.parameters;

            for (size_t i = 0; i < parameters.sz; ++i) {
                Value value = execute_expression(context, parameters.arr[i]);
                array_push_back(&param_values, value);
            }

            return handle_function_call(function_call.identifier, param_values);
        default:
            assert(0);
            
    }
}

void execute_module(Execution_Context* context, Ast_Module module) {
    Ast_Expression_Array statements = module.statements;

    for (size_t i = 0; i < statements.sz; ++i) {
        Value value = execute_expression(context, statements.arr[i]);
    }
}

void execute(Execution_Context* context) {
    execute_module(context, context->module);
}

#endif // SLANG_EXEC_C
#endif // SLANG_EXEC_H
