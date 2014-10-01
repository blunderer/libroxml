/**
 * \file roxml_shell.c
 * \brief shell xpath solver sources
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:	LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */

#include "roxml.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_help(void)
{
	fprintf(stderr, "LGPL command line XML parser\n<blunderer@blunderer.org>\n");
}

void print_usage(const char *progname)
{
	fprintf(stderr, "\nusage: %s [-q|-h] <filename> [/]<node1>/<node2>/<node3>/.../<nodeN>\n", progname);
	fprintf(stderr, "-q|--quiet activate quiet mode\n-h|--help display this message\n");
}

int main(int argc, char **argv)
{
	int optind;
	int quiet = 0;
	int j, max;
	node_t *root;
	node_t *cur;
	node_t **ans;

	for (optind = 1; optind < argc; optind++) {
		int option = 0;
		if (argv[optind][0] == '-') {
			/* this is an option */
			if (argv[optind][1] == '-') {
				/* long option */
				if (strcmp(argv[optind], "--help") == 0) {
					option = 'h';
				} else if (strcmp(argv[optind], "--quiet") == 0) {
					option = 'q';
				}
			} else {
				/* short option */
				if (strcmp(argv[optind], "-h") == 0) {
					option = 'h';
				} else if (strcmp(argv[optind], "-q") == 0) {
					option = 'q';
				}
			}
		} else {
			break;
		}
		switch (option) {
		case 'q':
			quiet = 1;
			break;
		case 'h':
			print_help();
			print_usage(argv[0]);
			return EXIT_FAILURE;
			break;
		default:
			print_usage(argv[0]);
			return EXIT_FAILURE;
			break;
		}
	}

	if (argc < optind + 2) {
		fprintf(stderr, "wrong syntax\n");
		print_usage(argv[0]);
		return -1;
	}

	root = roxml_load_doc(argv[optind]);
	cur = root;
	if (root == NULL) {
		fprintf(stdout, "no such file '%s'\n", argv[optind]);
	}

	ans = roxml_xpath(cur, argv[optind + 1], &max);

	for (j = 0; j < max; j++) {
		char *c = NULL;

		c = roxml_get_content(ans[j], NULL, 0, NULL);
		if (c) {
			if (*c == 0) {
				char *s = NULL;
				int i = 0;
				int nb_chld = roxml_get_chld_nb(ans[j]);

				for (i = 0; i < nb_chld; i++) {
					node_t *child = roxml_get_chld(ans[j], NULL, i);
					node_t *ns = roxml_get_ns(child);
					c = roxml_get_name(child, NULL, 0);
					if (!quiet) {
						fprintf(stdout, "ans[%d]: ", j);
					}
					if (ns) {
						s = roxml_get_name(ns, NULL, 0);
						if (strlen(s)) {
							fprintf(stdout, "%s:", s);
						}
					}
					fprintf(stdout, "%s\n", c);
				}
			} else {
				if (!quiet) {
					fprintf(stdout, "ans[%d]: ", j);
				}
				fprintf(stdout, "%s\n", c);
			}
		}
	}
	roxml_release(RELEASE_ALL);

	roxml_close(root);
	return 0;
}
