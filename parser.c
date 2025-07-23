#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ================== PARSER CREATION AND DESTRUCTION ==================

Parser* parser_create(Lexer* lexer) {
    Parser* parser = malloc(sizeof(Parser));
    if (!parser) return NULL;
    
    parser->lexer = lexer;
    parser->had_error = false;
    parser->panic_mode = false;
    parser->error_message[0] = '\0';
    parser->nodes_created = 0;
    parser->parse_start_time = (double)clock() / CLOCKS_PER_SEC;
    
    // Create arena for AST nodes
    parser->arena = arena_create();
    if (!parser->arena) {
        free(parser);
        return NULL;
    }
    
    // Get first token
    parser->current = lexer_next_token(lexer);
    
    // Skip any leading newlines
    while (parser->current.type == TOKEN_NEWLINE && parser->current.type != TOKEN_EOF) {
        parser->current = lexer_next_token(lexer);
    }
    
    return parser;
}

void parser_destroy(Parser* parser) {
    if (parser) {
        arena_destroy(parser->arena);
        free(parser);
    }
}

// Forward declarations
static void parser_error(Parser* parser, const char* message);
static bool match(Parser* parser, TokenType type);

// ================== UTILITY FUNCTIONS ==================

static void advance(Parser* parser) {
    parser->previous = parser->current;
    parser->current = lexer_next_token(parser->lexer);
    
    // Skip error tokens and report them
    while (parser->current.type == TOKEN_ERROR) {
        parser_error(parser, "Lexical error");
        parser->current = lexer_next_token(parser->lexer);
    }
}

static bool check(Parser* parser, TokenType type) {
    return parser->current.type == type;
}

static bool match(Parser* parser, TokenType type) {
    if (!check(parser, type)) return false;
    advance(parser);
    return true;
}

static bool consume(Parser* parser, TokenType type, const char* message) {
    if (parser->current.type == type) {
        advance(parser);
        return true;
    }
    
    parser_error(parser, message);
    return false;
}

static void parser_error(Parser* parser, const char* message) {
    if (parser->panic_mode) return;  // Don't cascade errors
    
    parser->panic_mode = true;
    parser->had_error = true;
    
    snprintf(parser->error_message, sizeof(parser->error_message),
             "Error at line %d, column %d: %s",
             parser->current.pos.line, parser->current.pos.column, message);
}

static void synchronize(Parser* parser) {
    parser->panic_mode = false;
    
    while (parser->current.type != TOKEN_EOF) {
        if (parser->previous.type == TOKEN_SEMICOLON) return;
        
        switch (parser->current.type) {
            case TOKEN_CLASS:
            case TOKEN_FUNCTION:
            case TOKEN_VAR:
            case TOKEN_FOR:
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_RETURN:
                return;
            default:
                break;
        }
        
        advance(parser);
    }
}

// ================== AST NODE CREATION ==================

static ASTNode* ast_allocate(Parser* parser, ASTNodeType type) {
    ASTNode* node = arena_alloc(parser->arena, sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = type;
    node->pos = parser->previous.pos;
    parser->nodes_created++;
    
    return node;
}

ASTNode* ast_create_literal(Parser* parser, TokenType type, Token token) {
    ASTNode* node = ast_allocate(parser, AST_LITERAL);
    if (!node) return NULL;
    
    node->data.literal.token_type = type;
    
    switch (type) {
        case TOKEN_INTEGER:
            node->data.literal.value.int_value = strtoll(token.start, NULL, 10);
            break;
        case TOKEN_FLOAT:
            node->data.literal.value.float_value = strtod(token.start, NULL);
            break;
        case TOKEN_STRING: {
            // Allocate string and copy (without quotes)
            size_t len = token.length - 2;  // Remove quotes
            char* str = arena_alloc(parser->arena, len + 1);
            if (str) {
                strncpy(str, token.start + 1, len);  // Skip opening quote
                str[len] = '\0';
                node->data.literal.value.string_value = str;
            }
            break;
        }
        default:
            break;
    }
    
    return node;
}

ASTNode* ast_create_identifier(Parser* parser, char* name) {
    ASTNode* node = ast_allocate(parser, AST_IDENTIFIER);
    if (!node) return NULL;
    
    // Copy name to arena
    size_t len = strlen(name);
    char* name_copy = arena_alloc(parser->arena, len + 1);
    if (name_copy) {
        strcpy(name_copy, name);
        node->data.identifier.name = name_copy;
    }
    
    return node;
}

ASTNode* ast_create_binary(Parser* parser, ASTNode* left, TokenType op, ASTNode* right) {
    ASTNode* node = ast_allocate(parser, AST_BINARY);
    if (!node) return NULL;
    
    node->data.binary.left = left;
    node->data.binary.operator = op;
    node->data.binary.right = right;
    
    return node;
}

ASTNode* ast_create_unary(Parser* parser, TokenType op, ASTNode* operand) {
    ASTNode* node = ast_allocate(parser, AST_UNARY);
    if (!node) return NULL;
    
    node->data.unary.operator = op;
    node->data.unary.operand = operand;
    
    return node;
}

ASTNode* ast_create_return(Parser* parser, ASTNode* value) {
    ASTNode* node = ast_allocate(parser, AST_RETURN_STMT);
    if (!node) return NULL;
    
    node->data.return_stmt.value = value;
    
    return node;
}

ASTNode* ast_create_var_decl(Parser* parser, TokenType type, char* name, ASTNode* init) {
    ASTNode* node = ast_allocate(parser, AST_VAR_DECLARATION);
    if (!node) return NULL;
    
    node->data.var_decl.type = type;
    
    // Copy name to arena
    size_t len = strlen(name);
    char* name_copy = arena_alloc(parser->arena, len + 1);
    if (name_copy) {
        strcpy(name_copy, name);
        node->data.var_decl.name = name_copy;
    }
    
    node->data.var_decl.initializer = init;
    
    return node;
}

// ================== RECURSIVE DESCENT PARSER ==================

// Forward declarations for recursive functions
static ASTNode* expression(Parser* parser);
static ASTNode* statement(Parser* parser);
static ASTNode* declaration(Parser* parser);

// Parse primary expressions (literals, identifiers, parentheses)
static ASTNode* primary(Parser* parser) {
    if (match(parser, TOKEN_TRUE) || match(parser, TOKEN_FALSE)) {
        // Boolean literals
        return ast_create_literal(parser, TOKEN_BOOL_KW, parser->previous);
    }
    
    if (match(parser, TOKEN_NULL)) {
        return ast_create_literal(parser, TOKEN_NULL, parser->previous);
    }
    
    if (match(parser, TOKEN_INTEGER)) {
        return ast_create_literal(parser, TOKEN_INTEGER, parser->previous);
    }
    
    if (match(parser, TOKEN_FLOAT)) {
        return ast_create_literal(parser, TOKEN_FLOAT, parser->previous);
    }
    
    if (match(parser, TOKEN_STRING)) {
        return ast_create_literal(parser, TOKEN_STRING, parser->previous);
    }
    
    if (match(parser, TOKEN_IDENTIFIER)) {
        // Extract identifier name
        char name[256];
        size_t len = parser->previous.length;
        if (len >= sizeof(name)) len = sizeof(name) - 1;
        strncpy(name, parser->previous.start, len);
        name[len] = '\0';
        
        return ast_create_identifier(parser, name);
    }
    
    if (match(parser, TOKEN_LPAREN)) {
        ASTNode* expr = expression(parser);
        consume(parser, TOKEN_RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    parser_error(parser, "Expected expression");
    return NULL;
}

// Parse unary expressions (-x, !flag)
static ASTNode* unary(Parser* parser) {
    if (match(parser, TOKEN_NOT) || match(parser, TOKEN_MINUS)) {
        TokenType operator = parser->previous.type;
        ASTNode* right = unary(parser);
        return ast_create_unary(parser, operator, right);
    }
    
    return primary(parser);
}

// Parse multiplication and division
static ASTNode* factor(Parser* parser) {
    ASTNode* expr = unary(parser);
    
    while (match(parser, TOKEN_DIVIDE) || match(parser, TOKEN_MULTIPLY) || match(parser, TOKEN_MODULO)) {
        TokenType operator = parser->previous.type;
        ASTNode* right = unary(parser);
        expr = ast_create_binary(parser, expr, operator, right);
    }
    
    return expr;
}

// Parse addition and subtraction
static ASTNode* term(Parser* parser) {
    ASTNode* expr = factor(parser);
    
    while (match(parser, TOKEN_MINUS) || match(parser, TOKEN_PLUS)) {
        TokenType operator = parser->previous.type;
        ASTNode* right = factor(parser);
        expr = ast_create_binary(parser, expr, operator, right);
    }
    
    return expr;
}

// Parse comparison operators
static ASTNode* comparison(Parser* parser) {
    ASTNode* expr = term(parser);
    
    while (match(parser, TOKEN_GREATER) || match(parser, TOKEN_GREATER_EQUAL) ||
           match(parser, TOKEN_LESS) || match(parser, TOKEN_LESS_EQUAL)) {
        TokenType operator = parser->previous.type;
        ASTNode* right = term(parser);
        expr = ast_create_binary(parser, expr, operator, right);
    }
    
    return expr;
}

// Parse equality operators
static ASTNode* equality(Parser* parser) {
    ASTNode* expr = comparison(parser);
    
    while (match(parser, TOKEN_NOT_EQUAL) || match(parser, TOKEN_EQUAL)) {
        TokenType operator = parser->previous.type;
        ASTNode* right = comparison(parser);
        expr = ast_create_binary(parser, expr, operator, right);
    }
    
    return expr;
}

// Parse logical AND
static ASTNode* logical_and(Parser* parser) {
    ASTNode* expr = equality(parser);
    
    while (match(parser, TOKEN_AND)) {
        TokenType operator = parser->previous.type;
        ASTNode* right = equality(parser);
        expr = ast_create_binary(parser, expr, operator, right);
    }
    
    return expr;
}

// Parse logical OR
static ASTNode* logical_or(Parser* parser) {
    ASTNode* expr = logical_and(parser);
    
    while (match(parser, TOKEN_OR)) {
        TokenType operator = parser->previous.type;
        ASTNode* right = logical_and(parser);
        expr = ast_create_binary(parser, expr, operator, right);
    }
    
    return expr;
}

// Parse assignment
static ASTNode* assignment(Parser* parser) {
    ASTNode* expr = logical_or(parser);
    
    if (match(parser, TOKEN_ASSIGN)) {
        ASTNode* value = assignment(parser);
        
        if (expr->type == AST_IDENTIFIER) {
            // Create assignment node
            ASTNode* assign = ast_allocate(parser, AST_ASSIGNMENT);
            assign->data.binary.left = expr;
            assign->data.binary.operator = TOKEN_ASSIGN;
            assign->data.binary.right = value;
            return assign;
        }
        
        parser_error(parser, "Invalid assignment target");
    }
    
    return expr;
}

// Parse full expressions
static ASTNode* expression(Parser* parser) {
    return assignment(parser);
}

// Parse variable declarations
static ASTNode* var_declaration(Parser* parser) {
    TokenType type = parser->previous.type;  // int, float, string, etc.
    
    consume(parser, TOKEN_IDENTIFIER, "Expected variable name");
    
    char name[256];
    size_t len = parser->previous.length;
    if (len >= sizeof(name)) len = sizeof(name) - 1;
    strncpy(name, parser->previous.start, len);
    name[len] = '\0';
    
    ASTNode* initializer = NULL;
    if (match(parser, TOKEN_ASSIGN)) {
        initializer = expression(parser);
    }
    
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after variable declaration");
    return ast_create_var_decl(parser, type, name, initializer);
}

// Parse return statements
static ASTNode* return_statement(Parser* parser) {
    ASTNode* value = NULL;
    
    if (!check(parser, TOKEN_SEMICOLON)) {
        value = expression(parser);
    }
    
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after return value");
    return ast_create_return(parser, value);
}

// Parse expression statements
static ASTNode* expression_statement(Parser* parser) {
    ASTNode* expr = expression(parser);
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after expression");
    
    ASTNode* stmt = ast_allocate(parser, AST_EXPRESSION_STMT);
    stmt->data.binary.left = expr;  // Reuse binary structure
    
    return stmt;
}

// Parse statements
static ASTNode* statement(Parser* parser) {
    if (match(parser, TOKEN_RETURN)) {
        return return_statement(parser);
    }
    
    return expression_statement(parser);
}

// Parse declarations
static ASTNode* declaration(Parser* parser) {
    if (match(parser, TOKEN_INT) || match(parser, TOKEN_FLOAT_KW) || 
        match(parser, TOKEN_STRING_KW) || match(parser, TOKEN_BOOL_KW)) {
        return var_declaration(parser);
    }
    
    return statement(parser);
}

// ================== MAIN PARSER FUNCTION ==================

ASTNode* parser_parse(Parser* parser) {
    ASTNode* program = ast_allocate(parser, AST_PROGRAM);
    if (!program) return NULL;
    
    // Temporary array for statements (in a real implementation, use dynamic array)
    ASTNode** statements = arena_alloc(parser->arena, sizeof(ASTNode*) * 1000);
    int statement_count = 0;
    
    while (!check(parser, TOKEN_EOF) && !parser->had_error) {
        // Skip newlines between declarations/statements
        if (match(parser, TOKEN_NEWLINE)) {
            continue;
        }
        
        ASTNode* decl = declaration(parser);
        if (decl) {
            statements[statement_count++] = decl;
        }
        
        if (parser->panic_mode) synchronize(parser);
    }
    
    program->data.program.statements = statements;
    program->data.program.statement_count = statement_count;
    
    return program;
}

// ================== UTILITY FUNCTIONS ==================

bool parser_has_error(const Parser* parser) {
    return parser->had_error;
}

const char* parser_get_error(const Parser* parser) {
    return parser->error_message;
}

double parser_get_parse_time(const Parser* parser) {
    double current_time = (double)clock() / CLOCKS_PER_SEC;
    return current_time - parser->parse_start_time;
}

int parser_get_nodes_created(const Parser* parser) {
    return parser->nodes_created;
}

// ================== AST PRINTING ==================

void ast_print(const ASTNode* node, int indent) {
    if (!node) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch (node->type) {
        case AST_LITERAL:
            printf("Literal: ");
            switch (node->data.literal.token_type) {
                case TOKEN_INTEGER:
                    printf("%lld", node->data.literal.value.int_value);
                    break;
                case TOKEN_FLOAT:
                    printf("%g", node->data.literal.value.float_value);
                    break;
                case TOKEN_STRING:
                    printf("\"%s\"", node->data.literal.value.string_value);
                    break;
                default:
                    printf("(unknown)");
                    break;
            }
            printf("\n");
            break;
            
        case AST_IDENTIFIER:
            printf("Identifier: %s\n", node->data.identifier.name);
            break;
            
        case AST_BINARY:
            printf("Binary: %s\n", token_type_to_string(node->data.binary.operator));
            ast_print(node->data.binary.left, indent + 1);
            ast_print(node->data.binary.right, indent + 1);
            break;
            
        case AST_VAR_DECLARATION:
            printf("VarDecl: %s %s\n", 
                   token_type_to_string(node->data.var_decl.type),
                   node->data.var_decl.name);
            if (node->data.var_decl.initializer) {
                ast_print(node->data.var_decl.initializer, indent + 1);
            }
            break;
            
        case AST_PROGRAM:
            printf("Program (%d statements)\n", node->data.program.statement_count);
            for (int i = 0; i < node->data.program.statement_count; i++) {
                ast_print(node->data.program.statements[i], indent + 1);
            }
            break;
            
        default:
            printf("Unknown AST node type\n");
            break;
    }
}
