#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"
#include <stdio.h>

// ================== CODE GENERATION STRUCTURES ==================

typedef enum {
    OUTPUT_C,           // Generate C code
    OUTPUT_JAVASCRIPT,  // Generate JavaScript
    OUTPUT_PYTHON,      // Generate Python
    OUTPUT_BYTECODE     // Generate custom bytecode
} OutputFormat;

typedef struct {
    FILE* output_file;      // Output file
    OutputFormat format;    // Output format
    int indent_level;       // Current indentation
    bool had_error;         // Error flag
    char error_message[256]; // Error message
    
    // Code generation statistics
    int lines_generated;    // Lines of code generated
    int variables_declared; // Number of variables
    int functions_generated; // Number of functions
    double gen_start_time;  // Generation start time
} CodeGenerator;

// ================== CODE GENERATOR FUNCTIONS ==================

// Core functions
CodeGenerator* codegen_create(const char* output_filename, OutputFormat format);
void codegen_destroy(CodeGenerator* codegen);
bool codegen_generate(CodeGenerator* codegen, const ASTNode* ast);

// Error handling
bool codegen_has_error(const CodeGenerator* codegen);
const char* codegen_get_error(const CodeGenerator* codegen);

// Statistics
int codegen_get_lines_generated(const CodeGenerator* codegen);
double codegen_get_generation_time(const CodeGenerator* codegen);

#endif
