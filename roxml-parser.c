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

int main(int argc, char ** argv)
{
	int j ,max;
	node_t *root;
	node_t *cur;
	node_t **ans;
	
	if(argc < 2)	{
		fprintf(stderr,"usage %s <filename> [/]<node1>/<node2>/<node3>/.../<nodeN>\n",argv[0]);
		return -1;
	}
	root = roxml_load_doc(argv[1]);
	cur = root;
	
	ans= roxml_exec_path(cur, argv[2],  &max);

	for(j = 0; j < max; j++)
	{
		int size;
		char *c = NULL;
		size = roxml_get_content(ans[j], c);
		c = (char*)malloc(sizeof(char)*(size+1));
		roxml_get_content(ans[j], c);
		fprintf(stderr,"ans[%d]: %s\n",j, c);
		free(c);
	}

	free(ans);
	roxml_close(root);
	return 0;
}

