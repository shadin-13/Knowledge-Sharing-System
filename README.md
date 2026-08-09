# Modular Knowledge Sharing & Q&A Platform in C

A command-line knowledge-sharing platform built in Pure C using dynamic memory allocation, function pointers, custom sorting (`qsort`), and file persistence.

## Features
- User Authentication & Role Management (Student, Teacher, Admin)
- Dynamic Dynamic Memory Allocation (`malloc`, `realloc`, `free`)
- Interactive Q&A System with search and upvoting
- Reputation-based dynamic Leaderboard sorted using `qsort`
- File I/O Persistence (`users.txt`, `questions.txt`, `answers.txt`)

## How to Run

### Using GCC
```bash
gcc main.c user.c qa.c -o system
./system   # Linux/Mac
system.exe # Windows
