# nyasm

### Clean:
```sh
make clean
```

### Build
```sh
make build arch=64
```

### Compile
```sh
make run target=nyasm
```

### Run
```sh
./nyasm input output
```


### Architecture
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

### 3rd party libs
[utf8proc](https://github.com/JuliaStrings/utf8proc) - For character parsing, normalization and unicode conversion

[argparse](https://github.com/cofyc/argparse) + [positional arguments](https://github.com/cofyc/argparse/pull/61) - For argument handling and help menu