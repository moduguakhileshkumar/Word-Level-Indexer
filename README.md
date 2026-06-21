# Word-Level Indexer

A command-line C++ application for indexing and analyzing large text files using configurable buffered file I/O. The system supports word-frequency queries, top-K frequent word retrieval, and cross-version word-frequency comparison.

## Features

* Buffered file processing (256 KB – 1024 KB)
* Case-insensitive word indexing
* Word frequency queries
* Top-K frequent word retrieval
* Cross-version frequency comparison
* Efficient hash-map based indexing

## Technologies Used

* C++17
* STL (`unordered_map`, `vector`, `algorithm`)
* File I/O
* Object-Oriented Programming

## OOP Concepts Implemented

* Templates
* Abstract Base Classes
* Runtime Polymorphism
* Function Overloading
* Exception Handling

## Build

```bash
g++ -std=c++17 -O2 -o indexer main.cpp
```

## Example Usage

```bash
./indexer --file data.txt --version v1 --buffer 512 --query word --word error
```

## Sample Output

```text
Version : v1
Count : 605079
```
