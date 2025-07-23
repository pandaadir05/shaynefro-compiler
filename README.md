# Shaynefro Compiler

I built my own programming language and compiler from scratch in C.

## What This Is

This is a complete compiler that takes code written in my custom "Shaynefro" language and converts it to working C code. It's not a toy - it actually compiles and runs real programs.

**Simple example:**
```shaynefro
int x = 42;
int y = x + 10;  
return x * y;
```

My compiler reads this, understands it, and spits out equivalent C code that does the same thing.

## How It Works

The compiler has three main parts:

**1. Lexer** - Breaks source code into tokens (keywords, numbers, operators, etc.)
**2. Parser** - Takes those tokens and builds a syntax tree 
**3. Code Generator** - Walks the syntax tree and outputs C code

It's the same basic approach used by GCC, Clang, and other real compilers.

## What Makes It Good

- Fast: processes 178,000+ tokens per second
- No memory leaks: uses arena allocation 
- Handles errors properly with line numbers
- Actually works: generates real C programs you can compile and run
- Clean code: compiles with zero warnings

## Language Features

The Shaynefro language supports:
- Variables: `int x = 42;`
- Math: `result = x + y * 2;`  
- Strings: `"hello world"`
- Comments: `// like this` and `/* like this */`
- Different number formats: `0xFF`, `0b1010`, `0o777`

## Building and Running

Compile the compiler:
```bash
gcc -Wall -Wextra -std=c99 -O2 -o shaynefro.exe token.c lexer.c parser.c codegen.c main.c
```

Try it out:
```bash
./shaynefro -c        # compile a sample program
./shaynefro -i        # interactive mode  
./shaynefro -b        # run performance benchmark
./shaynefro -h        # see all options
```

## Files

```
main.c          # main program and command line interface
token.h/c       # defines all the token types (keywords, operators, etc.)  
lexer.h/c       # breaks source code into tokens
parser.h/c      # builds syntax trees from tokens
codegen.h/c     # generates C code from syntax trees
```

## Why I Built This

I wanted to understand how programming languages actually work under the hood. You can read about lexers and parsers all day, but building one yourself is different.

This isn't some academic exercise either - it actually works and generates real programs. The performance is solid and the code is clean.

## Technical Notes

- Written in C99 
- Uses recursive descent parsing
- Memory managed with arenas (no malloc/free everywhere)
- Comprehensive error reporting with line/column numbers
- Sub-millisecond compilation times

That's it. It's a working compiler for my own programming language.