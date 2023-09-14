#include "ring_process.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <dirent.h>

/*
 * Ensures the arguments are valid for this program.
 *
 * argc: The number of arguments passed to the program.
 * argv: The array of arguments passed to the program.
 * np: Pointer to the number of processes to be used by the program.
 *
 * Returns an integer with a status code.
 */
int parse_args(int argc, char *argv[], int *np)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s nprocs directory\n", argv[0]);
        return (-1);
    }

    *np = atoi(argv[1]);

    if (*np <= 0)
    {
        fprintf(stderr, "Invalid number of processes. Must be a positive integer.\n");
        return (-1);
    }

    return 0;
}

/*
 * Constructs a ring of processes.
 *
 * Returns an integer with a status code.
 */
int make_trivial_ring()
{
    int fd[2];

    if (pipe(fd) == -1)
        return (-1);

    if ((dup2(fd[0], STDIN_FILENO) == -1) || (dup2(fd[1], STDOUT_FILENO) == -1))
        return (-2);

    if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
        return (-3);

    return 0;
}

/*
 * Creates a new child process.
 *
 * pid: Pointer to store the process ID.
 *
 * Returns an integer with a status code.
*/
int add_new_node(int *pid)
{
    int fd[2];

    if (pipe(fd) == -1)
        return (-1);

    if ((*pid = fork()) == -1)
        return (-2);

    if (*pid > 0 && dup2(fd[1], STDOUT_FILENO) < 0)
        return (-3);

    if (*pid == 0 && dup2(fd[0], STDIN_FILENO) < 0)
        return (-4);

    if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
        return (-5);

    return 0;
}

/*
 * Processes the individual letter counts in a file.
 *
 * inFile: The path to the file.
 * char_stats: The array containing the individual letter counts.
 *
 * Returns a long with the total character count of the file.
 */
long procFileCounts(char inFile[], long char_stats[])
{
    char cur_fname[256] = "./text_files/";
    strcat(cur_fname, inFile);
    FILE *file = fopen(cur_fname, "r");

    if (file == NULL)
    {
        perror("Failed to open file");
        exit(1);
    }

    char c;
    long char_count = 0;

    while ((c = fgetc(file)) != EOF)
    {
        if ((tolower(c) - 'a') >= 0 && (tolower(c) - 'a') < 26)
            char_stats[tolower(c) - 'a'] += 1;

        char_count += 1;
    }

    fclose(file);
    return char_count;
}

/*
 * Prints the histogram of the character frequencies.
 *
 * char_stats: The array containing the individual letter counts.
 * nprocs: The number of processes in the ring.
 */
void print_histogram(const long char_stats[], int nprocs)
{
    fprintf(stderr, "\nProcessing complete on the ring with %d processes\n\n", nprocs);

    long max_count = 0;

    for (int i = 0; i < 26; i++)
    {
        if (char_stats[i] > max_count)
            max_count = char_stats[i];
    }

    int max_bar_length = 25;

    for (int i = 0; i < 26; i++)
    {
        int bar_length = (int)((double)char_stats[i] / max_count * max_bar_length);
        fprintf(stderr, "Process %2d got char %c: %8ld | ", getpid(), 'a' + i, char_stats[i]);

        for (int j = 0; j < bar_length; j++)
            fprintf(stderr, "*");

        fprintf(stderr, "\n");
    }
}