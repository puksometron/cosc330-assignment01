/*
 * COSC330 Assignment 01 - By Vladimir Ovechkin
 * This program uses process rings to concurrently count the total
 * number of individual characters across a number of text files.
 *
 * To compile this program, use the "make" command from the Terminal.
 * To run this program, enter ./char_stats.o -np -dir, where np is the
 * number of processes you want to use and dir is the directory to the
 * included text files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <dirent.h>
#include "ring_process.h"

int main(int argc, char *argv[])
{
    int i;
    int childpid;
    int nprocs = argc;

    if (parse_args(argc, argv, &nprocs) < 0)
    {
        fprintf(stderr, "Usage: %s nprocs directory\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (make_trivial_ring() < 0)
    {
        perror("Could not make trivial ring");
        exit(EXIT_FAILURE);
    }

    for (i = 1; i <= nprocs; i++)
    {
        if (add_new_node(&childpid) < 0)
        {
            perror("Could not add new node to ring");
            exit(EXIT_FAILURE);
        }

        if (childpid)
            break;
    }

    long char_stats[26] = {0};

    DIR *dir;
    struct dirent *ent;
    dir = opendir(argv[2]);

    if (dir != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_type == DT_REG)
            {
                char filename[256];
                strcpy(filename, ent->d_name);
                procFileCounts(filename, char_stats);
            }
        }

        closedir(dir);
    }
    else
    {
        perror("Failed to open directory");
        exit(EXIT_FAILURE);
    }

    if (i == 1)
    {
        if (write(STDOUT_FILENO, char_stats, sizeof(char_stats)) == -1)
        {
            perror("Failed to send data to next process");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        long prev_char_stats[26];

        if (read(STDIN_FILENO, prev_char_stats, sizeof(prev_char_stats)) == -1)
        {
            perror("Failed to receive data from previous process");
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < 26; j++)
            char_stats[j] += prev_char_stats[j];

        if (write(STDOUT_FILENO, char_stats, sizeof(char_stats)) == -1)
        {
            perror("Failed to send data to next process");
            exit(EXIT_FAILURE);
        }
    }

    if (i == nprocs)
    {
        long final_char_stats[26];

        if (read(STDIN_FILENO, final_char_stats, sizeof(final_char_stats)) == -1)
        {
            perror("Failed to receive final data");
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < 26; j++)
            char_stats[j] += final_char_stats[j];

        print_histogram(char_stats, nprocs);
    }

    exit(EXIT_SUCCESS);
}