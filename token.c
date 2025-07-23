#include "token.h"
#include <stdio.h>

const char* token_type_to_string(TokenType type) {
    switch (type) {
        // Literals
        case TOKEN_INTEGER: return "INTEGER";
        case TOKEN_FLOAT: return "FLOAT";
        case TOKEN_STRING: return "STRING";
        case TOKEN_CHAR: return "CHAR";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        
        // Keywords - Basic Types
        case TOKEN_INT: return "INT";
        case TOKEN_FLOAT_KW: return "FLOAT_KW";
        case TOKEN_STRING_KW: return "STRING_KW";
        case TOKEN_BOOL_KW: return "BOOL_KW";
        case TOKEN_CHAR_KW: return "CHAR_KW";
        case TOKEN_VOID_KW: return "VOID_KW";
        
        // Keywords - Control Flow
        case TOKEN_IF: return "IF";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_FOR: return "FOR";
        case TOKEN_DO: return "DO";
        case TOKEN_SWITCH: return "SWITCH";
        case TOKEN_CASE: return "CASE";
        case TOKEN_DEFAULT: return "DEFAULT";
        case TOKEN_BREAK: return "BREAK";
        case TOKEN_CONTINUE: return "CONTINUE";
        case TOKEN_RETURN: return "RETURN";
        
        // Keywords - Functions & Variables
        case TOKEN_FUNCTION: return "FUNCTION";
        case TOKEN_VAR: return "VAR";
        case TOKEN_CONST: return "CONST";
        
        // Keywords - OOP
        case TOKEN_CLASS: return "CLASS";
        case TOKEN_STRUCT: return "STRUCT";
        case TOKEN_ENUM: return "ENUM";
        case TOKEN_INTERFACE: return "INTERFACE";
        case TOKEN_IMPLEMENTS: return "IMPLEMENTS";
        case TOKEN_EXTENDS: return "EXTENDS";
        case TOKEN_PUBLIC: return "PUBLIC";
        case TOKEN_PRIVATE: return "PRIVATE";
        case TOKEN_PROTECTED: return "PROTECTED";
        case TOKEN_STATIC: return "STATIC";
        case TOKEN_FINAL: return "FINAL";
        case TOKEN_ABSTRACT: return "ABSTRACT";
        case TOKEN_VIRTUAL: return "VIRTUAL";
        case TOKEN_OVERRIDE: return "OVERRIDE";
        
        // Keywords - Error Handling
        case TOKEN_TRY: return "TRY";
        case TOKEN_CATCH: return "CATCH";
        case TOKEN_FINALLY: return "FINALLY";
        case TOKEN_THROW: return "THROW";
        
        // Keywords - Modules
        case TOKEN_IMPORT: return "IMPORT";
        case TOKEN_EXPORT: return "EXPORT";
        case TOKEN_MODULE: return "MODULE";
        case TOKEN_NAMESPACE: return "NAMESPACE";
        
        // Keywords - Literals
        case TOKEN_TRUE: return "TRUE";
        case TOKEN_FALSE: return "FALSE";
        case TOKEN_NULL: return "NULL";
        case TOKEN_UNDEFINED: return "UNDEFINED";
        
        // Arithmetic Operators
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MULTIPLY: return "MULTIPLY";
        case TOKEN_DIVIDE: return "DIVIDE";
        case TOKEN_MODULO: return "MODULO";
        case TOKEN_POWER: return "POWER";
        case TOKEN_INCREMENT: return "INCREMENT";
        case TOKEN_DECREMENT: return "DECREMENT";
        
        // Assignment Operators
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_PLUS_ASSIGN: return "PLUS_ASSIGN";
        case TOKEN_MINUS_ASSIGN: return "MINUS_ASSIGN";
        case TOKEN_MULTIPLY_ASSIGN: return "MULTIPLY_ASSIGN";
        case TOKEN_DIVIDE_ASSIGN: return "DIVIDE_ASSIGN";
        case TOKEN_MODULO_ASSIGN: return "MODULO_ASSIGN";
        case TOKEN_POWER_ASSIGN: return "POWER_ASSIGN";
        
        // Comparison Operators
        case TOKEN_EQUAL: return "EQUAL";
        case TOKEN_NOT_EQUAL: return "NOT_EQUAL";
        case TOKEN_STRICT_EQUAL: return "STRICT_EQUAL";
        case TOKEN_LESS: return "LESS";
        case TOKEN_LESS_EQUAL: return "LESS_EQUAL";
        case TOKEN_GREATER: return "GREATER";
        case TOKEN_GREATER_EQUAL: return "GREATER_EQUAL";
        
        // Logical Operators
        case TOKEN_AND: return "AND";
        case TOKEN_OR: return "OR";
        case TOKEN_NOT: return "NOT";
        
        // Bitwise Operators
        case TOKEN_BITWISE_AND: return "BITWISE_AND";
        case TOKEN_BITWISE_OR: return "BITWISE_OR";
        case TOKEN_XOR: return "XOR";
        case TOKEN_TILDE: return "TILDE";
        case TOKEN_LSHIFT: return "LSHIFT";
        case TOKEN_RSHIFT: return "RSHIFT";
        case TOKEN_AND_ASSIGN: return "AND_ASSIGN";
        case TOKEN_OR_ASSIGN: return "OR_ASSIGN";
        case TOKEN_XOR_ASSIGN: return "XOR_ASSIGN";
        case TOKEN_LSHIFT_ASSIGN: return "LSHIFT_ASSIGN";
        case TOKEN_RSHIFT_ASSIGN: return "RSHIFT_ASSIGN";
        
        // Delimiters
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE";
        case TOKEN_LBRACKET: return "LBRACKET";
        case TOKEN_RBRACKET: return "RBRACKET";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_DOT: return "DOT";
        case TOKEN_COLON: return "COLON";
        case TOKEN_SCOPE: return "SCOPE";
        case TOKEN_ARROW: return "ARROW";
        case TOKEN_QUESTION: return "QUESTION";
        case TOKEN_ELLIPSIS: return "ELLIPSIS";
        case TOKEN_HASH: return "HASH";
        
        // Special
        case TOKEN_NEWLINE: return "NEWLINE";
        case TOKEN_EOF: return "EOF";
        case TOKEN_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

void token_print(const Token* token) {
    printf("Token{type=%s, lexeme='%.*s', line=%d, col=%d}",
           token_type_to_string(token->type),
           (int)token->length, token->start,
           token->pos.line, token->pos.column);
}
