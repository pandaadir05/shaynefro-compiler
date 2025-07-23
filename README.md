# Shaynefro Compiler

A complete 3-phase programming language compiler built from scratch, featuring lexical analysis, recursive descent parsing, and C code generation.

## Overview

Shaynefro is a modern programming language compiler that demonstrates the fundamental principles of compiler construction. This project implements a full compilation pipeline that transforms source code written in the Shaynefro language into executable C code.

## Architecture

The compiler follows the traditional 3-phase design:

**Phase 1: Lexical Analysis**
- Tokenizes source code into meaningful symbols
- Handles 46+ language keywords and operators
- Supports integers, floats, strings, and identifiers
- Includes comprehensive error reporting

**Phase 2: Parsing**
- Recursive descent parser with full grammar support
- Builds Abstract Syntax Trees (AST) for program representation
- Handles expressions, statements, variable declarations, and functions
- Robust error recovery and reporting

**Phase 3: Code Generation**
- Translates AST into clean, readable C code
- Maintains proper variable scoping and type information
- Generates efficient output suitable for compilation with GCC

## Features

- High-performance lexical analysis
- Complete recursive descent parser
- C code generation backend
- Interactive mode for testing
- Performance benchmarking
- Comprehensive error reporting
- Zero memory leaks with arena-based allocation
- Professional command-line interface

## Building

```bash
gcc -Wall -Wextra -std=c99 -O2 -o shaynefro.exe token.c lexer.c parser.c codegen.c main.c
```

## Usage

**Compile a sample program:**
```bash
./shaynefro -c
```

**Interactive mode:**
```bash
./shaynefro -i
```

**Performance benchmark:**
```bash
./shaynefro -b
```

**Compile from file:**
```bash
./shaynefro -f program.shay
```

**Help:**
```bash
./shaynefro -h
```

## Language Features

The Shaynefro language supports:

- Variable declarations (`int x = 42;`)
- Arithmetic expressions (`x + y * 2`)
- Function definitions
- Control flow statements
- String literals with escape sequences
- Single-line and block comments
- Advanced number formats (decimal, hexadecimal, binary, octal)

## Project Structure

```
shaynefro-compiler/
├── main.c          # Main program and CLI interface
├── token.h/c       # Token definitions and utilities
├── lexer.h/c       # Lexical analyzer implementation
├── parser.h/c      # Recursive descent parser
├── codegen.h/c     # C code generation backend
└── shaynefro.exe   # Compiled executable
```

## Development

This compiler was developed as a learning project to understand the fundamental concepts of programming language implementation. The goal was to create a complete, working compiler that demonstrates best practices in:

- Compiler design and architecture
- Memory management and performance optimization
- Error handling and user experience
- Clean, maintainable code structure

## Technical Details

- **Language:** C99 standard
- **Architecture:** Traditional 3-phase compiler design
- **Memory Management:** Arena-based allocation for zero leaks
- **Performance:** Sub-millisecond compilation times
- **Error Handling:** Comprehensive diagnostics with line/column information
- **Testing:** Built-in test suite and benchmarking tools

## License

This project is released under the MIT License. Feel free to study, modify, and learn from the code.

## Acknowledgments

Built with a focus on educational value and clean implementation. The compiler demonstrates core concepts taught in computer science compiler courses while maintaining production-quality code standards.