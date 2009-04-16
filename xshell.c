/** \file xshell.c
 *  \brief an xml shell to navigate inside a xml file
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

#define LTRIM(n)	while(((*n==' ')||(*n=='\t'))&&(*n!='\0')) { n++; }
#define RTRIM(n)	while((*n!=' ')&&(*n!='\t')&&(*n!='\r')&&(*n!='\n')&&(*n!='\0')) { n++; } *n = '\0';

int running = 0;

int main(int argc, char ** argv)
{
	node_t *root;
	node_t *cur;
	
	if(argc < 2)	{
		fprintf(stderr,"usage %s [buffer] <filename>\n",argv[0]);
		fprintf(stderr,"keyword buffer ask to use the buffer parsing instead of file parsing\n");
		return -1;
	}

	if(argc > 2)	{
		if(strcmp(argv[1], "buffer")==0)	{
			FILE *f;
			char * my_buffer;
			f = fopen(argv[2], "r");
			if(f)	{
				int size, ret;
				fseek(f, 0, SEEK_END);
				size = ftell(f);
				fseek(f, 0, SEEK_SET);
				my_buffer = (char*)malloc(sizeof(char)*(size+1));
				memset(my_buffer, 0, sizeof(char)*(size+1));
				ret = fread(my_buffer, 1, sizeof(char)*(size), f);
				root = roxml_load_buf(my_buffer);
				fclose(f);
			} else {
				fprintf(stderr,"no such file %s\n",argv[2]);
				exit(-1);
			}
		} else {
			fprintf(stderr,"unknown arg %s\n",argv[1]);
			exit(-1);
		}
	} else	{
		root = roxml_load_doc(argv[1]);
		if(root == NULL)	{
			fprintf(stderr,"no such file or directory\n");
			exit(-1);
		}
	}
	cur = root;
	
	running = 1;
	
	while(running)	{
		char *n;
		char *command;
		char cmd[512];
		fprintf(stdout, "[%s] > ",roxml_get_name(cur)); 
		cmd[0] = '\0';
		fgets(cmd, 511, stdin);

		n = cmd;
		LTRIM(n)
		command = n;
		if(strlen(cmd) == 0)	{
			fprintf(stdout,"\n");
			running = 0;
		} else if(strncmp(command, "ls", 2) == 0)	{
			int i, nb = roxml_get_son_nb(cur);
			fprintf(stdout,"%d elem in %s:\n\t", nb, roxml_get_name(cur));
			for(i = 0; i < nb; i++)	{
				node_t *n = roxml_get_son_nth(cur, i);
				char *name = roxml_get_name(n);
				if(roxml_get_son_nb(roxml_get_son_nth(cur, i)) > 0)	{
					fprintf(stdout,"%s/\t",name);
				} else	{
					fprintf(stdout,"%s\t",name);
				}
				free(name);
				if(i % 5 == 4)	{
					fprintf(stdout,"\n\t");
				}
			}
			fprintf(stdout,"\n");
		} else if(strncmp(command, "cd", 2) == 0)	{
			char* arg;
			int found = 0;
			n = command +2;
			LTRIM(n)
			arg = n;
			RTRIM(n)
			if(strcmp(arg, "..") == 0)	{
				found = 1;
				cur = roxml_get_parent(cur);
			} else	{
				int i, nb = roxml_get_son_nb(cur);
				for(i = 0; i < nb && !found; i++)	{
					if(strcmp(roxml_get_name(roxml_get_son_nth(cur, i)), arg)==0)	{
						found = 1;
						if(roxml_get_son_nb(roxml_get_son_nth(cur, i)) > 0)	{
							cur = roxml_get_son_nth(cur, i);
						} else	{
							fprintf(stdout,"This is a child node\n");
						}
					}
				}
			}
			if(!found)	{
				fprintf(stdout,"No such node\n");
			}
		} else if(strncmp(command, "ncd", 2) == 0)	{
			char* arg;
			int argn = 0;
			int found = 0;
			n = command +3;
			LTRIM(n)
			argn = atoi(n);
			RTRIM(n)
			n++;
			LTRIM(n)
			arg = n;
			RTRIM(n)
			int i, nb = roxml_get_son_nb(cur);
			for(i = 0; i < nb && !found; i++)	{
				if(strcmp(roxml_get_name(roxml_get_son_nth(cur, i)), arg)==0)	{
					argn--;
					if(argn == 0)	{
						found = 1;
						if(roxml_get_son_nb(roxml_get_son_nth(cur, i)) > 0)	{
							cur = roxml_get_son_nth(cur, i);
						} else	{
							fprintf(stdout,"This is a child node\n");
						}
					}
				}
			}
			if(!found)	{
				fprintf(stdout,"No such node\n");
			}
		} else if(strncmp(command, "cat", 3) == 0)	{
			int len;
			char* content;
			char* arg;
			int found = 0;
			n = command +3;
			LTRIM(n)
			arg = n;
			RTRIM(n)
			int i, nb = roxml_get_son_nb(cur);
			for(i = 0; i < nb && !found; i++)	{
				if(strcmp(roxml_get_name(roxml_get_son_nth(cur, i)), arg)==0)	{
					found = 1;
					if(roxml_get_son_nb(roxml_get_son_nth(cur, i)) > 0)	{
						fprintf(stdout,"This is not a child node\n");
					} else	{
						len = roxml_get_content(roxml_get_son_nth(cur, i), NULL);
						content = (char*)malloc(sizeof(char)*(len+1));
						roxml_get_content(roxml_get_son_nth(cur, i), content);
						fprintf(stdout,"%s\n",content);
						free(content);
					}
				}
			}
			if(!found)	{
				fprintf(stdout,"No such node\n");
			}
		} else if(strncmp(command, "ncat", 4) == 0)	{
			int len;
			char* content;
			char* arg;
			int argn;
			int found = 0;
			n = command +4;
			LTRIM(n)
			argn = atoi(n);
			RTRIM(n)
			n++;
			LTRIM(n)
			arg = n;
			RTRIM(n)
			int i, nb = roxml_get_son_nb(cur);
			for(i = 0; i < nb && !found; i++)	{
				if(strcmp(roxml_get_name(roxml_get_son_nth(cur, i)), arg)==0)	{
					argn--;
					if(argn == 0)	{
						found = 1;
						if(roxml_get_son_nb(roxml_get_son_nth(cur, i)) > 0)	{
							fprintf(stdout,"This is not a child node\n");
						} else	{
							len = roxml_get_content(roxml_get_son_nth(cur, i), NULL);
							content = (char*)malloc(sizeof(char)*(len+1));
							roxml_get_content(roxml_get_son_nth(cur, i), content);
							fprintf(stdout,"%s\n",content);
							free(content);
						}
					}
				}
			}
			if(!found)	{
				fprintf(stdout,"No such node\n");
			}
		} else if(strncmp(command, "file", 4) == 0)	{
			int nb, i, j;
			char* arg;
			int found = 0;
			n = command +4;
			LTRIM(n)
			arg = n;
			RTRIM(n)
			nb = roxml_get_son_nb(cur);
			for(i = 0; i < nb && !found; i++)	{
				if(strcmp(roxml_get_name(roxml_get_son_nth(cur, i)), arg)==0)	{
					found = 1;
					if(roxml_get_son_nb(roxml_get_son_nth(cur, i)) > 0)	{
						fprintf(stdout,"%s is a node with %d arguments:\n", roxml_get_name(roxml_get_son_nth(cur, i)),
							roxml_get_attr_nb(roxml_get_son_nth(cur, i)));
					} else	{
						fprintf(stdout,"%s is a child node with %d arguments:\n",roxml_get_name(roxml_get_son_nth(cur, i)),
							roxml_get_attr_nb(roxml_get_son_nth(cur, i)));
					}
					nb = roxml_get_attr_nb(roxml_get_son_nth(cur, i));
					for(j = 0; j < nb; j++) {
						fprintf(stdout,"\t%s=%s\n", roxml_get_attr_nth(roxml_get_son_nth(cur, i), j),
								roxml_get_attr_val_nth(roxml_get_son_nth(cur, i), j));
					}
				}
			}
			if(!found)	{
				fprintf(stdout,"No such node\n");
			}
		} else if(strncmp(command, "nfile", 5) == 0)	{
			int nb, i, j;
			char* arg;
			int argn;
			int found = 0;
			n = command +5;
			LTRIM(n)
			argn = atoi(n);
			RTRIM(n)
			n++;
			LTRIM(n)
			arg = n;
			RTRIM(n)
			nb = roxml_get_son_nb(cur);
			for(i = 0; i < nb && !found; i++)	{
				if(strcmp(roxml_get_name(roxml_get_son_nth(cur, i)), arg)==0)	{
					argn--;
					if(argn == 0)	{
						found = 1;
						if(roxml_get_son_nb(roxml_get_son_nth(cur, i)) > 0)	{
							fprintf(stdout,"%s is a node with %d arguments:\n", roxml_get_name(roxml_get_son_nth(cur, i)),
								roxml_get_attr_nb(roxml_get_son_nth(cur, i)));
						} else	{
							fprintf(stdout,"%s is a child node with %d arguments:\n",roxml_get_name(roxml_get_son_nth(cur, i)),
								roxml_get_attr_nb(roxml_get_son_nth(cur, i)));
						}
						nb = roxml_get_attr_nb(roxml_get_son_nth(cur, i));
						for(j = 0; j < nb; j++) {
							fprintf(stdout,"\t%s=%s\n", roxml_get_attr_nth(roxml_get_son_nth(cur, i), j),
									roxml_get_attr_val_nth(roxml_get_son_nth(cur, i), j));
						}
					}
				}
			}
			if(!found)	{
				fprintf(stdout,"No such node\n");
			}
		} else if(strncmp(command, "exit", 4) == 0)	{
			running = 0;
		} else if(strncmp(command, "quit", 4) == 0)	{
			running = 0;
		}
	}

	roxml_close(root);
	return 0;
}

