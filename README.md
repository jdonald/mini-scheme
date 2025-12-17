# mini-scheme

A minimal Scheme interpreter in C/C++ built using standard tools like flex/bison

## Features

This is a minimal Scheme language interpreter supporting:

- **Arithmetic operators**: `+`, `-`, `*`, `/`, `%`
- **Comparison operators**: `=`, `<`, `>`
- **Logical operators**: `if`, `and`, `or`
- **Print function**: `print`
- **Function definitions**: `define`

### Type System

The interpreter uses a simplified type system:
- Only **integers** are supported (no strings, floats, or other types)
- **0** represents false
- **1** represents true (or any non-zero value)
- All operations return a value (`print` returns 1)

### Evaluation Order

- Math operators and `print` use **eager (applicative) evaluation** - all arguments are evaluated before the operation
- `if`, `and`, and `or` use **lazy evaluation** - arguments are evaluated only as needed

## Prerequisites

Before building, you need to install flex and bison:

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install flex bison g++ make
```

### macOS
```bash
brew install flex bison
```

### Fedora/RHEL
```bash
sudo dnf install flex bison gcc-c++ make
```

## Building

Build the interpreter using make:

```bash
make
```

This will:
1. Generate the parser from `parser.y` using bison
2. Generate the lexer from `lexer.l` using flex
3. Compile all C++ source files
4. Link the final executable `mini-scheme`

To clean build artifacts:
```bash
make clean
```

## Usage

### REPL Mode

Run the interpreter without arguments to start an interactive REPL:

```bash
./mini-scheme
```

Example session:
```
mini-scheme REPL (type expressions and press Enter, Ctrl+D to exit)
> (+ 1 2 3)
6
> (* 4 5)
20
> (define (square x) (* x x))
1
> (square 9)
81
> (if (< 3 5) (print 100) (print 200))
100
1
```

Press Ctrl+D to exit the REPL.

### File Execution Mode

Run a Scheme program from a file:

```bash
./mini-scheme program.scm
```

## Examples

### Basic Arithmetic

```scheme
(print (+ 1 2 3))          ; Output: 6
(print (* 4 5))            ; Output: 20
(print (- 10 3))           ; Output: 7
(print (/ 20 4))           ; Output: 5
(print (% 17 5))           ; Output: 2
```

### Comparisons

```scheme
(print (= 5 5))            ; Output: 1 (true)
(print (< 3 5))            ; Output: 1 (true)
(print (> 5 3))            ; Output: 1 (true)
(print (= 3 5))            ; Output: 0 (false)
```

### Conditionals

```scheme
(if (< 3 5)
  (print 100)
  (print 200))             ; Output: 100

(print (and (< 1 2) (> 5 3)))   ; Output: 1 (true)
(print (or 0 (< 3 5)))          ; Output: 1 (true)
```

### Function Definitions

Define and call a square function:
```scheme
(define (square x)
  (* x x))

(print (square 7))         ; Output: 49
```

Recursive factorial function:
```scheme
(define (factorial n)
  (if (= n 0)
    1
    (* n (factorial (- n 1)))))

(print (factorial 5))      ; Output: 120
```

### Complete Example Program

Create a file `example.scm`:
```scheme
; Define a square function
(define (square x)
  (* x x))

; Define a factorial function
(define (factorial n)
  (if (= n 0)
    1
    (* n (factorial (- n 1)))))

; Test the functions
(print (square 7))
(print (factorial 5))

; Test conditionals
(if (< 3 5) (print 100) (print 200))
```

Run it:
```bash
./mini-scheme example.scm
```

Output:
```
49
120
100
```

## Implementation Details

The interpreter consists of:

- **lexer.l**: Flex lexical analyzer for tokenization
- **parser.y**: Bison parser for building Abstract Syntax Trees (AST)
- **interpreter.h/cpp**: Evaluation engine and environment management
- **main.cpp**: Entry point supporting both REPL and file execution
- **Makefile**: Build configuration

The implementation uses:
- C++11 standard
- flex for lexical analysis
- bison for parsing
- Smart pointers for memory management
