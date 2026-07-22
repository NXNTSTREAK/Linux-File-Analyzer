## Linux File Analyzer 

A high-performance, zero-dependency command-line utility written in C++17 that extracts and formats file metadata by directly interfacing with the Linux Kernel.

This project was built to demonstrate safe systems programming concepts, including POSIX system calls, memory safety across C/C++ boundaries, and bitwise operations.

## Overview

While standard C++ provides high-level file stream abstractions (like std::ifstream), they hide the underlying operating system mechanics. This tool bypasses standard streams and talks directly to the Linux Kernel using raw POSIX APIs. It fetches file metadata (size, permissions, timestamps, and type) without ever opening the file's contents, minimizing Disk I/O and maximizing performance.

## Core Systems Concepts Demonstrated

Zero-Copy Abstractions: Uses std::string_view for read-only string passing to prevent unnecessary heap memory allocations.

Safe C-API Boundaries: Safely transitions from modern C++ bounds-checked types to raw, null-terminated C-strings required by legacy Kernel APIs.

Bitwise Operations: Decodes the packed mode_t bitmask from the kernel into human-readable read/write/execute strings using bitwise AND (&) operations.

Symlink Safety: Utilizes lstat() instead of stat() to prevent infinite loops when encountering symbolic links pointing to themselves or parent directories.

Clean Architecture: Strictly separates the OS Interface (Kernel communication) from the Formatter (CLI output) to prevent "God Object" anti-patterns.

## Project Structure

.
├── src/
│   └── main.cpp       # Core logic, OS interface, and formatting
├── README.md          # Project documentation
└── analyzer           # Compiled executable binary (ignored in git)


## Build Instructions

This project requires a Linux environment (or WSL/macOS) and a C++ compiler that supports the C++17 standard (such as GCC or Clang).

To compile the source code into an executable binary, run:

```g++ -std=c++17 -Wall -Wextra -O2 src/main.cpp -o analyzer```


(Note: -Wall -Wextra enables strict compiler warnings, and -O2 enables standard compiler optimizations).

## Usage

Provide the absolute or relative path to any file, directory, or symlink as the only argument:

```./analyzer /etc/passwd```


Example Output:

--- File Analysis ---
Target      : /etc/passwd
Size        : 2834 bytes
Permissions : rw- r-- r-- 
Modified    : 2023-10-15 14:32:01
Type        : Regular File


Error Handling Example:
If the tool lacks permissions or the file does not exist, it gracefully fails without segment faults:

./analyzer /root/secret_file
> Couldn't find or access: /root/secret_file


## Under the Hood: Memory & Performance

In user-space applications, heavy reliance on std::string can trigger thousands of slow heap allocations. This tool is optimized to use the Stack wherever possible:

The FileMetadata Struct: Uses standard POSIX integer types (off_t, mode_t, time_t) and a strongly-typed enum class to ensure the entire struct lives on the fast stack.

System Call: The struct stat buffer is allocated directly on the stack, and its physical memory address is passed to the kernel via the & pointer operator, avoiding dynamic memory allocation overhead.

## Future Improvements

Detailed errno logging: Read the global errno value to output specific kernel failures (e.g., EACCES vs ENOENT).

Dependency Injection: Abstract the OSInterface into a virtual class to allow for mocked unit testing of the formatter logic without requiring disk access.
