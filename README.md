# nyasm

A toy-language assembler I developed for my 12th grade school project.

The assembler has a python-like indentation-style syntax with support for all common assembly directives, macros, and data structures. The assembler is able to target 6502 assembly and 8086 / 80386 assembly and also has support for a simple scripting language.
## Compile nyasm

Run in order

### Clean:
```sh
make clean
```

### Build
```sh
make build arch=64
```

or

```sh
make build arch=32
```
### Compile
```sh
make run target=nyasm
```
or compile only certain targets:
```sh
make run target=core
make run target=impl
```
### Run
```sh
./nyasm input output
```

## Help
```sh
./nyasm --help
```
```
Usage: nyasm [options] [[--] args] input output
   or: nyasm [options]

    -h, --help            show this help message and exit

Assembler parameters:
    -v, --version         print the version number
    -l, --langs           print a list of installed languages
    -m, --memory=<unint>  assembler's memory size in Kb
    input=<str>           input file path
    output=<str>          output file path

nyasm v1.0.0 - 64 bit - built on: Jul  2 2026 13:19:04
```

## Architecture
```
+-----------------------------------------+
|                                         |
|  nyasm -----------------------|         |
|    |                          | (uses)  |
|    V (uses)                   V         |
|  Assembler(core) <-(uses)-- langs       |
|    |                                    |
|    V (uses)                             |
|  implementations                        |
|                                         |
+-----------------------------------------+
```

## 3rd party libs
[utf8proc](https://github.com/JuliaStrings/utf8proc) - For character parsing, normalization and unicode conversion

[argparse](https://github.com/cofyc/argparse) + [positional arguments](https://github.com/cofyc/argparse/pull/61) - For argument handling and help menu

## How to use

There are plenty of examples, both for 6502 and 8086. There are also examples that dont use assembly at all and use the internal interpreted language
`./nyasm examples/snake.asm out.bin`

`qemu-system-i386.exe -drive format=raw,file=out.bin`

## How to add new languages

Languages are added by implementing:

```c
ASM_LANG _6502 = {
    .lang_name = "6502 assembly v1.0.0",
    .code_name = "_6502",
    .max_addr = 0Xffff,
    .size_of_ptr = 2, // in bytes
    .le = 1, // is little endian?

    .exec = _6502_exec,
};
```

Then add the target when compiling and linking
