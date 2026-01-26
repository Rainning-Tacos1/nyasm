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
./nyasm
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