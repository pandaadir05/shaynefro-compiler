#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#define ARENA_SIZE 65536
#define MAX_KEYWORDS 64
#define MAX_STRING_LITERAL 1024
#define LEXER_LOOKAHEAD 2

// ================== 2025 CUTTING-EDGE ENHANCEMENTS ==================

// 2025 ENHANCEMENT: Emoji tokens for modern coding
typedef enum {
    TOKEN_PERFORMANCE_BOOST = 200,  // 🚀
    TOKEN_FAST_EXECUTION,           // ⚡
    TOKEN_TARGET,                   // 🎯
    TOKEN_HOT_PATH,                 // 🔥
    TOKEN_PRECIOUS,                 // 💎
    TOKEN_FEATURE,                  // 🌟
    TOKEN_SECURE,                   // 🛡️
    TOKEN_AI,                       // 🧠
    TOKEN_CONFIG,                   // ⚙️
    TOKEN_ANALYTICS                 // 📊
} EmojiTokenType;

// AI-powered token prediction cache
typedef struct {
    TokenType predicted_tokens[16];
    float confidence_scores[16];
    int prediction_count;
    uint64_t neural_hash;
} AITokenPredictor;

// Real-time performance telemetry
typedef struct {
    uint64_t total_cycles;
    uint64_t cache_hits;
    uint64_t cache_misses;
    double thermal_coefficient;
    double efficiency_rating;
    time_t session_start;
    uint64_t quantum_entropy;
} PerformanceTelemetry;

// Advanced error recovery system
typedef struct {
    char suggestion[256];
    int confidence;
    int recovery_strategy;
    bool ai_generated;
} ErrorRecovery;

// Machine learning feature vector
typedef struct {
    float lexical_density;
    float semantic_complexity;
    float syntactic_depth;
    float innovation_index;
} MLFeatureVector;

// Performance-oriented arena with quantum-inspired alignment
typedef struct {
    char* memory;
    size_t size;
    size_t used;
    size_t peak_used;  // Track peak memory usage for optimization
    uint64_t quantum_entropy;  // 2025: Quantum entropy for cache optimization
    double coherence_factor;   // 2025: Memory coherence rating
} Arena;

typedef struct {
    const char* keyword;
    TokenType token_type;
    size_t length;  // Cache length for O(1) comparisons
    uint64_t hash_signature;  // 2025: Pre-computed hash for ultra-fast lookup
} Keyword;

// 2025 ULTIMATE ENHANCED LEXER with AI and Quantum Features
typedef struct {
    const char* source;
    const char* current;
    const char* start;
    const char* end;  // Cache end pointer for bounds checking
    Position pos;
    Position start_pos;  // Cache start position for tokens
    Arena* arena;
    Keyword keywords[MAX_KEYWORDS];
    size_t keyword_count;
    bool has_error;
    char error_message[256];
    
    // Performance enhancements
    char lookahead[LEXER_LOOKAHEAD];  // Character lookahead buffer
    size_t tokens_processed;  // Statistics
    double start_time;  // For performance profiling
    
    // String interning for identifiers (elite feature)
    char* string_pool;
    size_t string_pool_used;
    size_t string_pool_size;
    
    // ================== 2025 REVOLUTIONARY FEATURES ==================
    AITokenPredictor ai_predictor;        // AI-powered next token prediction
    PerformanceTelemetry telemetry;       // Real-time performance monitoring
    ErrorRecovery last_recovery;          // Smart error recovery system
    MLFeatureVector ml_features;          // Machine learning code analysis
    
    // Quantum-inspired memory management
    Arena* secondary_arena;               // Multi-level memory pools
    Arena* temp_arena;                    // Temporary computation space
    
    // Advanced debugging and profiling
    bool debug_mode;                      // Real-time debugging support
    uint64_t instruction_count;           // CPU instruction profiling
    double code_quality_score;            // AI-computed quality rating
    
    // 2025 Unicode and Emoji support
    bool emoji_mode;                      // Enable emoji tokens
    uint32_t unicode_state;               // Unicode processing state
    
    // Parallel processing support (future-ready)
    int thread_id;                        // For multi-threaded lexing
    bool parallel_mode;                   // Enable parallel tokenization
    
    // Security and integrity
    uint64_t checksum;                    // Source code integrity check
    bool secure_mode;                     // Enhanced security features
} Lexer;

// Arena functions with quantum-inspired alignment
Arena* arena_create(void);
void arena_destroy(Arena* arena);
void* arena_alloc(Arena* arena, size_t size);
void* arena_alloc_aligned(Arena* arena, size_t size, size_t alignment);
void arena_reset(Arena* arena);
size_t arena_get_usage(const Arena* arena);

// Core lexer functions
Lexer* lexer_create(const char* source, const char* filename);
void lexer_destroy(Lexer* lexer);
Token lexer_next_token(Lexer* lexer);
Token lexer_peek_token(Lexer* lexer);  // Lookahead without consuming
bool lexer_has_error(const Lexer* lexer);
const char* lexer_get_error(const Lexer* lexer);

// Performance and debugging functions
void lexer_print_stats(const Lexer* lexer);
double lexer_get_tokens_per_second(const Lexer* lexer);
void lexer_reset_position(Lexer* lexer, const char* position);

// String interning for optimal memory usage
const char* lexer_intern_string(Lexer* lexer, const char* str, size_t length);

// ================== 2025 REVOLUTIONARY API ==================

// AI-Powered Features
void lexer_enable_ai_prediction(Lexer* lexer, bool enable);
float lexer_get_prediction_confidence(const Lexer* lexer, TokenType token);
TokenType lexer_get_ai_suggestion(const Lexer* lexer);

// Real-time Performance Telemetry
void lexer_start_telemetry(Lexer* lexer);
PerformanceTelemetry lexer_get_telemetry(const Lexer* lexer);
double lexer_get_efficiency_rating(const Lexer* lexer);
uint64_t lexer_get_quantum_entropy(const Lexer* lexer);

// Advanced Error Recovery
ErrorRecovery lexer_get_smart_suggestion(const Lexer* lexer);
bool lexer_attempt_auto_recovery(Lexer* lexer);
void lexer_enable_debug_mode(Lexer* lexer, bool enable);

// Machine Learning Code Analysis
MLFeatureVector lexer_analyze_code_quality(const Lexer* lexer);
double lexer_compute_innovation_score(const Lexer* lexer);
double lexer_get_code_complexity(const Lexer* lexer);

// 2025 Unicode and Emoji Support
void lexer_enable_emoji_tokens(Lexer* lexer, bool enable);
bool lexer_is_emoji_supported(const Lexer* lexer);
const char* lexer_get_emoji_description(TokenType token);

// Quantum-Inspired Memory Management
Arena* lexer_get_secondary_arena(Lexer* lexer);
void lexer_optimize_memory_layout(Lexer* lexer);
double lexer_get_memory_coherence(const Lexer* lexer);

// Security and Integrity
uint64_t lexer_compute_checksum(const Lexer* lexer);
bool lexer_verify_integrity(const Lexer* lexer);
void lexer_enable_secure_mode(Lexer* lexer, bool enable);

// Parallel Processing (Future-Ready)
void lexer_enable_parallel_mode(Lexer* lexer, bool enable);
int lexer_get_optimal_thread_count(const Lexer* lexer);
void lexer_set_thread_id(Lexer* lexer, int thread_id);

// Advanced Profiling
uint64_t lexer_get_instruction_count(const Lexer* lexer);
double lexer_get_cache_hit_ratio(const Lexer* lexer);
void lexer_print_advanced_stats(const Lexer* lexer);

#endif
