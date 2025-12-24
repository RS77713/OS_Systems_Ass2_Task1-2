#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

// Simple error helper
static void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(void) {
    int parent_to_child[2];
    int child_to_parent[2];

    if (pipe(parent_to_child) == -1) {
        die("pipe parent_to_child");
    }
    if (pipe(child_to_parent) == -1) {
        die("pipe child_to_parent");
    }

    pid_t pid = fork();
    if (pid < 0) {
        die("fork");
    }

    if (pid == 0) {
        // ----------------- CHILD PROCESS -----------------
        close(parent_to_child[1]); // child doesn't write to this pipe
        close(child_to_parent[0]); // child doesn't read from this pipe

        pid_t child_pid = getpid();
        pid_t parent_pid = getppid();
        printf("[Child  %d] Started. Parent PID: %d\n",
               (int)child_pid, (int)parent_pid);
        fflush(stdout);

        int value;
        ssize_t n = read(parent_to_child[0], &value, sizeof(value));
        if (n < 0) {
            die("[Child] read");
        } else if (n == 0) {
            fprintf(stderr,
                    "[Child  %d] Parent closed pipe unexpectedly\n",
                    (int)child_pid);
            exit(EXIT_FAILURE);
        }

        printf("[Child  %d] Received value %d from parent. Processing...\n",
               (int)child_pid, value);
        fflush(stdout);

        int result = value * value; // example processing
        printf("[Child  %d] Computed square: %d. Sending back to parent...\n",
               (int)child_pid, result);
        fflush(stdout);

        if (write(child_to_parent[1], &result, sizeof(result)) != sizeof(result)) {
            die("[Child] write");
        }

        printf("[Child  %d] Done. Exiting.\n", (int)child_pid);
        fflush(stdout);

        close(parent_to_child[0]);
        close(child_to_parent[1]);

        _exit(EXIT_SUCCESS); // use _exit in child after fork
    } else {
        // ----------------- PARENT PROCESS -----------------
        close(parent_to_child[0]); // parent doesn't read from this pipe
        close(child_to_parent[1]); // parent doesn't write to this pipe

        pid_t parent_pid = getpid();
        printf("[Parent %d] Started. Created child with PID %d\n",
               (int)parent_pid, (int)pid);

        int value;
        printf("[Parent %d] Enter an integer to send to the child: ",
               (int)parent_pid);
        fflush(stdout);

        if (scanf("%d", &value) != 1) {
            fprintf(stderr, "[Parent %d] Invalid input.\n", (int)parent_pid);
            exit(EXIT_FAILURE);
        }

        printf("[Parent %d] Sending value %d to child...\n",
               (int)parent_pid, value);
        fflush(stdout);

        if (write(parent_to_child[1], &value, sizeof(value)) != sizeof(value)) {
            die("[Parent] write");
        }
        close(parent_to_child[1]); // finished sending

        int result;
        ssize_t n = read(child_to_parent[0], &result, sizeof(result));
        if (n < 0) {
            die("[Parent] read");
        } else if (n == 0) {
            fprintf(stderr,
                    "[Parent %d] Child closed pipe unexpectedly\n",
                    (int)parent_pid);
            exit(EXIT_FAILURE);
        }

        printf("[Parent %d] Received result %d from child.\n",
               (int)parent_pid, result);
        printf("[Parent %d] Expected: %d^2 = %d\n",
               (int)parent_pid, value, value * value);
        fflush(stdout);

        close(child_to_parent[0]);

        int status;
        if (waitpid(pid, &status, 0) == -1) {
            die("waitpid");
        }
        if (WIFEXITED(status)) {
            printf("[Parent %d] Child exited with status %d\n",
                   (int)parent_pid, WEXITSTATUS(status));
        } else {
            printf("[Parent %d] Child terminated abnormally\n",
                   (int)parent_pid);
        }

        printf("[Parent %d] Done.\n", (int)parent_pid);
    }

    return 0;
}
