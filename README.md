# Lexical Analyzer

> [!IMPORTANT]
> This repository contains the code used for Project 1 (PA1) of my CS280 class

This is a lexical analyzer for a "Mini C-Like (MCL)" language, as defined below.
It takes in a file to perform the lexical analysis on as well as flags to display supporting information.

# Building

To build this project, download the project and compile `main.cpp`, `lex.cpp`, and `lex.h` with C++11.

For example, with `gcc`

```bash
g++ -std=c++11 *.cpp -o a
```

## Usage

Assuming the compiled executable is named `a`

```bash
./a <filename> [flags]
```

Where only one `filename` can be provided, and optional flags can be provided

### Test Cases

If using the provided test cases, use the command

```bash
./a tests/<test> [flags]
```

Where `test` is the filename of the file you wish to test

> [!NOTE]
> You can compare the test cases to those with the same name but have `.correct` appended to it.
> The output of the program and the text within the correct file should be identical.

### Optional Flags

`-all`

> Prints each token to the console as it is read

`-id`

> Prints a sorted list of identifiers after traversing the file

`-kw`

> Prints a sorted list of keywords after traversing the file

`-num`

> Prints a sorted list of integers and real constants after traversing the file

`-str`

> Prints a sorted list of string constants after traversing the file

`-char`

> Prints a sorted list of character constants after traversing the file

`-bool`

> Prints a sorted list of boolean constants after traversing the file

These methods are not a part of the lexer but rather the supporting driver program to display this information

## Project Outline

`main.cpp`

> The driver of the program which handles the input flags and collecting the tokens to be displayed.
> It calls `getNextToken()` on the input file repeatively to extract its tokens.

`lex.h`

> The header file that contains information about the tokens the lexer can use and the functions implemented in `lex.cpp`

`lex.cpp`

> The actual lexer itself, `getNextToken()` takes in a stream of characters and returns the next token.
> It also contains helper functions like `operator<<()` to display tokens and `id_or_kws()` to determine if a given lexeme is a keyword or identifier

`tests/`

> A folder that contains files used for testing and grading the lexer.
> Files are named for the things they test with a matching file ending with `.correct` that contains the "correct" response of the lexer

# Lexer Description

A lexer, or lexical analysis, is the first step in the compilation process.

A lexer takes in a stream of characters and breaks it apart into chunks called "Tokens" that the compiler has an easier time dealing with.
From there, it is given to the parser to continue compiling.

## Tokens

Here is a comprehensive list of all tokens recognized by the lexer

### Identifiers

Identifiers (or variables) are defined as:

`IDENT :: ( [a-z A-Z] | _ ) ( [a-z A-Z] | [0-9] | _ )*`

### Integer Constants

Integers are defined as:

`ICONST ::= [+ - ]? [0-9]+`

### Real Constants

Real constants/floats are defined as:

`RCONST ::= [+ -]? [0-9]* \. [0-9]+`

### String Constants

String constants are defined as:

`SCONST ::= "[a-z A-Z]*"`

### Character Constants

Character constants are defined as:

`CCONST ::= '[a-z A-Z]?'`

### Boolean Constants

Boolean constants are defined as:

`BCONST ::= TRUE | FALSE`

### Keywords

Keywords, or "reserved words", cannot be names of identifiers but rather are converted to these tokens

These are NOT case sensitive

| Keywords | Token   |
| -------- | ------- |
| program  | PROGRAM |
| string   | STRING  |
| else     | ELSE    |
| if       | IF      |
| int      | INT     |
| float    | FLOAT   |
| char     | CHAR    |
| print    | PRINT   |
| bool     | BOOL    |
| true     | TRUE    |
| false    | FALSE   |

### Operators

| Operator | Token    | Description                          |
| -------- | -------- | ------------------------------------ |
| +        | PLUS     | Arithmetic addition or concatenation |
| -        | MINUS    | Arithmetic subtraction               |
| \*       | MULT     | Multiplication                       |
| /        | DIV      | Division                             |
| =        | ASSOP    | Assignment operator                  |
| ==       | EQ       | Equality                             |
| !=       | NEQ      | Not Equality                         |
| <        | LTHAN    | Less than operator                   |
| >        | GTHAN    | Greater than operator                |
| %        | REM      | Remainder/Modulus                    |
| &&       | AND      | Logical And                          |
| \|\|     | OR       | Logical Or                           |
| !        | NOT      | Logical Complement                   |
| +=       | ADDASSOP | Addition and assignment              |
| -=       | SUBASSOP | Subtraction and assignment           |
| \*=      | MULASSOP | Multiplication and assignment        |
| /=       | DIVASSOP | Division and assignment              |
| %=       | REMASSOP | Remainder/modulus and assignment     |

### Delimiters

| Delimiter | Token   | Description       |
| --------- | ------- | ----------------- |
| ,         | COMMA   | Comma             |
| ;         | SEMICOL | Semi-colon        |
| (         | LPAREN  | Left Parenthesis  |
| )         | RPAREN  | Right Parenthesis |
| {         | LBRACE  | Left Brace        |
| }         | RBRACE  | Right Brace       |
| .         | DOT     | Dot               |

### Comments

- Single line comments are defined to be all characters followed by `//` on a single line
- Block comments are defined by all characters after `/*` and are termined by `*/`
- No tokens exist for comments, and they are completely skipped by the lexer

### Extra Tokens

- The `ERR` token denotes that an error has occured
- The `DONE` token denotes that the lexer has reached the end of the file

## Errors

TODO
Maybe also talk about the edge cases that were missed
