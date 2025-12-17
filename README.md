# mini-scheme

`A minimal Scheme interpreter in C/C++ built using standard tools like flex/bison

## Usage

This is a minimal Scheme language interpreter. It supports a very limited number of operations/procedures, such as
+, -, \*, %, =, <, > (plus, minus, multiply, divide, equality comparison, inequality comparisons), logical
operators `if`, `and`, `or`, and the `print` function.

Its only supported types are integers/bools, i.e. no need to support strings.

Math operators and `print` work in eager (applicative) order, evaluating all arguments before passing, while
`if`, `and`, and `or` process their arguments lazily.

Lastly, the `define` keyword can be used to make functions. For example this is a square function:
```
(define (square x)
  (* x x))
```

As shortcut to limit support of different types, the only type is integer, 0 represents false, and 1 is true, and every operation including
`print` returns something (`print` returns 1)

The interpreter either operates in REPL mode by default, or executes a program if passed as an argument.

## Implementation

The parsing functionality should be implemented using standard native parsing tools such as `flex` and `bison` or `yacc`.

The build process should use `make`
