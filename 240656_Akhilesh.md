# Word Frequency Analyzer — C++ Assignment

A command-line C++ tool that reads large text files in buffered chunks, builds per-version word-frequency maps, and answers three types of queries: single-word count, top-K words, and cross-version diff.

---

## Features

- Buffered file I/O (256–1024 KB chunks) for memory-efficient processing of large files
- Multiple named "versions" of frequency maps stored simultaneously
- Template utility `printContainer<T>` for generic pair-container printing
- Two overloads of `buildVersion` — standard and verbose
- Three query modes: `word`, `top`, `diff`
- Case-insensitive tokenization (alphanumeric tokens only)
- Execution time reporting

---

## Build

```bash
g++ -std=c++17 -O2 -o word_freq 240656_Akhilesh.cpp
```

---

## Usage

### Word Count Query
```bash
./word_freq --file text.txt --version v1 --buffer 512 --query word --word hello
```

### Top-K Query
```bash
./word_freq --file text.txt --version v1 --buffer 512 --query top --top 10
```

### Diff Query (compare two files/versions)
```bash
./word_freq \
  --file1 old.txt --version1 v1 \
  --file2 new.txt --version2 v2 \
  --buffer 512 --query diff --word hello
```

---

## Command-Line Arguments

| Argument | Description |
|----------|-------------|
| `--file <path>` | Path to the input text file |
| `--version <n>` | Name to assign to this version |
| `--buffer <KB>` | Buffer size in KB (must be 256–1024) |
| `--query <type>` | Query type: `word`, `top`, or `diff` |
| `--word <word>` | Target word for `word` or `diff` queries |
| `--top <k>` | Number of top words for `top` query |
| `--file1 / --file2` | Input files for diff query |
| `--version1 / --version2` | Version names for diff query |

---

## Output Format

**Word query:**
```
Version : v1
Count : 42
```

**Top-K query:**
```
Top-5 words in version: v1
the 1023
and 876
...
```

**Diff query:**
```
Difference (v2-v1): 15
```

---

## Constraints and Assumptions

- Buffer size must be between 256 KB and 1024 KB (enforced at runtime)
- Tokens are defined as contiguous alphanumeric sequences; punctuation acts as a delimiter
- All tokens are lowercased before counting
- A word split across a chunk boundary is handled via a `leftover` string carried between reads
- The `versions` map is a global — all loaded versions persist for the lifetime of the process

---

## Dependencies

- C++17 standard library only (no external dependencies)
