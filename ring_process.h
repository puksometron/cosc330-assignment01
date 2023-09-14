#ifndef RING_PROCESS_H
#define RING_PROCESS_H

/*
 * Ensures the arguments are valid for this program.
 *
 * argc: The number of arguments passed to the program.
 * argv: The array of arguments passed to the program.
 * np: Pointer to the number of processes to be used by the program.
 *
 * Returns an integer with a status code.
 */
int parse_args(int argc, char *argv[], int *np);

/*
 * Constructs a ring of processes.
 *
 * Returns an integer with a status code.
 */
int make_trivial_ring();

/*
 * Creates a new child process.
 *
 * pid: Pointer to store the process ID.
 *
 * Returns an integer with a status code.
*/
int add_new_node(int *pid);

/*
 * Processes the individual letter counts in a file.
 *
 * inFile: The path to the file.
 * char_stats: The array containing the individual letter counts.
 *
 * Returns a long with the total character count of the file.
 */
long procFileCounts(char inFile[], long char_stats[]);

/*
 * Prints the histogram of the character frequencies.
 *
 * char_stats: The array containing the individual letter counts.
 * nprocs: The number of processes in the ring.
 */
void print_histogram(const long char_stats[], int nprocs);

#endif