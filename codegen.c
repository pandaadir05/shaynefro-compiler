#include "codegen.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ================== CODE GENERATOR CREATION ==================

CodeGenerator* codegen_create(const char* output_filename, OutputFormat format) {
    CodeGenerator* codegen = malloc(sizeof(CodeGenerator));
    if (!codegen) return NULL;
    
    codegen->output_file = fopen(output_filename, "w");
    if (!codegen->output_file) {
        free(codegen);
        return NULL;
    }
    
    codegen->format = format;
    codegen->indent_level = 0;
    codegen->had_error = false;
    codegen->error_message[0] = '\0';
    codegen->lines_generated = 0;
    codegen->variables_declared = 0;
    codegen->functions_generated = 0;
    codegen->gen_start_time = (double)clock() / CLOCKS_PER_SEC;
    
    return codegen;
}

void codegen_destroy(CodeGenerator* codegen) {
    if (codegen) {
        if (codegen->output_file) {
            fclose(codegen->output_file);
        }
        free(codegen);
    }
}

// ================== UTILITY FUNCTIONS ==================

// Forward declarations
static void generate_c_expression(CodeGenerator* codegen, const ASTNode* node);

static void emit_indent(CodeGenerator* codegen) {
    for (int i = 0; i < codegen->indent_level; i++) {
        fprintf(codegen->output_file, "    ");
    }
}

static void emit_line(CodeGenerator* codegen, const char* line) {
    emit_indent(codegen);
    fprintf(codegen->output_file, "%s\n", line);
    codegen->lines_generated++;
}

static void codegen_error(CodeGenerator* codegen, const char* message) {
    codegen->had_error = true;
    strncpy(codegen->error_message, message, sizeof(codegen->error_message) - 1);
}

// ================== C CODE GENERATION ==================

static void generate_c_literal(CodeGenerator* codegen, const ASTNode* node) {
    switch (node->data.literal.token_type) {
        case TOKEN_INTEGER:
            fprintf(codegen->output_file, "%lld", node->data.literal.value.int_value);
            break;
        case TOKEN_FLOAT:
            fprintf(codegen->output_file, "%g", node->data.literal.value.float_value);
            break;
        case TOKEN_STRING:
            fprintf(codegen->output_file, "\"%s\"", node->data.literal.value.string_value);
            break;
        case TOKEN_TRUE:
            fprintf(codegen->output_file, "true");
            break;
        case TOKEN_FALSE:
            fprintf(codegen->output_file, "false");
            break;
        case TOKEN_NULL:
            fprintf(codegen->output_file, "NULL");
            break;
        default:
            codegen_error(codegen, "Unknown literal type");
            break;
    }
}

static void generate_c_identifier(CodeGenerator* codegen, const ASTNode* node) {
    fprintf(codegen->output_file, "%s", node->data.identifier.name);
}

static void generate_c_binary(CodeGenerator* codegen, const ASTNode* node) {
    fprintf(codegen->output_file, "(");
    generate_c_expression(codegen, node->data.binary.left);
    
    switch (node->data.binary.operator) {
        case TOKEN_PLUS: fprintf(codegen->output_file, " + "); break;
        case TOKEN_MINUS: fprintf(codegen->output_file, " - "); break;
        case TOKEN_MULTIPLY: fprintf(codegen->output_file, " * "); break;
        case TOKEN_DIVIDE: fprintf(codegen->output_file, " / "); break;
        case TOKEN_MODULO: fprintf(codegen->output_file, " %% "); break;
        case TOKEN_EQUAL: fprintf(codegen->output_file, " == "); break;
        case TOKEN_NOT_EQUAL: fprintf(codegen->output_file, " != "); break;
        case TOKEN_LESS: fprintf(codegen->output_file, " < "); break;
        case TOKEN_LESS_EQUAL: fprintf(codegen->output_file, " <= "); break;
        case TOKEN_GREATER: fprintf(codegen->output_file, " > "); break;
        case TOKEN_GREATER_EQUAL: fprintf(codegen->output_file, " >= "); break;
        case TOKEN_AND: fprintf(codegen->output_file, " && "); break;
        case TOKEN_OR: fprintf(codegen->output_file, " || "); break;
        case TOKEN_ASSIGN: fprintf(codegen->output_file, " = "); break;
        default:
            codegen_error(codegen, "Unknown binary operator");
            break;
    }
    
    generate_c_expression(codegen, node->data.binary.right);
    fprintf(codegen->output_file, ")");
}

static void generate_c_expression(CodeGenerator* codegen, const ASTNode* node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_LITERAL:
            generate_c_literal(codegen, node);
            break;
        case AST_IDENTIFIER:
            generate_c_identifier(codegen, node);
            break;
        case AST_BINARY:
        case AST_ASSIGNMENT:
            generate_c_binary(codegen, node);
            break;
        default:
            codegen_error(codegen, "Unknown expression type");
            break;
    }
}

static void generate_c_var_declaration(CodeGenerator* codegen, const ASTNode* node) {
    emit_indent(codegen);
    
    // Convert ShayLang types to C types
    switch (node->data.var_decl.type) {
        case TOKEN_INT:
            fprintf(codegen->output_file, "int ");
            break;
        case TOKEN_FLOAT_KW:
            fprintf(codegen->output_file, "double ");
            break;
        case TOKEN_STRING_KW:
            fprintf(codegen->output_file, "char* ");
            break;
        case TOKEN_BOOL_KW:
            fprintf(codegen->output_file, "bool ");
            break;
        default:
            fprintf(codegen->output_file, "int ");
            break;
    }
    
    fprintf(codegen->output_file, "%s", node->data.var_decl.name);
    
    if (node->data.var_decl.initializer) {
        fprintf(codegen->output_file, " = ");
        generate_c_expression(codegen, node->data.var_decl.initializer);
    }
    
    fprintf(codegen->output_file, ";\n");
    codegen->lines_generated++;
    codegen->variables_declared++;
}

static void generate_c_statement(CodeGenerator* codegen, const ASTNode* node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_VAR_DECLARATION:
            generate_c_var_declaration(codegen, node);
            break;
        case AST_EXPRESSION_STMT:
            emit_indent(codegen);
            generate_c_expression(codegen, node->data.binary.left);
            fprintf(codegen->output_file, ";\n");
            codegen->lines_generated++;
            break;
        case AST_RETURN_STMT:
            emit_indent(codegen);
            fprintf(codegen->output_file, "return");
            if (node->data.return_stmt.value) {
                fprintf(codegen->output_file, " ");
                generate_c_expression(codegen, node->data.return_stmt.value);
            }
            fprintf(codegen->output_file, ";\n");
            codegen->lines_generated++;
            break;
        default:
            codegen_error(codegen, "Unknown statement type");
            break;
    }
}

static void generate_c_program(CodeGenerator* codegen, const ASTNode* node) {
    // Generate C headers
    emit_line(codegen, "#include <stdio.h>");
    emit_line(codegen, "#include <stdlib.h>");
    emit_line(codegen, "#include <stdbool.h>");
    emit_line(codegen, "#include <string.h>");
    emit_line(codegen, "");
    emit_line(codegen, "int main() {");
    
    codegen->indent_level++;
    
    // Generate all statements
    for (int i = 0; i < node->data.program.statement_count; i++) {
        generate_c_statement(codegen, node->data.program.statements[i]);
    }
    
    // Add return 0 if no explicit return
    emit_line(codegen, "return 0;");
    
    codegen->indent_level--;
    emit_line(codegen, "}");
}

// ================== MAIN CODE GENERATION FUNCTION ==================

bool codegen_generate(CodeGenerator* codegen, const ASTNode* ast) {
    if (!codegen || !ast) return false;
    
    switch (codegen->format) {
        case OUTPUT_C:
            generate_c_program(codegen, ast);
            break;
        case OUTPUT_JAVASCRIPT:
            // Could implement JavaScript generation here
            codegen_error(codegen, "JavaScript output not implemented yet");
            return false;
        case OUTPUT_PYTHON:
            // Could implement Python generation here
            codegen_error(codegen, "Python output not implemented yet");
            return false;
        case OUTPUT_BYTECODE:
            // Could implement bytecode generation here
            codegen_error(codegen, "Bytecode output not implemented yet");
            return false;
    }
    
    return !codegen->had_error;
}

// ================== UTILITY FUNCTIONS ==================

bool codegen_has_error(const CodeGenerator* codegen) {
    return codegen->had_error;
}

const char* codegen_get_error(const CodeGenerator* codegen) {
    return codegen->error_message;
}

int codegen_get_lines_generated(const CodeGenerator* codegen) {
    return codegen->lines_generated;
}

double codegen_get_generation_time(const CodeGenerator* codegen) {
    double current_time = (double)clock() / CLOCKS_PER_SEC;
    return current_time - codegen->gen_start_time;
}
