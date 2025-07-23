#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

// ShayLang compiler - full implementation

static void compile_program(const char* source_code, const char* filename) {
    printf(">> COMPILING SHAYNEFRO PROGRAM\n");
    printf("==============================\n");
    printf("Source: %s\n\n", filename);
    
    // lexical analysis first
    printf("Phase 1: Lexical Analysis...\n");
    Lexer* lexer = lexer_create(source_code, filename);
    if (!lexer) {
        printf("[ERROR] Failed to create lexer\n");
        return;
    }
    
    // now parse into AST
    printf("Phase 2: Parsing...\n");
    Parser* parser = parser_create(lexer);
    if (!parser) {
        printf("[ERROR] Failed to create parser\n");
        lexer_destroy(lexer);
        return;
    }
    
    ASTNode* ast = parser_parse(parser);
    if (!ast || parser_has_error(parser)) {
        printf("[ERROR] Parsing failed: %s\n", parser_get_error(parser));
        parser_destroy(parser);
        lexer_destroy(lexer);
        return;
    }
    
    printf("[SUCCESS] Successfully parsed %d AST nodes\n", parser_get_nodes_created(parser));
    
    // generate C code from AST
    printf("Phase 3: Code Generation...\n");
    CodeGenerator* codegen = codegen_create("output.c", OUTPUT_C);
    if (!codegen) {
        printf("[ERROR] Failed to create code generator\n");
        parser_destroy(parser);
        lexer_destroy(lexer);
        return;
    }
    
    bool success = codegen_generate(codegen, ast);
    if (!success || codegen_has_error(codegen)) {
        printf("[ERROR] Code generation failed: %s\n", codegen_get_error(codegen));
        codegen_destroy(codegen);
        parser_destroy(parser);
        lexer_destroy(lexer);
        return;
    }
    
    printf("[SUCCESS] Successfully generated %d lines of C code\n", codegen_get_lines_generated(codegen));
    printf("[SUCCESS] Compilation complete! Generated: output.c\n\n");
    
    // show some stats
    printf(">> COMPILATION STATISTICS:\n");
    printf("   Parse time: %.4f seconds\n", parser_get_parse_time(parser));
    printf("   Codegen time: %.4f seconds\n", codegen_get_generation_time(codegen));
    printf("   AST nodes: %d\n", parser_get_nodes_created(parser));
    printf("   Output lines: %d\n", codegen_get_lines_generated(codegen));
    
    // dump the AST tree
    printf("\n>> ABSTRACT SYNTAX TREE:\n");
    ast_print(ast, 0);
    
    // cleanup everything
    codegen_destroy(codegen);
    parser_destroy(parser);
    lexer_destroy(lexer);
}

// testing functions for the lexer

static void print_header(void) {
    printf("===============================================================\n");
    printf("                      Shaynefro Compiler                      \n");
    printf("                   Modern Language Compiler                   \n");
    printf("           Complete: Lexer + Parser + CodeGen                 \n");
    printf("===============================================================\n\n");
}

static void test_lexer(const char* source, const char* description) {
    printf("-- Testing: %s\n", description);
    printf("   Source: %.60s%s\n", source, strlen(source) > 60 ? "..." : "");
    printf("   Tokens:\n");
    
    clock_t start = clock();
    Lexer* lexer = lexer_create(source, "test.shay");
    if (!lexer) {
        printf("   [ERROR] Failed to create lexer\n");
        return;
    }
    
    Token token;
    int token_count = 0;
    while (true) {
        token = lexer_next_token(lexer);
        printf("   ");
        token_print(&token);
        printf("\n");
        token_count++;
        
        if (token.type == TOKEN_ERROR) {
            printf("   [ERROR]: %s\n", lexer_get_error(lexer));
            break;
        }
        
        if (token.type == TOKEN_EOF) {
            break;
        }
        
        // don't loop forever if something goes wrong
        if (token_count > 1000) {
            printf("   [WARNING] Safety limit reached (1000 tokens)\n");
            break;
        }
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // print results if no errors
    if (lexer_has_error(lexer)) {
        printf("   [ERROR] Test failed with errors\n");
    } else {
        printf("   [SUCCESS] Success: %d tokens in %.4f seconds\n", token_count, time_taken);
        lexer_print_stats(lexer);
    }
    
    lexer_destroy(lexer);
    printf("\n");
}

static void performance_benchmark(void) {
    printf(">> Performance Benchmark\n");
    printf("========================\n");
    
    // big test program
    const char* large_program = 
        "// Shaynefro Performance Test\n"
        "function fibonacci(int n) {\n"
        "    if (n <= 1) return n;\n"
        "    return fibonacci(n-1) + fibonacci(n-2);\n"
        "}\n\n"
        "function quicksort(int[] arr, int low, int high) {\n"
        "    if (low < high) {\n"
        "        int pi = partition(arr, low, high);\n"
        "        quicksort(arr, low, pi - 1);\n"
        "        quicksort(arr, pi + 1, high);\n"
        "    }\n"
        "}\n\n"
        "function main() {\n"
        "    int n = 100;\n"
        "    float result = 0.0;\n"
        "    string message = \"Computing...\";\n"
        "    \n"
        "    for (int i = 0; i < n; i++) {\n"
        "        result += fibonacci(i) * 3.14159;\n"
        "        if (result > 1000000.0) {\n"
        "            break;\n"
        "        }\n"
        "    }\n"
        "    \n"
        "    return 0;\n"
        "}\n";
    
    printf("Tokenizing %zu characters...\n", strlen(large_program));
    
    clock_t start = clock();
    Lexer* lexer = lexer_create(large_program, "benchmark.shay");
    
    int total_tokens = 0;
    Token token;
    while (true) {
        token = lexer_next_token(lexer);
        total_tokens++;
        
        if (token.type == TOKEN_EOF || token.type == TOKEN_ERROR) {
            break;
        }
        
        // safety check
        if (total_tokens > 10000) {
            printf("   [WARNING] Benchmark safety limit reached\n");
            break;
        }
    }
    
    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    double tokens_per_sec = total_tokens / time_taken;
    
    printf(">> Results:\n");
    printf("   Tokens processed: %d\n", total_tokens);
    printf("   Time taken: %.6f seconds\n", time_taken);
    printf("   Speed: %.0f tokens/second\n", tokens_per_sec);
    printf("   Throughput: %.2f KB/second\n", (strlen(large_program) / 1024.0) / time_taken);
    
    lexer_destroy(lexer);
    printf("\n");
}

static void interactive_mode(void) {
    printf(">> Interactive Shaynefro Mode\n");
    printf("=============================\n");
    printf("Enter Shaynefro code (type 'quit' to exit):\n\n");
    
    char input[1024];
    while (1) {
        printf("shay> ");
        fflush(stdout);
        
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }
        
        // strip newline
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "quit") == 0) {
            break;
        }
        
        if (strlen(input) == 0) {
            continue;
        }
        
        Lexer* lexer = lexer_create(input, "interactive");
        Token token;
        printf("Tokens: ");
        do {
            token = lexer_next_token(lexer);
            if (token.type != TOKEN_EOF && token.type != TOKEN_NEWLINE) {
                printf("[%s] ", token_type_to_string(token.type));
            }
        } while (token.type != TOKEN_EOF);
        printf("\n");
        
        if (lexer_has_error(lexer)) {
            printf("Error: %s\n", lexer_get_error(lexer));
        }
        
        lexer_destroy(lexer);
    }
    
    printf("Goodbye!\n");
}

int main(int argc, char* argv[]) {
    print_header();
    
    // handle command line args
    if (argc == 2 && strcmp(argv[1], "-i") == 0) {
        interactive_mode();
        return 0;
    }
    
    if (argc == 2 && strcmp(argv[1], "-b") == 0) {
        performance_benchmark();
        return 0;
    }
    
    if (argc == 2 && strcmp(argv[1], "-c") == 0) {
        // full compiler test
        const char* sample_program = 
            "int x = 42;\n"
            "int y = x + 10;\n"
            "int result = x * y;\n"
            "return result;\n";
        
        compile_program(sample_program, "sample.shay");
        return 0;
    }
    
    if (argc == 3 && strcmp(argv[1], "-f") == 0) {
        // compile from file
        FILE* file = fopen(argv[2], "r");
        if (!file) {
            printf("[ERROR] Cannot open file: %s\n", argv[2]);
            return 1;
        }
        
        // read the whole file
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        char* content = malloc(file_size + 1);
        fread(content, 1, file_size, file);
        content[file_size] = '\0';
        fclose(file);
        
        compile_program(content, argv[2]);
        free(content);
        return 0;
    }
    
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        printf(">> Shaynefro - Modern Programming Language Compiler\n\n");
        printf("Usage:\n");
        printf("  %s        - Run lexer test suite\n", argv[0]);
        printf("  %s -i     - Interactive mode\n", argv[0]);
        printf("  %s -b     - Performance benchmark\n", argv[0]);
        printf("  %s -c     - Compile sample program (FULL COMPILER)\n", argv[0]);
        printf("  %s -f <file> - Compile specific file\n", argv[0]);
        printf("  %s -h     - Show this help\n", argv[0]);
        printf("\n>> Features:\n");
        printf("  * High-performance lexical analysis\n");
        printf("  * Complete recursive descent parser\n");
        printf("  * C code generation\n");
        printf("  * 46+ language keywords\n");
        printf("  * Zero memory leaks\n");
        printf("  * Professional error reporting\n");
        return 0;
    }
    
    // run all the tests
    printf(">> Comprehensive Test Suite\n");
    printf("============================\n");
    
    // basic stuff
    test_lexer("( ) { } [ ] ; , .", "Delimiters");
    
    // operators
    test_lexer("+ - * / % = == != < <= > >= && || !", "Operators");
    
    // numbers
    test_lexer("42 3.14159 0 999.999", "Numbers");
    
    // strings
    test_lexer("\"hello\" \"world\" \"Hello, ShayLang!\"", "Strings");
    
    // keywords and identifiers
    test_lexer("int float string if else while for return function var", "Keywords");
    test_lexer("variable _private myVar x1 y2", "Identifiers");
    
    // full program test
    test_lexer(
        "function factorial(int n) {\n"
        "    if (n <= 1) {\n"
        "        return 1;\n"
        "    } else {\n"
        "        return n * factorial(n - 1);\n"
        "    }\n"
        "}",
        "Complete Function"
    );
    
    // comment handling
    test_lexer(
        "int x = 42; // line comment\n"
        "/* block comment */\n"
        "int y = 24;",
        "Comments"
    );
    
    // fancy number formats
    test_lexer("0x1A 0b1010 0o777", "Advanced Number Formats");
    
    test_lexer(
        "class Matrix {\n"
        "    private float[4][4] data;\n"
        "    public Matrix multiply(Matrix other) {\n"
        "        // Implementation\n"
        "    }\n"
        "}",
        "Advanced Language Features"
    );
    
    test_lexer("\"string with \\\"escapes\\\" and \\n newlines\"", "String Escapes");
    
    // error cases to make sure they fail properly
    test_lexer("\"unterminated string", "Error Case - Unterminated String");
    test_lexer("@#$", "Error Case - Invalid Characters");
    
    printf(">> Running Performance Benchmark...\n");
    performance_benchmark();
    
    printf("===============================================================\n");
    printf("                    Testing Complete!                         \n");
    printf("                                                               \n");
    printf("  >> To run:                                                   \n");
    printf("     make && ./shaynefro                                       \n");
    printf("     ./shaynefro -i    (interactive mode)                     \n");
    printf("     ./shaynefro -b    (benchmark mode)                       \n");
    printf("===============================================================\n");
    
    return 0;
}
