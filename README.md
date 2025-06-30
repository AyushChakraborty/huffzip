# huffzip

**huffzip** is a lightweight, command line file compression utility written in C, using the [Huffman coding algorithm](https://en.wikipedia.org/wiki/Huffman_coding). It provides an efficient way to compress and decompress plain text files by using character frequency distributions.

This project is educational, minimal : perfect for understanding how entropy based compression works under the hood.

---

## Features

- **Compress & decompress** files using Huffman coding
- Handles **ASCII** files with clear structure
- Modular codebase for future extension (like unicode support)
- Simple and portable **CLI interface**
- **Homebrew tap support** for easy installation on macOS

---

## Installation

### Homebrew (Recommended – macOS)

```bash
brew tap ayushchakraborty/huff
brew install huff
```

---

## Usage

```bash
#compress a file
huff compress input.txt output.txt

#decompress the file
huff decompress output.txt decoded.txt
```

---

### Build from Source

```bash
git clone https://github.com/AyushChakraborty/huffzip
cd huffzip
make
./huff compress input.txt output.txt
```

---
