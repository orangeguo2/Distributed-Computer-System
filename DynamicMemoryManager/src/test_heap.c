/*
 * test_heap.c
 *
 * @since 2019-02-20
 * @author philip gust
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include "mm_heap.h"

/**
 * usage - Explain the command line arguments
 */
static void usage(void) {
    fprintf(stderr, "Usage: test_heap [-hvd]] <file1> [...<file>]\n");
    fprintf(stderr, "Options\n");
    fprintf(stderr, "\t-h         Print this message.\n");
    fprintf(stderr, "\t-v         Print detailed performance info.\n");
    fprintf(stderr, "\t-d         Print debug information.\n");
    fprintf(stderr, "\t<file>     Use <file> as the trace file.\n");
}

/** Structure for individual trace results */
typedef struct {
	char *traceName;
	int leaks;
	int errors;
	int ops;
	float secs;
} TraceInfo;

/**
 * Program processes trace files.
 * @param argc the argument count
 * @param argv the argument array
 */
int main(int argc, char *argv[]) {
	char c;
	bool verbose = false;
	bool debug = false;
    while ((c = getopt(argc, argv, "dhv")) != EOF) {
        switch (c) {
        case 'd':
        	debug = true;
        	break;
        case 'v': /* Print per-trace performance breakdown */
            verbose = true;
            break;
        case 'h': /* Print this message */
        	usage();
            return EXIT_SUCCESS;
        default:
        	usage();
            return EXIT_FAILURE;
        }
    }

    // ensure trace files specified
    if (optind == argc) {
    	fprintf(stderr, "one or more trace files required.\n");
    	usage();
    	return EXIT_FAILURE;
    }

    // init memory model with default size
//    fprintf(stderr, "calling mm_init\n");

    mm_init();

    // allocate array for trace results
    TraceInfo results[argc-optind];

    int traceindex = 0;
    for (int index = optind; index < argc; index++, traceindex++) {
		results[traceindex].traceName = argv[index];

		if (verbose) fprintf(stderr, "Opening trace file: %s\n", results[traceindex].traceName);
		FILE *tracefile = fopen(results[traceindex].traceName, "r");
		if (tracefile == NULL) {
			results[traceindex].ops = 0;
			if (verbose) fprintf(stderr, "Missing trace file: %s\n\n", results[traceindex].traceName);
			continue;
		}

		int heapsize;
		int num_ids;
		int num_ops;
		int weight;

		fscanf(tracefile, "%d", &heapsize); /* not used */
		fscanf(tracefile, "%d", &num_ids);
		fscanf(tracefile, "%d", &num_ops);
		fscanf(tracefile, "%d", &weight);        /* not used */

		/* We'll keep an array of pointers to the allocated blocks here... */
		size_t block_sizes[num_ids];
		memset(block_sizes, 0, num_ids * sizeof(size_t));

		void* blocks[num_ids];
		memset(blocks, 0, num_ids * sizeof(void*));

		/* read every request line in the trace file */
		int index = 0;
		int op_index = 0;
		int max_index = num_ids-1;
		int size;
		char type[2];
		int nerrors = 0;
		clock_t elapsed_time = 0;
		if (debug || verbose) fprintf(stderr, "Processing trace file %s\n",
				results[traceindex].traceName);

		while (fscanf(tracefile, "%s", type) != EOF) {
			switch(type[0]) {
			case 'a':
				fscanf(tracefile, "%u %u", &index, &size);
				if (debug && verbose) fprintf(stderr, "  Allocating block %u size %u\n", index, size);
				if (blocks[index] != NULL) {
					if (debug) fprintf(stderr, "  Block %u already allocated\n", index);
					nerrors++;
				} else {
					max_index = (index > max_index) ? index : max_index;
					time_t t = clock();
//					 fprintf(stderr, "\n mm_malloc was called with size of %zu \n",size);
					blocks[index] = mm_malloc(size);
//					 fprintf(stderr, "\n block index %d get address at %p \n",index,blocks[index]);
					elapsed_time += clock()-t;
					if (blocks[index] == NULL) {
						if (debug) fprintf(stderr, "  Block %u not allocated\n", index);
						nerrors++;
					} else {
						if (debug && verbose) fprintf(stderr, "  Allocated block %u size %u\n", index, size);
						/*
						 * fill range with low byte of index to make sure that the old
						 * data was copied to the new block on realloc or free
						 */
						memset(blocks[index], (index & 0xFF), size);
						block_sizes[index] = size;
					}
				}
//				fprintf(stderr, "\ngo out the a case\n");
				break;
			case 'r':
				fscanf(tracefile, "%u %u", &index, &size);
				if (debug && verbose) fprintf(stderr, "  Reallocating block %u size %u\n", index, size);
				if (blocks[index] == NULL) {
					if (debug) fprintf(stderr, "  Block %u not reallocated\n", index);
					nerrors++;
				} else {
					for (int i = 0; i < block_sizes[index]; i++) {
						if (*((char*)blocks[index]+i) != (char)(index & 0xFF)) {
							if (debug) fprintf(stderr, "  Block %u has unexpected data before realloc.\n", index);
							nerrors++;
							/*
							 * re-fill range with low byte of index to make sure that the old
							 * data was copied to the new block on realloc or free
							 */
							memset(blocks[index], (index & 0xFF), block_sizes[index]);
							break;
						}
					}
					time_t t = clock();
					void *b = mm_realloc(blocks[index], size);
					elapsed_time += clock()-t;
					if (b == NULL) {
						if (debug) fprintf(stderr, "  Unable to realloc block %u to size %u\n", index, size);
						nerrors++;
					} else {
						if (debug && verbose) fprintf(stderr, "  Reallocated block %u size %u\n", index, size);
						blocks[index] = b;
						for (int i = 0; i < block_sizes[index]; i++) {
							if (*((char*)blocks[index]+i) != (char)(index & 0xFF)) {
								if (debug) fprintf(stderr, "  Block %u has unexpected data after reallocation.\n", index);
								nerrors++;
								break;
							}
						}
						/*
						 * re-fill range with low byte of index to make sure that the old
						 * data was copied to the new block on realloc or free
						 */
						memset(blocks[index], (index & 0xFF), size);
						block_sizes[index] = size;
					}
				}
				break;
			case 'f':
//				fprintf(stderr, "\ninside the f\n");
				fscanf(tracefile, "%u", &index);
				if (blocks[index] == NULL) {
					if (debug) fprintf(stderr, "  Block %u not allocated\n", index);
					nerrors++;
				} else {
					if (debug & verbose) fprintf(stderr, "  Freeing block %u size %zu\n", index, block_sizes[index]);
					for (int i = 0; i < block_sizes[index]; i++) {
						if (*((char*)blocks[index]+i) != (char)(index & 0xFF)) {
							if (debug) fprintf(stderr, "  Block %u has unexpected data before free and i = %d.\n", index,i);
							nerrors++;
							break;
						}
					}
					time_t t = clock();
//					 fprintf(stderr, "\n free the block index %d, pass address to funciton %p \n",index,blocks[index]);

					mm_free(blocks[index]);
					elapsed_time += clock()-t;
					if (debug & verbose) fprintf(stderr, "  Freed block %u size %zu\n", index, block_sizes[index]);
					blocks[index] = NULL;
					block_sizes[index] = 0;
				}
				break;
			default:
				if (debug) fprintf(stderr, "Invalid type character (%c) in tracefile %s\n",
									type[0], results[traceindex].traceName);
				nerrors++;
			}

			op_index++;
		}
		fclose(tracefile);

		if (debug || verbose) fprintf(stderr, "Done processing trace file %s\n",
				results[traceindex].traceName);

		assert(max_index == num_ids - 1);
		assert(num_ops == op_index);

		results[traceindex].leaks = 0;
		results[traceindex].errors = nerrors;

		// tally and report leaks
		char *newline = "\n";
		for (int i = 0; i < num_ids; i++) {
			if (blocks[index] != NULL) {
				if (debug) fprintf(stderr, "%sblock %d not freed, size=%zu\n", newline, i, block_sizes[i]);
				results[traceindex].leaks++;
				newline = "";
			}
		}

		if (debug || verbose) fprintf(stderr, "Errors: %d, leaks: %d\n\n",
				results[traceindex].errors, results[traceindex].leaks);

		results[traceindex].secs = ((double) (elapsed_time)) / CLOCKS_PER_SEC;
		results[traceindex].ops = op_index;

		// reset memory model for next test
		mm_reset();
	}


    /* Print the individual results for each trace */
    if (verbose) fprintf(stderr, "\nResults for traces:\n");
	fprintf(stderr, "%5s%7s%7s%8s%10s%8s  %s\n",
	   "index", "leaks", "errors", "ops", "secs", "Kops", "file");

    for (int i = 0; i < traceindex; i++) {
    	if (results[i].ops > 0) {
			fprintf(stderr, "%5d%7d%7d%8d%10.6f%8d  %s\n",
					i+1, results[i].leaks, results[i].errors, results[i].ops, results[i].secs,
					(int)(results[i].ops/1e3/results[i].secs), results[i].traceName);
    	}
    }

    // deinitialize memory model
    mm_deinit();

    return EXIT_SUCCESS;
}
