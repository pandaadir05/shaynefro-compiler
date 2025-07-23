#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "lexer.h"
#include <stdbool.h>

// ================== AST (Abstract Syntax Tree) NODES ==================

typedef enum {
    // Expressions
    AST_LITERAL,           // 42, 3.14, "hello"
    AST_IDENTIFIER,        // variable names
    AST_BINARY,            // x + y, a * b
    AST_UNARY,             // -x, !flag
    AST_ASSIGNMENT,        // x = 42
    AST_CALL,              // function(args)
    
    // Statements
    AST_EXPRESSION_STMT,   // expression;
    AST_VAR_DECLARATION,   // int x = 42;
    AST_FUNCTION_DECL,     // function name() { }
    AST_CLASS_DECL,        // class Name { }
    AST_IF_STMT,           // if (condition) { }
    AST_WHILE_STMT,        // while (condition) { }
    AST_FOR_STMT,          // for (init; condition; update) { }
    AST_RETURN_STMT,       // return value;
    AST_BLOCK_STMT,        // { statements }
    
    // Program structure
    AST_PROGRAM            // Root node containing all statements
} ASTNodeType;

// Forward declaration
typedef struct ASTNode ASTNode;

// AST Node structure
typedef struct ASTNode {
    ASTNodeType type;
    Position pos;  // Source location for error reporting
    
    union {
        // Literals
        struct {
            TokenType token_type;
            union {
                long long int_value;
                double float_value;
                char* string_value;
            } value;
        } literal;
        
        // Identifiers
        struct {
            char* name;
        } identifier;
        
        // Binary operations (x + y, a * b, etc.)
        struct {
            ASTNode* left;
            TokenType operator;
            ASTNode* right;
        } binary;
        
        // Unary operations (-x, !flag)
        struct {
            TokenType operator;
            ASTNode* operand;
        } unary;
        
        // Variable declarations (int x = 42;)
        struct {
            TokenType type;  // int, float, string, etc.
            char* name;      // variable name
            ASTNode* initializer;  // initial value
        } var_decl;
        
        // Function declarations
        struct {
            char* name;
            char** parameters;  // parameter names
            TokenType* param_types;  // parameter types
            int param_count;
            ASTNode* body;  // function body
        } func_decl;
        
        // Class declarations
        struct {
            char* name;
            ASTNode** methods;  // array of method declarations
            ASTNode** fields;   // array of field declarations
            int method_count;
            int field_count;
        } class_decl;
        
        // If statements
        struct {
            ASTNode* condition;
            ASTNode* then_stmt;
            ASTNode* else_stmt;  // optional
        } if_stmt;
        
        // While loops
        struct {
            ASTNode* condition;
            ASTNode* body;
        } while_stmt;
        
        // For loops
        struct {
            ASTNode* initializer;  // int i = 0
            ASTNode* condition;    // i < 10
            ASTNode* update;       // i++
            ASTNode* body;         // loop body
        } for_stmt;
        
        // Return statements
        struct {
            ASTNode* value;  // optional return value
        } return_stmt;
        
        // Block statements
        struct {
            ASTNode** statements;
            int statement_count;
        } block;
        
        // Function calls
        struct {
            char* name;
            ASTNode** arguments;
            int arg_count;
        } call;
        
        // Program (root node)
        struct {
            ASTNode** statements;
            int statement_count;
        } program;
    } data;
} ASTNode;

// ================== PARSER STRUCTURE ==================

typedef struct {
    Lexer* lexer;           // The lexer that provides tokens
    Token current;          // Current token being processed
    Token previous;         // Previous token
    bool had_error;         // Error flag
    bool panic_mode;        // Panic mode for error recovery
    char error_message[256]; // Error message storage
    
    // Memory management
    Arena* arena;           // Arena for AST nodes
    
    // Performance tracking
    int nodes_created;      // Number of AST nodes created
    double parse_start_time; // Parsing start time
} Parser;

// ================== PARSER FUNCTIONS ==================

// Core parser functions
Parser* parser_create(Lexer* lexer);
void parser_destroy(Parser* parser);
ASTNode* parser_parse(Parser* parser);

// AST node creation
ASTNode* ast_create_literal(Parser* parser, TokenType type, Token token);
ASTNode* ast_create_identifier(Parser* parser, char* name);
ASTNode* ast_create_binary(Parser* parser, ASTNode* left, TokenType op, ASTNode* right);
ASTNode* ast_create_unary(Parser* parser, TokenType op, ASTNode* operand);
ASTNode* ast_create_var_decl(Parser* parser, TokenType type, char* name, ASTNode* init);
ASTNode* ast_create_function(Parser* parser, char* name, ASTNode* body);
ASTNode* ast_create_class(Parser* parser, char* name);
ASTNode* ast_create_if(Parser* parser, ASTNode* condition, ASTNode* then_stmt, ASTNode* else_stmt);
ASTNode* ast_create_while(Parser* parser, ASTNode* condition, ASTNode* body);
ASTNode* ast_create_return(Parser* parser, ASTNode* value);
ASTNode* ast_create_block(Parser* parser);

// AST utilities
void ast_print(const ASTNode* node, int indent);
void ast_destroy(ASTNode* node);

// Error handling
bool parser_has_error(const Parser* parser);
const char* parser_get_error(const Parser* parser);

// Performance functions
double parser_get_parse_time(const Parser* parser);
int parser_get_nodes_created(const Parser* parser);

#endif
