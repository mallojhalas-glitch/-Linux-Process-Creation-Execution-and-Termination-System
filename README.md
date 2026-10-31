# Operating System Project
## Integrated OS Concepts and Modern OS Observability

**Course:** Operating Systems and Systems Programming (OSSP)

**Project Team:** 
Srivalli
Shanmukhi
Rohita

**Platform:** Ubuntu 24.04.4 LTS (WSL2)

**Language:** C

**Compiler:** GCC

**Observability:** bpftrace v0.20.2

## Project Overview
A menu-driven C application demonstrating six Operating System
concepts through practical implementation.

## Course Outcomes

| CO | Module | Implementation |
|---|---|---|
| CO1 | Process Management | fork(), exec(), wait() |
| CO2 | CPU Scheduling | FCFS, SJF, Round Robin |
| CO3 | Memory Management | First Fit, Best Fit, Worst Fit |
| CO4 | File System Management | Directory reading, stat() |
| CO5 | Concurrency and Synchronization | Thread-safe bounded queue |
| CO6 | Modern OS Observability | System calls, Block I/O, Page Faults |

## Installation

Install GCC and bpftrace:

    sudo apt update
    sudo apt install gcc bpftrace

## Compilation

    gcc main.c -o os_project -pthread

## Execution

    ./os_project

## Testing

The project has recorded execution runs for CO1, CO2, CO3,
CO5 and CO6.

CO4 directory reading requires a successful additional test.
Block I/O tracing attached but did not display event counts
in the recorded run.

## Technologies Used
- C Programming
- Linux / Ubuntu
- GCC
- POSIX Threads
- bpftrace
- WSL2
- Git and GitHub

## Future Enhancements
- Additional test cases
- Improved input validation
- Extended kernel event monitoring
- Detailed execution documentation

## Author
Buddarthi Srivalli
K L University
