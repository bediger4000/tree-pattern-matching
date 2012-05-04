/* $Id$ */

/*
 *
 */

#include <stdio.h>
#include <stdlib.h>  /* free() */
#include <unistd.h>  /* getopt() */

#include <tree.h>
#include <buffer.h>
#include <aho_corasick.h>
#include <algorithm_d.h>

void usage(char *progname);

int
main(int ac, char **av)
{
	char *pat_string = NULL;
	char *subject_string = NULL;
	int debug = 0;
	int cc;

	while (-1 != (cc = getopt(ac, av, "dhHp:P:s:S:x")))
	{
		switch (cc)
		{
		case 'd': ++debug; break;
		case 'p':
		case 'P':
			pat_string = optarg;
			break;
		case 's':
		case 'S':
			subject_string = optarg;
			break;
		case 'h':
		case 'H':
		case 'x':
			usage(av[0]);
			exit(1);
			break;
		}
	}

	struct tree *pattern = read_tree(&pat_string);

	if (pattern)
	{
		int i, pat_path_cnt;
		char **paths;
		struct tree *subject;
		struct gto *g;

		if (debug)
		{
			printf("Pattern as string: \"%s\"\n", pat_string);
			printf("Pattern (%d nodes): ", pattern->tree_size);
			print_tree(pattern, stdout);
			fputc('\n', stdout);
		}

		pat_path_cnt = set_pattern_paths(pattern);
		paths = get_pat_paths();

		free_tree(pattern);

		g = init_goto();

		if (debug)
		{
			printf("Found %d paths through pattern\n", pat_path_cnt);

			for (i = 0; i < pat_path_cnt; ++i)
				printf("path %d: %s\n", i, paths[i]);
		}


		if (subject_string)
		{
			if (debug)
				printf("Subject as string: \"%s\"\n", subject_string);

			subject = read_tree(&subject_string);

			if (subject)
			{
				if (debug)
				{
					printf("Subject tree (%d nodes): ", subject->tree_size);
					printf("Subject tree: ");
					print_tree(subject, stdout);
					fputc('\n', stdout);
				}

				construct_goto(paths, pat_path_cnt, g);
				construct_failure(g);
				construct_delta(g);

				for (i = 0; i < pat_path_cnt; ++i)
					free(paths[i]);
				free(paths);

				algorithm_d(g, subject, pat_path_cnt);

				free_tree(subject);

			} else
				fprintf(stderr, "Bad parse of subject \"%s\"\n", subject_string);
		} else
				fprintf(stderr, "No subject tree as string\n");

		destroy_goto(g);

	} else {
		if (pat_string)
			fprintf(stderr, "Bad parse of pattern \"%s\"\n", pat_string);
		else
			fprintf(stderr, "No pattern.\n");
	}
	
	return 0;
}

void usage(char *progname)
{
	printf("%s: binary tree pattern matcher\n", progname);
}
