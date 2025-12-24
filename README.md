# OS_Systems_Ass2_Task1-2
Task 1
// Single application that, when executed, creates a child process using fork() (Linux).
// Parent and child demonstrate an IPC mechanism.
// Example: parent sends data → child processes → child sends result back.
// Detailed console output with PIDs and progress.
// IPC mechanism is chosen and justified (pipes / shared memory / sockets / FIFO).



~/os-assignment2/
└── pa2/
    ├── ipc.c        # Task 1: Process + IPC with pipes
    └── threads.c    # Task 2: Threads, race condition + mutex

- Common terminal commands used:
     mkdir – create directories
     cd – change directory
     nano – create/edit text files
     gcc – compile C programs
     ./program – run an executable in the current directory

   
# Instructions
- Open terminal

Task 1
Steps:
    mkdir -p ~/os-assignment2
    cd ~/os-assignment2
    mkdir pa2
    cd pa2

   nano ipc.c
   Paste code from ipc.c file
   Save: Ctrl+O, Enter
   Exit: Ctrl+X
   gcc ipc.c -o ipc

   ./ipc
   Your input example: 7
   
   Task 2
   // Single application that creates multiple threads (4 or more) accessing one shared resource.
   // First clearly show a race condition (wrong result).
   // Then protect it with a synchronization mechanism (e.g. mutex), showing the correct result.
   // Display detailed information about threads (IDs, progress).
   
   Steps: 
       cd ~/os-assignment2/pa2
       nano threads.c
       Paste code from threads.c file
       Save: Ctrl+O, Enter
       Exit: Ctrl+X
       gcc threads.c -o threads
       


       
