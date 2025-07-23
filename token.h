#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>

typedef enum {
    // Literals
    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_STRING,
    TOKEN_CHAR,
    TOKEN_IDENTIFIER,
    
    // Keywords - Basic Types
    TOKEN_INT,
    TOKEN_FLOAT_KW,
    TOKEN_STRING_KW,
    TOKEN_BOOL_KW,
    TOKEN_CHAR_KW,
    TOKEN_VOID_KW,
    
    // Keywords - Control Flow
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_FOR,
    TOKEN_DO,
    TOKEN_SWITCH,
    TOKEN_CASE,
    TOKEN_DEFAULT,
    TOKEN_BREAK,
    TOKEN_CONTINUE,
    TOKEN_RETURN,
    
    // Keywords - Functions & Variables
    TOKEN_FUNCTION,
    TOKEN_VAR,
    TOKEN_CONST,
    
    // Keywords - OOP
    TOKEN_CLASS,
    TOKEN_STRUCT,
    TOKEN_ENUM,
    TOKEN_INTERFACE,
    TOKEN_IMPLEMENTS,
    TOKEN_EXTENDS,
    TOKEN_PUBLIC,
    TOKEN_PRIVATE,
    TOKEN_PROTECTED,
    TOKEN_STATIC,
    TOKEN_FINAL,
    TOKEN_ABSTRACT,
    TOKEN_VIRTUAL,
    TOKEN_OVERRIDE,
    
    // Keywords - Error Handling
    TOKEN_TRY,
    TOKEN_CATCH,
    TOKEN_FINALLY,
    TOKEN_THROW,
    
    // Keywords - Modules
    TOKEN_IMPORT,
    TOKEN_EXPORT,
    TOKEN_MODULE,
    TOKEN_NAMESPACE,
    
    // Keywords - Literals
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_NULL,
    TOKEN_UNDEFINED,
    
    // Arithmetic Operators
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_MODULO,
    TOKEN_POWER,
    TOKEN_INCREMENT,
    TOKEN_DECREMENT,
    
    // Assignment Operators
    TOKEN_ASSIGN,
    TOKEN_PLUS_ASSIGN,
    TOKEN_MINUS_ASSIGN,
    TOKEN_MULTIPLY_ASSIGN,
    TOKEN_DIVIDE_ASSIGN,
    TOKEN_MODULO_ASSIGN,
    TOKEN_POWER_ASSIGN,
    
    // Comparison Operators
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_STRICT_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    
    // Logical Operators
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,
    
    // Bitwise Operators
    TOKEN_BITWISE_AND,
    TOKEN_BITWISE_OR,
    TOKEN_XOR,
    TOKEN_TILDE,
    TOKEN_LSHIFT,
    TOKEN_RSHIFT,
    TOKEN_AND_ASSIGN,
    TOKEN_OR_ASSIGN,
    TOKEN_XOR_ASSIGN,
    TOKEN_LSHIFT_ASSIGN,
    TOKEN_RSHIFT_ASSIGN,
    
    // Delimiters
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_COLON,
    TOKEN_SCOPE,
    TOKEN_ARROW,
    TOKEN_QUESTION,
    TOKEN_ELLIPSIS,
    TOKEN_HASH,
    
    // Special
    TOKEN_NEWLINE,
    TOKEN_EOF,
    TOKEN_ERROR,
    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    int line;
    int column;
    const char* filename;
} Position;

typedef struct {
    TokenType type;
    const char* start;
    size_t length;
    Position pos;
    union {
        long long int_value;
        double float_value;
    } value;
} Token;

const char* token_type_to_string(TokenType type);
void token_print(const Token* token);

#endif
