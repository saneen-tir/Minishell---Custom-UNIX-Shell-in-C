# 🐚 MiniShell (msh) — Custom Unix Shell in C

> A fully functional Unix shell built from scratch in C, implementing core OS concepts including process management, signal handling, job control, and IPC via pipes.

---

## 📌 Overview

MiniShell (`msh`) is a lightweight Unix shell developed as part of a **Linux Internals** module. The goal was to deeply understand and apply POSIX system calls related to process creation, signal handling, process synchronization, exit status tracking, and text parsing — by building a real, working shell from the ground up.

---

## ✨ Features

### 🖥️ Custom Prompt
- Displays a default prompt: `Minishell$: <current_directory>`
- Fully customizable via `PS1=NEW_PROMPT`
- Strict syntax enforcement — whitespace around `=` is rejected and treated as a regular command

### ⚙️ Command Execution
- Classifies commands as **built-in**, **external**, or **unknown**
- External commands are executed by spawning a child process via `fork()` + `execvp()`
- Parent waits for the child to complete before re-displaying the prompt
- Empty input gracefully re-displays the prompt

### 📦 Built-in Commands

| Command | Description |
|--------|-------------|
| `cd [path]` | Change directory (`/` if no path given) |
| `pwd` | Print current working directory |
| `echo [arg]` | Print text or special variables |
| `exit` | Terminate the shell |

### 🔣 Special Variables

| Variable | Description |
|----------|-------------|
| `echo $$` | PID of the `msh` process |
| `echo $?` | Exit status of the last executed command |
| `echo $SHELL` | Path of the current shell executable |

### 🔔 Signal Handling

| Signal | Shortcut | Behavior |
|--------|----------|----------|
| `SIGINT` | `Ctrl+C` | Kills foreground process; re-displays prompt if idle |
| `SIGTSTP` | `Ctrl+Z` | Stops foreground process and adds it to the jobs list |
| `SIGCHLD` | _(auto)_ | Cleans up terminated background processes via `waitpid()` |

### 🔄 Job Control

- **`jobs`** — Lists all stopped/background processes with their PID and command
- **`fg`** — Brings the most recently stopped process to the foreground
- **`bg`** — Resumes the most recently stopped process in the background
- Stopped jobs are tracked using a **singly linked list** (`struct list`)

### 🔗 Pipe Support

Supports dynamic multi-stage pipelines:

```bash
ls | wc
ls -l /dev | grep tty | wc -l
```

Pipes are parsed at runtime, with child processes and `pipe()`/`dup2()` file descriptor redirection created dynamically based on the number of `|` operators in the command.

---

## 🛠️ Tech Stack

| Layer | Tools / APIs |
|-------|-------------|
| Language | C (C99) |
| Process Management | `fork()`, `execvp()`, `waitpid()`, `exit()` |
| Signal Handling | `signal()`, `kill()`, `SIGINT`, `SIGTSTP`, `SIGCHLD` |
| IPC | `pipe()`, `dup2()`, `dup()` |
| File I/O | `open()`, `read()`, `close()` |
| Memory | `malloc()`, `strdup()`, `free()` |
| Directory | `getcwd()`, `chdir()` |

---

## 📁 Project Structure

```
minishell/
├── main.c          # Shell driver: prompt loop, command dispatch, signal setup
├── utils.c         # Linked list (job control) + external command loader
├── minishell.h     # Macros, structs, ANSI colors, function prototypes
└── external.txt    # Registry of supported external commands
```

---

## 🚀 Build & Run

```bash
gcc main.c utils.c -o msh
./msh
```

> Make sure `external.txt` is in the same directory as the binary.

---

## 📚 Key Concepts Demonstrated

- **Process lifecycle** — Creating, synchronizing, and reaping child processes
- **Signal-driven control flow** — Custom handlers for interactive signal management
- **Job control** — Stopping, resuming, and tracking processes using linked lists
- **Inter-process communication** — Dynamic pipe chaining across multiple commands
- **Text parsing** — Tokenizing command strings and dispatching to the right executor
- **File descriptor manipulation** — Redirecting stdin/stdout using `dup2()`

---

## 👤 Author

**Muhammed Saneen P**  
📅 January 2026

---

> *Built as part of the Linux Internals module to gain hands-on experience with OS-level programming — foundational for systems, embedded, and low-level software engineering.*
