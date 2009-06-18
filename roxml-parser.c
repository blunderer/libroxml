/** \file roxml-parser.c
 *  \brief command line xml parser
 *         
 * \author blunderer <blunderer@blunderer.org>
 * \date 19 Feb 2009
 *
 * Copyright (C) 2009 blunderer
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "roxml.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void print_usage (const char *progname) {
	fprintf (stderr, "usage: %s [-q] <filename> [/]<node1>/<node2>/<node3>/.../<nodeN>\n", progname) ;
}

int main(int argc, char ** argv)
{
	int option ;
	int quiet = 0 ;
	int j ,max;
	node_t *root;
	node_t *cur;
	node_t **ans;

	while ((option = getopt (argc, argv, "q")) >= 0) {
		switch (option) {
			case 'q' :
				quiet = 1 ;
				break ;
			case 'h' :
				print_usage (argv[0]) ;
				return EXIT_FAILURE ;
				break ;
			default :
				print_usage (argv[0]) ;
				return EXIT_FAILURE ;
				break;
		}
	}
	if(argc < optind + 2) {
		print_usage (argv[0]) ;
		return -1;
	}

	root = roxml_load_doc(argv[optind]);
	cur = root;
	if(root == NULL)	{
		fprintf(stdout,"no such file '%s'\n", argv[optind]);
	}


	ans = roxml_exec_path(cur, argv[optind + 1],  &max);

	for(j = 0; j < max; j++)
	{
		char *c = NULL;
		c = roxml_get_content(ans[j]);
		if (! quiet)	{
			fprintf(stdout,"ans[%d]: ", j);
		}
		fprintf(stdout,"%s\n", c);
	}
	roxml_release(RELEASE_LAST);
	roxml_release(RELEASE_LAST);
	roxml_release(RELEASE_ALL);
	roxml_release(RELEASE_ALL);
	roxml_release(RELEASE_LAST);
	roxml_release(RELEASE_LAST);

	roxml_close(root);
	return 0;
}

