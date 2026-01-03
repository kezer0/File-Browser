# File Search CLI (C++20)

A simple command-line tool written in C++ that recursively searches for files by name (partial match, case-insensitive) inside a given directory. The project was created as a learning exercise to practice modern C++, filesystem APIs, and basic CLI design.

---

## Features
- Recursive file search
- Partial name matching (substring search)
- Case-insensitive comparison
- Displays full file paths
- Measures and prints search execution time
- Skips directories without permissions

---

## Example usage

```
search main -d C:\Projects
```

## Output:
```
[0] C:\Projects\App\main.cpp
[1] C:\Projects\Tests\test_main.cpp
Found: 2 files
Search time: 34 ms
```
## Requirements
- C++20 compatible compiler
- Standard Library: <filesystem>

## Tested with:
- GCC
- MSVC

## Build and Run
- Build
```
g++ -std=c++17 main.cpp -o filesearch
```
- Run
```
./filesearch search example -d C:\Path\To\Directory
```
## TODO
- Add suport to UTF-8 characters
- Add diffrent types of search
```


