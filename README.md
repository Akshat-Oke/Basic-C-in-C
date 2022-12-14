# Basic-C-in-C - TOC Assignment 2

This is an interpreter written in C for a small language named "Basic C" that has variable declarations, assignments, for loop, read anda write statements.

Here is an example program in Basic C:

```c
int a, b, c;
a = 4;
read b;
c = (a+b)*7/b;
for(; b > a; a=a+1){
  write c;
  c = c - a/b;
};
```

The grammar and other details about the implementation are in the PDF.

## Building and running

To compile, run `make`.
To run, enter your program in `input.txt` and run `./bin/main`.

### Options

Use flag `-u` to enable unary negation.

```bash
> ./bin/main -u
```

To run a different file, give the filename as the cmd argument.

```bash
> ./bin/main file.txt
```

Note that both `-u` and a filename cannot be given at the same time.
