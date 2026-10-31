#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>

/*
===========================================================
OPERATING SYSTEM PROJECT
LINUX PROCESS CREATION, EXECUTION AND TERMINATION SYSTEM

CO1 - Process Management
CO2 - CPU Scheduling
CO3 - Memory Management
CO4 - File System Management
CO5 - Concurrency and Synchronization
CO6 - Modern OS Observability / eBPF
===========================================================
*/


/* =========================================================
   CO1 - PROCESS MANAGEMENT
   Topics:
   fork(), exec(), wait(), process creation and termination
   ========================================================= */

void process_management()
{
    pid_t pid;
    int status;

    printf("\n========================================\n");
    printf("          CO1 - PROCESS MANAGEMENT\n");
    printf("========================================\n");

    pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        return;
    }

    if (pid == 0)
    {
        printf("\n--- CHILD PROCESS ---\n");
        printf("Child PID  : %d\n", getpid());
        printf("Parent PID : %d\n", getppid());

        printf("\nChild is executing 'ls -l'...\n\n");

        execlp("ls", "ls", "-l", NULL);

        perror("exec failed");
        exit(1);
    }
    else
    {
        printf("\n--- PARENT PROCESS ---\n");
        printf("Parent PID : %d\n", getpid());
        printf("Child PID  : %d\n", pid);

        waitpid(pid, &status, 0);

        if (WIFEXITED(status))
        {
            printf("\nChild terminated normally.\n");
            printf("Exit status: %d\n",
                   WEXITSTATUS(status));
        }
    }
}


/* =========================================================
   CO2 - CPU PROCESS SCHEDULING
   Topics:
   FCFS, SJF, Round Robin
   ========================================================= */

typedef struct
{
    int pid;
    int burst;
    int waiting;
    int turnaround;
} Process;


/* ---------- FCFS ---------- */

void fcfs()
{
    int n;
    Process p[20];

    printf("\n========== CO2 - FCFS ==========\n");

    printf("Enter number of processes: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++)
    {
        p[i].pid = i + 1;

        printf("Enter burst time for P%d: ",
               p[i].pid);
        scanf("%d", &p[i].burst);
    }

    p[0].waiting = 0;

    for (int i = 1; i < n; i++)
    {
        p[i].waiting =
            p[i - 1].waiting +
            p[i - 1].burst;
    }

    float total_wait = 0;
    float total_turnaround = 0;

    printf("\nProcess\tBurst\tWaiting\tTurnaround\n");

    for (int i = 0; i < n; i++)
    {
        p[i].turnaround =
            p[i].waiting + p[i].burst;

        printf("P%d\t%d\t%d\t%d\n",
               p[i].pid,
               p[i].burst,
               p[i].waiting,
               p[i].turnaround);

        total_wait += p[i].waiting;
        total_turnaround += p[i].turnaround;
    }

    printf("\nAverage Waiting Time = %.2f\n",
           total_wait / n);

    printf("Average Turnaround Time = %.2f\n",
           total_turnaround / n);
}


/* ---------- SJF ---------- */

void sjf()
{
    int n;
    Process p[20];

    printf("\n========== CO2 - SJF ==========\n");

    printf("Enter number of processes: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++)
    {
        p[i].pid = i + 1;

        printf("Enter burst time for P%d: ",
               p[i].pid);
        scanf("%d", &p[i].burst);
    }

    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (p[j].burst < p[i].burst)
            {
                Process temp = p[i];
                p[i] = p[j];
                p[j] = temp;
            }
        }
    }

    p[0].waiting = 0;

    for (int i = 1; i < n; i++)
    {
        p[i].waiting =
            p[i - 1].waiting +
            p[i - 1].burst;
    }

    float total_wait = 0;
    float total_turnaround = 0;

    printf("\nProcess\tBurst\tWaiting\tTurnaround\n");

    for (int i = 0; i < n; i++)
    {
        p[i].turnaround =
            p[i].waiting + p[i].burst;

        printf("P%d\t%d\t%d\t%d\n",
               p[i].pid,
               p[i].burst,
               p[i].waiting,
               p[i].turnaround);

        total_wait += p[i].waiting;
        total_turnaround += p[i].turnaround;
    }

    printf("\nAverage Waiting Time = %.2f\n",
           total_wait / n);

    printf("Average Turnaround Time = %.2f\n",
           total_turnaround / n);
}


/* ---------- ROUND ROBIN ---------- */

void round_robin()
{
    int n, quantum;
    Process p[20];
    int remaining[20];

    printf("\n========== CO2 - ROUND ROBIN ==========\n");

    printf("Enter number of processes: ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++)
    {
        p[i].pid = i + 1;

        printf("Enter burst time for P%d: ",
               p[i].pid);
        scanf("%d", &p[i].burst);

        remaining[i] = p[i].burst;
        p[i].waiting = 0;
        p[i].turnaround = 0;
    }

    printf("Enter time quantum: ");
    scanf("%d", &quantum);

    int time = 0;
    int completed = 0;

    printf("\nExecution Order:\n");

    while (completed < n)
    {
        for (int i = 0; i < n; i++)
        {
            if (remaining[i] > 0)
            {
                printf("P%d ", p[i].pid);

                if (remaining[i] > quantum)
                {
                    time += quantum;
                    remaining[i] -= quantum;
                }
                else
                {
                    time += remaining[i];
                    remaining[i] = 0;

                    p[i].turnaround = time;

                    p[i].waiting =
                        p[i].turnaround -
                        p[i].burst;

                    completed++;
                }
            }
        }
    }

    float total_wait = 0;
    float total_turnaround = 0;

    printf("\n\nProcess\tBurst\tWaiting\tTurnaround\n");

    for (int i = 0; i < n; i++)
    {
        printf("P%d\t%d\t%d\t%d\n",
               p[i].pid,
               p[i].burst,
               p[i].waiting,
               p[i].turnaround);

        total_wait += p[i].waiting;
        total_turnaround += p[i].turnaround;
    }

    printf("\nAverage Waiting Time = %.2f\n",
           total_wait / n);

    printf("Average Turnaround Time = %.2f\n",
           total_turnaround / n);
}


void scheduling_management()
{
    int choice;

    while (1)
    {
        printf("\n========================================\n");
        printf("          CO2 - CPU SCHEDULING\n");
        printf("========================================\n");

        printf("1. FCFS\n");
        printf("2. SJF\n");
        printf("3. Round Robin\n");
        printf("4. Back\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                fcfs();
                break;

            case 2:
                sjf();
                break;

            case 3:
                round_robin();
                break;

            case 4:
                return;

            default:
                printf("Invalid choice.\n");
        }
    }
}


/* =========================================================
   CO3 - MEMORY MANAGEMENT
   Topics:
   First Fit, Best Fit, Worst Fit
   ========================================================= */

#define MAX 20


void first_fit()
{
    int nb, np;
    int blocks[MAX];
    int process[MAX];
    int allocation[MAX];

    printf("\n========== CO3 - FIRST FIT ==========\n");

    printf("Enter number of memory blocks: ");
    scanf("%d", &nb);

    for (int i = 0; i < nb; i++)
    {
        printf("Block B%d size: ", i + 1);
        scanf("%d", &blocks[i]);
    }

    printf("Enter number of processes: ");
    scanf("%d", &np);

    for (int i = 0; i < np; i++)
    {
        printf("Process P%d size: ", i + 1);
        scanf("%d", &process[i]);

        allocation[i] = -1;
    }

    for (int i = 0; i < np; i++)
    {
        for (int j = 0; j < nb; j++)
        {
            if (blocks[j] >= process[i])
            {
                allocation[i] = j;
                blocks[j] -= process[i];
                break;
            }
        }
    }

    printf("\nProcess\tSize\tAllocation\n");

    for (int i = 0; i < np; i++)
    {
        if (allocation[i] != -1)
        {
            printf("P%d\t%d\tB%d\n",
                   i + 1,
                   process[i],
                   allocation[i] + 1);
        }
        else
        {
            printf("P%d\t%d\tNot Allocated\n",
                   i + 1,
                   process[i]);
        }
    }
}


void best_fit()
{
    int nb, np;
    int blocks[MAX];
    int process[MAX];
    int allocation[MAX];

    printf("\n========== CO3 - BEST FIT ==========\n");

    printf("Enter number of memory blocks: ");
    scanf("%d", &nb);

    for (int i = 0; i < nb; i++)
    {
        printf("Block B%d size: ", i + 1);
        scanf("%d", &blocks[i]);
    }

    printf("Enter number of processes: ");
    scanf("%d", &np);

    for (int i = 0; i < np; i++)
    {
        printf("Process P%d size: ", i + 1);
        scanf("%d", &process[i]);

        allocation[i] = -1;
    }

    for (int i = 0; i < np; i++)
    {
        int best = -1;

        for (int j = 0; j < nb; j++)
        {
            if (blocks[j] >= process[i])
            {
                if (best == -1 ||
                    blocks[j] < blocks[best])
                {
                    best = j;
                }
            }
        }

        if (best != -1)
        {
            allocation[i] = best;
            blocks[best] -= process[i];
        }
    }

    printf("\nProcess\tSize\tAllocation\n");

    for (int i = 0; i < np; i++)
    {
        if (allocation[i] != -1)
        {
            printf("P%d\t%d\tB%d\n",
                   i + 1,
                   process[i],
                   allocation[i] + 1);
        }
        else
        {
            printf("P%d\t%d\tNot Allocated\n",
                   i + 1,
                   process[i]);
        }
    }
}


void worst_fit()
{
    int nb, np;
    int blocks[MAX];
    int process[MAX];
    int allocation[MAX];

    printf("\n========== CO3 - WORST FIT ==========\n");

    printf("Enter number of memory blocks: ");
    scanf("%d", &nb);

    for (int i = 0; i < nb; i++)
    {
        printf("Block B%d size: ", i + 1);
        scanf("%d", &blocks[i]);
    }

    printf("Enter number of processes: ");
    scanf("%d", &np);

    for (int i = 0; i < np; i++)
    {
        printf("Process P%d size: ", i + 1);
        scanf("%d", &process[i]);

        allocation[i] = -1;
    }

    for (int i = 0; i < np; i++)
    {
        int worst = -1;

        for (int j = 0; j < nb; j++)
        {
            if (blocks[j] >= process[i])
            {
                if (worst == -1 ||
                    blocks[j] > blocks[worst])
                {
                    worst = j;
                }
            }
        }

        if (worst != -1)
        {
            allocation[i] = worst;
            blocks[worst] -= process[i];
        }
    }

    printf("\nProcess\tSize\tAllocation\n");

    for (int i = 0; i < np; i++)
    {
        if (allocation[i] != -1)
        {
            printf("P%d\t%d\tB%d\n",
                   i + 1,
                   process[i],
                   allocation[i] + 1);
        }
        else
        {
            printf("P%d\t%d\tNot Allocated\n",
                   i + 1,
                   process[i]);
        }
    }
}


void memory_management()
{
    int choice;

    while (1)
    {
        printf("\n========================================\n");
        printf("          CO3 - MEMORY MANAGEMENT\n");
        printf("========================================\n");

        printf("1. First Fit\n");
        printf("2. Best Fit\n");
        printf("3. Worst Fit\n");
        printf("4. Back\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                first_fit();
                break;

            case 2:
                best_fit();
                break;

            case 3:
                worst_fit();
                break;

            case 4:
                return;

            default:
                printf("Invalid choice.\n");
        }
    }
}


/* =========================================================
   CO4 - FILE SYSTEM MANAGEMENT
   Topics:
   Directory handling, opendir(), readdir(), stat()
   ========================================================= */

void read_directory()
{
    char path[256];

    DIR *dir;
    struct dirent *entry;

    printf("\n========================================\n");
    printf("       CO4 - READ DIRECTORY\n");
    printf("========================================\n");

    printf("Enter directory path (. for current): ");
    scanf("%255s", path);

    dir = opendir(path);

    if (dir == NULL)
    {
        perror("opendir");
        return;
    }

    printf("\nDirectory contents:\n");

    while ((entry = readdir(dir)) != NULL)
    {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}


void file_information()
{
    char filename[256];
    struct stat st;

    printf("\n========================================\n");
    printf("       CO4 - FILE INFORMATION\n");
    printf("========================================\n");

    printf("Enter file name: ");
    scanf("%255s", filename);

    if (stat(filename, &st) == -1)
    {
        perror("stat");
        return;
    }

    printf("\nFile Name  : %s\n", filename);
    printf("File Size  : %ld bytes\n",
           (long)st.st_size);
    printf("Inode      : %ld\n",
           (long)st.st_ino);
    printf("Links      : %ld\n",
           (long)st.st_nlink);
    printf("Permissions: %o\n",
           st.st_mode & 0777);

    if (S_ISREG(st.st_mode))
        printf("Type       : Regular File\n");
    else if (S_ISDIR(st.st_mode))
        printf("Type       : Directory\n");
    else if (S_ISLNK(st.st_mode))
        printf("Type       : Symbolic Link\n");
    else
        printf("Type       : Other\n");
}


void file_system_management()
{
    int choice;

    while (1)
    {
        printf("\n========================================\n");
        printf("       CO4 - FILE SYSTEM MANAGEMENT\n");
        printf("========================================\n");

        printf("1. Read Directory\n");
        printf("2. File Information using stat()\n");
        printf("3. Back\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                read_directory();
                break;

            case 2:
                file_information();
                break;

            case 3:
                return;

            default:
                printf("Invalid choice.\n");
        }
    }
}


/* =========================================================
   CO5 - CONCURRENCY AND SYNCHRONIZATION
   Topics:
   Threads, mutex, condition variables,
   producer-consumer, bounded queue
   ========================================================= */

#define QUEUE_SIZE 5
#define ITEMS 10

int queue[QUEUE_SIZE];

int front = 0;
int rear = 0;
int count = 0;

pthread_mutex_t mutex =
    PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t not_empty =
    PTHREAD_COND_INITIALIZER;

pthread_cond_t not_full =
    PTHREAD_COND_INITIALIZER;


/* ---------- PRODUCER ---------- */

void *producer(void *arg)
{
    (void)arg;

    for (int i = 1; i <= ITEMS; i++)
    {
        pthread_mutex_lock(&mutex);

        while (count == QUEUE_SIZE)
        {
            pthread_cond_wait(
                &not_full,
                &mutex);
        }

        queue[rear] = i;

        rear = (rear + 1) % QUEUE_SIZE;

        count++;

        printf("Producer produced: %d\n", i);

        pthread_cond_signal(&not_empty);

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}


/* ---------- CONSUMER ---------- */

void *consumer(void *arg)
{
    (void)arg;

    for (int i = 1; i <= ITEMS; i++)
    {
        pthread_mutex_lock(&mutex);

        while (count == 0)
        {
            pthread_cond_wait(
                &not_empty,
                &mutex);
        }

        int item = queue[front];

        front =
            (front + 1) % QUEUE_SIZE;

        count--;

        printf("Consumer consumed: %d\n",
               item);

        pthread_cond_signal(&not_full);

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}


void thread_safe_queue()
{
    pthread_t producer_thread;
    pthread_t consumer_thread;

    printf("\n========================================\n");
    printf("     CO5 - THREAD SAFE BOUNDED QUEUE\n");
    printf("========================================\n");

    pthread_create(
        &producer_thread,
        NULL,
        producer,
        NULL);

    pthread_create(
        &consumer_thread,
        NULL,
        consumer,
        NULL);

    pthread_join(
        producer_thread,
        NULL);

    pthread_join(
        consumer_thread,
        NULL);

    printf("\nSynchronization completed.\n");
}


void synchronization_management()
{
    int choice;

    while (1)
    {
        printf("\n========================================\n");
        printf("    CO5 - CONCURRENCY & SYNCHRONIZATION\n");
        printf("========================================\n");

        printf("1. Thread-Safe Bounded Queue\n");
        printf("2. Back\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                thread_safe_queue();
                break;

            case 2:
                return;

            default:
                printf("Invalid choice.\n");
        }
    }
}


/* =========================================================
   CO6 - MODERN OS OBSERVABILITY
   Topics:
   bpftrace, system-call tracing,
   block I/O and page-fault tracing
   ========================================================= */

void trace_syscalls()
{
    printf("\n========================================\n");
    printf("       CO6 - SYSTEM CALL TRACING\n");
    printf("========================================\n");

    printf("Running bpftrace...\n");

    system(
        "sudo timeout 5 bpftrace "
        "-e 'tracepoint:raw_syscalls:sys_enter "
        "{ @[comm] = count(); }'"
    );
}


void trace_block_io()
{
    printf("\n========================================\n");
    printf("       CO6 - BLOCK I/O TRACING\n");
    printf("========================================\n");

    printf("Running bpftrace...\n");

    system(
        "sudo timeout 5 bpftrace "
        "-e 'tracepoint:block:block_rq_complete "
        "{ @[comm] = count(); }'"
    );
}


void trace_page_faults()
{
    printf("\n========================================\n");
    printf("       CO6 - PAGE FAULT TRACING\n");
    printf("========================================\n");

    printf("Running bpftrace...\n");

    system(
        "sudo timeout 5 bpftrace "
        "-e 'tracepoint:exceptions:page_fault_user "
        "{ @[comm] = count(); }'"
    );
}


void modern_os_management()
{
    int choice;

    while (1)
    {
        printf("\n========================================\n");
        printf("       CO6 - MODERN OS OBSERVABILITY\n");
        printf("========================================\n");

        printf("1. Count System Calls\n");
        printf("2. Trace Block I/O\n");
        printf("3. Trace Page Faults\n");
        printf("4. Back\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                trace_syscalls();
                break;

            case 2:
                trace_block_io();
                break;

            case 3:
                trace_page_faults();
                break;

            case 4:
                return;

            default:
                printf("Invalid choice.\n");
        }
    }
}


/* =========================================================
   MAIN MENU - CO1 TO CO6
   ========================================================= */

int main()
{
    int choice;

    while (1)
    {
        printf("\n\n");
        printf("============================================\n");
        printf("       OPERATING SYSTEM PROJECT\n");
        printf("============================================\n");

        printf("1. Process Management              (CO1)\n");
        printf("2. CPU Scheduling                  (CO2)\n");
        printf("3. Memory Management               (CO3)\n");
        printf("4. File System Management          (CO4)\n");
        printf("5. Concurrency & Synchronization   (CO5)\n");
        printf("6. Modern OS Observability         (CO6)\n");
        printf("7. Exit\n");

        printf("============================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                process_management();
                break;

            case 2:
                scheduling_management();
                break;

            case 3:
                memory_management();
                break;

            case 4:
                file_system_management();
                break;

            case 5:
                synchronization_management();
                break;

            case 6:
                modern_os_management();
                break;

            case 7:
                printf("\nExiting project...\n");
                return 0;

            default:
                printf("\nInvalid choice. Try again.\n");
        }
    }

    return 0;
}
