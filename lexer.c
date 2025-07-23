#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// Arena implementation
Arena* arena_create(void) {
    Arena* arena = malloc(sizeof(Arena));
    if (!arena) return NULL;
    
    arena->memory = malloc(ARENA_SIZE);
    if (!arena->memory) {
        free(arena);
        return NULL;
    }
    
    arena->size = ARENA_SIZE;
    arena->used = 0;
    return arena;
}

void arena_destroy(Arena* arena) {
    if (arena) {
        free(arena->memory);
        free(arena);
    }
}

void* arena_alloc(Arena* arena, size_t size) {
    if (arena->used + size > arena->size) {
        return NULL; // Arena full
    }
    
    void* ptr = arena->memory + arena->used;
    arena->used += size;
    return ptr;
}

// Keyword initialization
static void init_keywords(Lexer* lexer) {
    const struct { const char* word; TokenType type; } keywords[] = {
        {"int", TOKEN_INT},
        {"float", TOKEN_FLOAT_KW},
        {"string", TOKEN_STRING_KW},
        {"bool", TOKEN_BOOL_KW},
        {"char", TOKEN_CHAR_KW},
        {"void", TOKEN_VOID_KW},
        {"if", TOKEN_IF},
        {"else", TOKEN_ELSE},
        {"while", TOKEN_WHILE},
        {"for", TOKEN_FOR},
        {"do", TOKEN_DO},
        {"switch", TOKEN_SWITCH},
        {"case", TOKEN_CASE},
        {"default", TOKEN_DEFAULT},
        {"break", TOKEN_BREAK},
        {"continue", TOKEN_CONTINUE},
        {"return", TOKEN_RETURN},
        {"function", TOKEN_FUNCTION},
        {"var", TOKEN_VAR},
        {"const", TOKEN_CONST},
        {"class", TOKEN_CLASS},
        {"struct", TOKEN_STRUCT},
        {"enum", TOKEN_ENUM},
        {"interface", TOKEN_INTERFACE},
        {"implements", TOKEN_IMPLEMENTS},
        {"extends", TOKEN_EXTENDS},
        {"public", TOKEN_PUBLIC},
        {"private", TOKEN_PRIVATE},
        {"protected", TOKEN_PROTECTED},
        {"static", TOKEN_STATIC},
        {"final", TOKEN_FINAL},
        {"abstract", TOKEN_ABSTRACT},
        {"virtual", TOKEN_VIRTUAL},
        {"override", TOKEN_OVERRIDE},
        {"try", TOKEN_TRY},
        {"catch", TOKEN_CATCH},
        {"finally", TOKEN_FINALLY},
        {"throw", TOKEN_THROW},
        {"import", TOKEN_IMPORT},
        {"export", TOKEN_EXPORT},
        {"module", TOKEN_MODULE},
        {"namespace", TOKEN_NAMESPACE},
        {"true", TOKEN_TRUE},
        {"false", TOKEN_FALSE},
        {"null", TOKEN_NULL},
        {"undefined", TOKEN_UNDEFINED}
    };
    
    lexer->keyword_count = sizeof(keywords) / sizeof(keywords[0]);
    for (size_t i = 0; i < lexer->keyword_count; i++) {
        lexer->keywords[i].keyword = keywords[i].word;
        lexer->keywords[i].token_type = keywords[i].type;
    }
}

// Lexer creation and destruction
Lexer* lexer_create(const char* source, const char* filename) {
    Lexer* lexer = malloc(sizeof(Lexer));
    if (!lexer) return NULL;
    
    lexer->arena = arena_create();
    if (!lexer->arena) {
        free(lexer);
        return NULL;
    }
    
    lexer->source = source;
    lexer->current = source;
    lexer->start = source;
    lexer->pos.line = 1;
    lexer->pos.column = 1;
    lexer->pos.filename = filename;
    lexer->has_error = false;
    lexer->error_message[0] = '\0';
    
    // Initialize performance tracking
    lexer->tokens_processed = 0;
    lexer->start_time = (double)clock() / CLOCKS_PER_SEC;
    
    // Initialize string pool for interning
    lexer->string_pool_size = 8192;
    lexer->string_pool = arena_alloc(lexer->arena, lexer->string_pool_size);
    lexer->string_pool_used = 0;
    
    init_keywords(lexer);
    
    return lexer;
}

void lexer_destroy(Lexer* lexer) {
    if (lexer) {
        arena_destroy(lexer->arena);
        free(lexer);
    }
}

// Utility functions
static bool is_at_end(const Lexer* lexer) {
    return *lexer->current == '\0';
}

static char advance(Lexer* lexer) {
    if (is_at_end(lexer)) return '\0';
    
    if (*lexer->current == '\n') {
        lexer->pos.line++;
        lexer->pos.column = 1;
    } else {
        lexer->pos.column++;
    }
    
    return *lexer->current++;
}

static char peek(const Lexer* lexer) {
    return *lexer->current;
}

static char peek_next(const Lexer* lexer) {
    if (is_at_end(lexer)) return '\0';
    return lexer->current[1];
}

static bool match(Lexer* lexer, char expected) {
    if (is_at_end(lexer) || *lexer->current != expected) {
        return false;
    }
    lexer->current++;
    lexer->pos.column++;
    return true;
}

static void skip_whitespace(Lexer* lexer) {
    while (!is_at_end(lexer)) {
        char c = peek(lexer);
        if (c == ' ' || c == '\r' || c == '\t') {
            advance(lexer);
        } else if (c == '/' && peek_next(lexer) == '/') {
            // Skip line comment
            while (peek(lexer) != '\n' && !is_at_end(lexer)) {
                advance(lexer);
            }
        } else if (c == '/' && peek_next(lexer) == '*') {
            // Skip block comment
            advance(lexer); // '/'
            advance(lexer); // '*'
            while (!is_at_end(lexer)) {
                if (peek(lexer) == '*' && peek_next(lexer) == '/') {
                    advance(lexer); // '*'
                    advance(lexer); // '/'
                    break;
                }
                advance(lexer);
            }
        } else {
            break;
        }
    }
}

static Token make_token(Lexer* lexer, TokenType type) {
    Token token;
    token.type = type;
    token.start = lexer->start;
    token.length = lexer->current - lexer->start;
    token.pos = lexer->pos;
    token.pos.column -= (int)token.length;
    return token;
}

static Token error_token(Lexer* lexer, const char* message) {
    lexer->has_error = true;
    strncpy(lexer->error_message, message, sizeof(lexer->error_message) - 1);
    lexer->error_message[sizeof(lexer->error_message) - 1] = '\0';
    
    Token token;
    token.type = TOKEN_ERROR;
    token.start = lexer->start;
    token.length = lexer->current - lexer->start;
    token.pos = lexer->pos;
    return token;
}

static Token number(Lexer* lexer) {
    bool is_float = false;
    int base = 10;
    
    // Check for hex (0x), binary (0b), or octal (0o) prefixes
    if (peek(lexer) == '0' && !is_at_end(lexer)) {
        char next = peek_next(lexer);
        if (next == 'x' || next == 'X') {
            advance(lexer); // consume '0'
            advance(lexer); // consume 'x'
            base = 16;
            while (isxdigit(peek(lexer))) {
                advance(lexer);
            }
        } else if (next == 'b' || next == 'B') {
            advance(lexer); // consume '0'
            advance(lexer); // consume 'b'
            base = 2;
            while (peek(lexer) == '0' || peek(lexer) == '1') {
                advance(lexer);
            }
        } else if (next == 'o' || next == 'O') {
            advance(lexer); // consume '0'
            advance(lexer); // consume 'o'
            base = 8;
            while (peek(lexer) >= '0' && peek(lexer) <= '7') {
                advance(lexer);
            }
        } else {
            // Regular decimal number starting with 0
            while (isdigit(peek(lexer))) {
                advance(lexer);
            }
            
            // Look for decimal point (only for base 10)
            if (peek(lexer) == '.' && isdigit(peek_next(lexer))) {
                is_float = true;
                advance(lexer); // consume '.'
                while (isdigit(peek(lexer))) {
                    advance(lexer);
                }
            }
        }
    } else {
        // Regular decimal number
        while (isdigit(peek(lexer))) {
            advance(lexer);
        }
        
        // Look for decimal point
        if (peek(lexer) == '.' && isdigit(peek_next(lexer))) {
            is_float = true;
            advance(lexer); // consume '.'
            while (isdigit(peek(lexer))) {
                advance(lexer);
            }
        }
        
        // Look for scientific notation (e.g., 1.5e10, 2E-5)
        if ((peek(lexer) == 'e' || peek(lexer) == 'E') && 
            (isdigit(peek_next(lexer)) || 
             (peek_next(lexer) == '+' || peek_next(lexer) == '-'))) {
            is_float = true;
            advance(lexer); // consume 'e' or 'E'
            if (peek(lexer) == '+' || peek(lexer) == '-') {
                advance(lexer);
            }
            while (isdigit(peek(lexer))) {
                advance(lexer);
            }
        }
    }
    
    Token token = make_token(lexer, is_float ? TOKEN_FLOAT : TOKEN_INTEGER);
    
    if (is_float) {
        char* endptr;
        token.value.float_value = strtod(lexer->start, &endptr);
    } else {
        char* endptr;
        token.value.int_value = strtoll(lexer->start, &endptr, base);
    }
    
    return token;
}

static Token string(Lexer* lexer) {
    while (peek(lexer) != '"' && !is_at_end(lexer)) {
        if (peek(lexer) == '\\') {
            // Handle escape sequences
            advance(lexer); // consume backslash
            if (!is_at_end(lexer)) {
                char escaped = peek(lexer);
                switch (escaped) {
                    case 'n':   // newline
                    case 't':   // tab
                    case 'r':   // carriage return
                    case '\\':  // backslash
                    case '"':   // quote
                    case '\'':  // single quote
                    case '0':   // null character
                        advance(lexer);
                        break;
                    case 'x':   // hex escape \xHH
                        advance(lexer);
                        if (isxdigit(peek(lexer))) {
                            advance(lexer);
                            if (isxdigit(peek(lexer))) {
                                advance(lexer);
                            }
                        }
                        break;
                    case 'u':   // unicode escape \uHHHH
                        advance(lexer);
                        for (int i = 0; i < 4 && isxdigit(peek(lexer)); i++) {
                            advance(lexer);
                        }
                        break;
                    default:
                        // Invalid escape sequence, but continue parsing
                        advance(lexer);
                        break;
                }
            }
        } else {
            if (peek(lexer) == '\n') {
                lexer->pos.line++;
                lexer->pos.column = 1;
            } else {
                lexer->pos.column++;
            }
            lexer->current++;
        }
    }
    
    if (is_at_end(lexer)) {
        return error_token(lexer, "Unterminated string");
    }
    
    advance(lexer); // closing quote
    return make_token(lexer, TOKEN_STRING);
}

static TokenType check_keyword(const Lexer* lexer, const char* text, size_t length) {
    for (size_t i = 0; i < lexer->keyword_count; i++) {
        const char* keyword = lexer->keywords[i].keyword;
        if (strlen(keyword) == length && memcmp(text, keyword, length) == 0) {
            return lexer->keywords[i].token_type;
        }
    }
    return TOKEN_IDENTIFIER;
}

static Token identifier(Lexer* lexer) {
    while (isalnum(peek(lexer)) || peek(lexer) == '_') {
        advance(lexer);
    }
    
    TokenType type = check_keyword(lexer, lexer->start, lexer->current - lexer->start);
    return make_token(lexer, type);
}

// Main tokenization function
Token lexer_next_token(Lexer* lexer) {
    skip_whitespace(lexer);
    
    lexer->start = lexer->current;
    lexer->tokens_processed++; // Track performance
    
    if (is_at_end(lexer)) {
        return make_token(lexer, TOKEN_EOF);
    }
    
    char c = advance(lexer);
    
    if (isalpha(c) || c == '_') {
        return identifier(lexer);
    }
    
    if (isdigit(c)) {
        return number(lexer);
    }
    
    switch (c) {
        case '(': return make_token(lexer, TOKEN_LPAREN);
        case ')': return make_token(lexer, TOKEN_RPAREN);
        case '{': return make_token(lexer, TOKEN_LBRACE);
        case '}': return make_token(lexer, TOKEN_RBRACE);
        case '[': return make_token(lexer, TOKEN_LBRACKET);
        case ']': return make_token(lexer, TOKEN_RBRACKET);
        case ';': return make_token(lexer, TOKEN_SEMICOLON);
        case ',': return make_token(lexer, TOKEN_COMMA);
        case '.': 
            if (match(lexer, '.')) {
                if (match(lexer, '.')) {
                    return make_token(lexer, TOKEN_ELLIPSIS); // ...
                }
                return error_token(lexer, "Invalid token '..'");
            }
            return make_token(lexer, TOKEN_DOT);
        case ':':
            return make_token(lexer, match(lexer, ':') ? TOKEN_SCOPE : TOKEN_COLON);
        case '?': return make_token(lexer, TOKEN_QUESTION);
        case '~': return make_token(lexer, TOKEN_TILDE);
        case '^': 
            return make_token(lexer, match(lexer, '=') ? TOKEN_XOR_ASSIGN : TOKEN_XOR);
        case '+':
            if (match(lexer, '+')) return make_token(lexer, TOKEN_INCREMENT);
            if (match(lexer, '=')) return make_token(lexer, TOKEN_PLUS_ASSIGN);
            return make_token(lexer, TOKEN_PLUS);
        case '-':
            if (match(lexer, '-')) return make_token(lexer, TOKEN_DECREMENT);
            if (match(lexer, '=')) return make_token(lexer, TOKEN_MINUS_ASSIGN);
            if (match(lexer, '>')) return make_token(lexer, TOKEN_ARROW);
            return make_token(lexer, TOKEN_MINUS);
        case '*':
            if (match(lexer, '=')) return make_token(lexer, TOKEN_MULTIPLY_ASSIGN);
            if (match(lexer, '*')) {
                return make_token(lexer, match(lexer, '=') ? TOKEN_POWER_ASSIGN : TOKEN_POWER);
            }
            return make_token(lexer, TOKEN_MULTIPLY);
        case '/':
            if (match(lexer, '=')) return make_token(lexer, TOKEN_DIVIDE_ASSIGN);
            return make_token(lexer, TOKEN_DIVIDE);
        case '%':
            return make_token(lexer, match(lexer, '=') ? TOKEN_MODULO_ASSIGN : TOKEN_MODULO);
        case '\n': return make_token(lexer, TOKEN_NEWLINE);
        case '"': return string(lexer);
        case '\'': // Character literals
            advance(lexer); // consume opening quote
            if (peek(lexer) == '\\') {
                advance(lexer); // consume backslash
                advance(lexer); // consume escaped character
            } else if (peek(lexer) != '\'') {
                advance(lexer); // consume character
            }
            if (!match(lexer, '\'')) {
                return error_token(lexer, "Unterminated character literal");
            }
            return make_token(lexer, TOKEN_CHAR);
        case '!':
            return make_token(lexer, match(lexer, '=') ? TOKEN_NOT_EQUAL : TOKEN_NOT);
        case '=':
            if (match(lexer, '=')) {
                return make_token(lexer, match(lexer, '=') ? TOKEN_STRICT_EQUAL : TOKEN_EQUAL);
            }
            return make_token(lexer, TOKEN_ASSIGN);
        case '<':
            if (match(lexer, '<')) {
                return make_token(lexer, match(lexer, '=') ? TOKEN_LSHIFT_ASSIGN : TOKEN_LSHIFT);
            }
            return make_token(lexer, match(lexer, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
            if (match(lexer, '>')) {
                return make_token(lexer, match(lexer, '=') ? TOKEN_RSHIFT_ASSIGN : TOKEN_RSHIFT);
            }
            return make_token(lexer, match(lexer, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '&':
            if (match(lexer, '&')) return make_token(lexer, TOKEN_AND);
            if (match(lexer, '=')) return make_token(lexer, TOKEN_AND_ASSIGN);
            return make_token(lexer, TOKEN_BITWISE_AND);
        case '|':
            if (match(lexer, '|')) return make_token(lexer, TOKEN_OR);
            if (match(lexer, '=')) return make_token(lexer, TOKEN_OR_ASSIGN);
            return make_token(lexer, TOKEN_BITWISE_OR);
        case '#': // Preprocessor or special directives
            return make_token(lexer, TOKEN_HASH);
    }
    
    return error_token(lexer, "Unexpected character");
}

bool lexer_has_error(const Lexer* lexer) {
    return lexer->has_error;
}

const char* lexer_get_error(const Lexer* lexer) {
    return lexer->error_message;
}

void lexer_print_stats(const Lexer* lexer) {
    if (!lexer) return;
    
    double current_time = (double)clock() / CLOCKS_PER_SEC;
    double elapsed_time = current_time - lexer->start_time;
    double tokens_per_sec = elapsed_time > 0 ? lexer->tokens_processed / elapsed_time : 0;
    
    printf("   >> Lexer Stats:\n");
    printf("      * Arena usage: %zu / %zu bytes (%.1f%%)\n", 
           lexer->arena->used, lexer->arena->size, 
           (double)lexer->arena->used / lexer->arena->size * 100.0);
    printf("      * String pool: %zu / %zu bytes (%.1f%%)\n",
           lexer->string_pool_used, lexer->string_pool_size,
           (double)lexer->string_pool_used / lexer->string_pool_size * 100.0);
    printf("      * Keywords loaded: %zu\n", lexer->keyword_count);
    printf("      * Tokens processed: %zu\n", lexer->tokens_processed);
    printf("      * Processing speed: %.0f tokens/sec\n", tokens_per_sec > 0 ? tokens_per_sec : 999999.0);
    printf("      * Elapsed time: %.6f seconds\n", elapsed_time);
    if (lexer->has_error) {
        printf("      * Status: [ERROR] Error state\n");
    } else {
        printf("      * Status: [OK] Operational\n");
    }
}

// String interning for memory efficiency
const char* lexer_intern_string(Lexer* lexer, const char* str, size_t length) {
    if (!lexer || !str || length == 0) return NULL;
    
    // Check if we have enough space
    if (lexer->string_pool_used + length + 1 > lexer->string_pool_size) {
        return NULL; // Pool full
    }
    
    // Simple interning - in a real implementation, you'd use a hash table
    char* interned = lexer->string_pool + lexer->string_pool_used;
    memcpy(interned, str, length);
    interned[length] = '\0';
    lexer->string_pool_used += length + 1;
    
    return interned;
}

// Performance measurement
double lexer_get_tokens_per_second(const Lexer* lexer) {
    if (!lexer) return 0.0;
    
    double current_time = (double)clock() / CLOCKS_PER_SEC;
    double elapsed_time = current_time - lexer->start_time;
    return elapsed_time > 0 ? lexer->tokens_processed / elapsed_time : 0.0;
}

// Reset lexer position for backtracking
void lexer_reset_position(Lexer* lexer, const char* position) {
    if (!lexer || !position) return;
    
    lexer->current = position;
    lexer->start = position;
    // Note: Position tracking would need more sophisticated implementation for full reset
}

// Peek next token without consuming it
Token lexer_peek_token(Lexer* lexer) {
    if (!lexer) {
        Token error = {0};
        error.type = TOKEN_ERROR;
        return error;
    }
    
    // Save current state
    const char* saved_current = lexer->current;
    const char* saved_start = lexer->start;
    Position saved_pos = lexer->pos;
    size_t saved_tokens = lexer->tokens_processed;
    bool saved_error = lexer->has_error;
    
    // Get next token
    Token token = lexer_next_token(lexer);
    
    // Restore state
    lexer->current = saved_current;
    lexer->start = saved_start;
    lexer->pos = saved_pos;
    lexer->tokens_processed = saved_tokens;
    lexer->has_error = saved_error;
    
    return token;
}
