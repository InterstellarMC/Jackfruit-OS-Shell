Jackfruit OS Shell
This is a custom Linux shell developed for the 4th-semester Operating Systems course. It demonstrates core kernel-level concepts including process management, inter-process communication, and CPU scheduling.

1. Project Overview
The Jackfruit Shell is a command-line interface that interacts directly with the Linux kernel using system calls. Beyond running standard binaries, it includes custom logic for I/O redirection, piping, and a scheduling simulator.

2. Implemented Features
Shell & Process Management
Command Parsing: Robust tokenization of user input using strtok.

Execution: Utilizes the fork(), execvp(), and waitpid() lifecycle to manage child processes.

Built-ins: Native implementation of cd (change directory) and exit to maintain shell state.

Advanced Features
I/O Redirection: Supports the > operator to redirect standard output to a file using dup2().

Piping: Implements IPC (Inter-Process Communication) via anonymous pipes, allowing the output of one command to be fed into another (e.g., ls | grep).

Background Execution: Basic handling for background tasks.

MLFQ Scheduling Simulator
The shell includes a built-in simulator for a Multilevel Feedback Queue (MLFQ). This shows how an OS handles process priorities:

Q0 (High Priority): 4ms time quantum.

Q1 (Medium Priority): 8ms time quantum.

Q2 (Low Priority): 16ms time quantum.

Logic: Tasks that exceed their time slice are preempted and demoted to a lower priority queue to prevent CPU hogging.

3. How to Compile and Use
Prerequisites
Ubuntu/Linux environment

GCC compiler

Installation
Clone the repository:

Bash
git clone https://github.com/shivangjhalani/OS-Jackfruit.git
Navigate to the directory:

Bash
cd OS-Jackfruit
Execution
Compile the source code:

Bash
gcc main.c -o jackfruit
Run the shell:

Bash
./jackfruit
4. Testing Commands
Once inside the shell, you can test the following:

Redirection: ls > output.txt

Pipes: ls | grep main

Scheduler: schedule MyTask 30

Navigation: cd ..

5. Challenges Faced
The primary challenge was managing file descriptors during piping. Ensuring that both the read and write ends of the pipe were properly closed in the parent process was crucial to avoid hanging processes and deadlocks.
