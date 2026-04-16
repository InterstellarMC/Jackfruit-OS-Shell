#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 1024
#define DELIMITERS " \t\r\n\a"

// MLFQ Logic for Scheduling Simulation
void simulate_mlfq(char *cmd, int total_time) {
    printf("\n[MLFQ Scheduler] Monitoring: %s\n", cmd);
    int current_time = total_time;
    int priority = 0;
    while (current_time > 0) {
        int quantum = (priority == 0) ? 4 : (priority == 1) ? 8 : 16;
        printf(" -> Q%d (Quantum %dms): ", priority, quantum);
        if (current_time <= quantum) {
            printf("Finished!\n");
            current_time = 0;
        } else {
            printf("Preempted to Q%d...\n", priority + 1);
            current_time -= quantum;
            if (priority < 2) priority++;
        }
    }
    printf("[MLFQ] Task Complete.\n\n");
}

// Pipe Execution Logic (The 10/10 feature)
void execute_pipe(char *left[], char *right[]) {
    int pipefd[2];
    if (pipe(pipefd) == -1) { perror("Pipe failed"); return; }
    
    pid_t p1 = fork();
    if (p1 == 0) {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        execvp(left[0], left);
        exit(1);
    }

    pid_t p2 = fork();
    if (p2 == 0) {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[1]);
        close(pipefd[0]);
        execvp(right[0], right);
        exit(1);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
}

void launch_command(char *line) {
    char *args[64];
    char *pipe_pos = strchr(line, '|');

    // Check for Pipes
    if (pipe_pos != NULL) {
        *pipe_pos = '\0';
        char *left_part = line;
        char *right_part = pipe_pos + 1;

        int i = 0;
        args[i] = strtok(left_part, DELIMITERS);
        while (args[i]) args[++i] = strtok(NULL, DELIMITERS);
        char *left_args[64];
        for(int k=0; k<=i; k++) left_args[k] = args[k];

        i = 0;
        char *args2[64];
        args2[i] = strtok(right_part, DELIMITERS);
        while (args2[i]) args2[++i] = strtok(NULL, DELIMITERS);
        
        execute_pipe(left_args, args2);
        return;
    }

    // Normal Command Parsing
    int i = 0;
    args[i] = strtok(line, DELIMITERS);
    while (args[i]) args[++i] = strtok(NULL, DELIMITERS);
    
    if (!args[0]) return;
    if (strcmp(args[0], "exit") == 0) exit(0);
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) fprintf(stderr, "cd: missing argument\n");
        else if (chdir(args[1]) != 0) perror("cd failed");
        return;
    }
    if (strcmp(args[0], "schedule") == 0) {
        if (args[1] && args[2]) simulate_mlfq(args[1], atoi(args[2]));
        else printf("Usage: schedule <name> <time>\n");
        return;
    }

    // Handle Redirection
    int redirect_idx = -1;
    for (int j = 0; args[j]; j++) {
        if (strcmp(args[j], ">") == 0) { redirect_idx = j; break; }
    }

    if (fork() == 0) {
        if (redirect_idx != -1) {
            int fd = open(args[redirect_idx+1], O_WRONLY|O_CREAT|O_TRUNC, 0644);
            if (fd < 0) { perror("File error"); exit(1); }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[redirect_idx] = NULL;
        }
        execvp(args[0], args);
        perror("Execution failed");
        exit(1);
    } else wait(NULL);
}

int main() {
    char line[MAX_LINE];
    printf("🍈 Jackfruit OS v4.0 (Pipes & MLFQ) 🍈\n");
    while (1) {
        printf("🍈 jackfruit_os> ");
        if (!fgets(line, MAX_LINE, stdin)) break;
        line[strcspn(line, "\n")] = 0;
        launch_command(line);
    }
    return 0;
}
