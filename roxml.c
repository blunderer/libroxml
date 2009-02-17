/** \file roxml.c
 *  \brief source for libroxml.so
 *         
 * This is the source file for lib libroxml.so
 * \author blunderer <blunderer@blunderer.org>
 * \date 23 Dec 2008
 *
 */

#ifndef ROXML_C
#define ROXML_C

#include "roxml.h"

#ifdef BUILD_AS_BIN
int main(int argc, char ** argv)
{
	if(argc == 1)	{
		return 0;
	}
	node_t *my = roxml_load_doc(argv[1]);
	roxml_close(my);
	return 0;
}
#endif /* BUILD_AS_BIN */

node_t * roxml_parent_node(node_t *parent, node_t *n)
{
	if(parent)	{
		n->fat = parent;
		if(parent->son == NULL)	{
			parent->son = n;
		} else	{
			node_t *brother = parent->son;
			while(brother->bra != NULL)	{
				brother = brother->bra;
			}
			brother->bra = n;
		}
	}

	return n;
}

node_t * roxml_new_node(int pos, FILE *file, char * buf, unsigned int * idx)
{
	node_t *n = (node_t*)malloc(sizeof(node_t));
	n->type = ((file)?FILE_FILE:FILE_BUFF);
	n->idx = idx;
	n->buf = buf;
	n->fil = file;
	n->pos = pos;
	n->end = pos;
	n->bra = NULL;
	n->son = NULL;
	n->fat = NULL;

	return n;
}

void roxml_close_node(node_t *n, node_t *close)
{
	n->end = close->pos;
	roxml_free_node(close);
}

void roxml_free_node(node_t *n)
{
	free(n);
}

void roxml_parse_node(node_t *n, char *name, char * arg, char * value, int * num, int max)
{
	char c = 0;
	int len = 0;
	int count = 0;
	int mode = MODE_COMMENT_NONE;
	int state = STATE_NODE_NAME;

	PUSH(n)

	while((!ROXML_FEOF(n))&&(c != '>')&&((max >= 0)?(count < max+1):(1)))      {
		c = ROXML_FGETC(n);
		if(c == '"')	{
			if(mode == MODE_COMMENT_NONE)	{
				mode = MODE_COMMENT_DQUOTE;
			} else if(mode == MODE_COMMENT_DQUOTE)	{
				mode = MODE_COMMENT_NONE;
			}
			continue;
		} else if(c == '\'')	{
			if(mode == MODE_COMMENT_NONE)	{
				mode = MODE_COMMENT_QUOTE;
			} else if(mode == MODE_COMMENT_QUOTE)	{
				mode = MODE_COMMENT_NONE;
			}
			continue;
		}
		
		if(mode == MODE_COMMENT_NONE)	{
			if((c == ' ')||(c == '\t'))	{
				if(state == STATE_NODE_NAME)	{
					state = STATE_NODE_ARG;
					len = 0;
				} else if(state == STATE_NODE_ARG)	{
					if(len > 0)	{
						len = 0;
						count++;
					}
				} else if(state == STATE_NODE_ARGVAL)     {
					if(len > 0)	{
						state = STATE_NODE_ARG;
						len = 0;
						count++;
					}
				}
			} else if(c == '=')	{
				if(state == STATE_NODE_ARG)	{
					state = STATE_NODE_ARGVAL;
					len = 0;
				}
			} else if(c == '/')	{
				;
			} else if(c == '>')	{
				if(state == STATE_NODE_ARG)	{
					if(len > 0)	{
						count++;
					}
				} else if(state == STATE_NODE_ARGVAL)	{
					if(len > 0)	{
						count++;
					}
				}
			} else if(state == STATE_NODE_NAME)    {
				if(name)	{
					name[len] = c;
					name[len+1] = '\0';
				}
				len++;
			} else if(state == STATE_NODE_ARG)    {
				if(arg)	{
					arg[len] = c;
					arg[len+1] = '\0';
				}
				len++;
			} else if(state == STATE_NODE_ARGVAL)    {
				if(value)	{
					value[len] = c;
					value[len+1] = '\0';
				}
				len++;
			}
		} else	{
			if(state == STATE_NODE_ARG)	{
				if(arg)	{
					arg[len] = c;
					arg[len+1] = '\0';
				}
				len++;
			} else if(state == STATE_NODE_ARGVAL)	{
				if(value)	{
					value[len] = c;
					value[len+1] = '\0';
				}
				len++;
			}
		}
	}

	if(num)	{
		*num = count;
	}

	POP(n)
}

int roxml_get_content(node_t *n, char *content)
{
	char c = 0;
	int len = 0;
	long long int start = 0;	

	PUSH(n)
	
	while((!ROXML_FEOF(n))&&(c != '>'))      {
		c = ROXML_FGETC(n);
	}

	if(c == '>')	{
		start = ROXML_FTELL(n);
		len = n->end - start - 1;
		if(len > 0)	{
			if(content)	{
				fread(content, len, sizeof(char), n->fil);
				content[len] = '\0';
			}
		} else	{
			len = 0;
			if(content)	{
				content[0] = '\0';
			}
		}
	}

	POP(n)
	return len;
}

char * roxml_get_name(node_t *n)
{
	char * name = (char*)malloc(sizeof(char)*512);
	memset(name, 0, sizeof(char)*512);

	if(n->fat == NULL)	{
		strcpy(name,"root");
	} else	{
		roxml_parse_node(n, name, NULL, NULL, NULL, 1);
	}
	return name;
}

int ROXML_API roxml_get_nb_attr(node_t *n)
{
	int count;
	roxml_parse_node(n, NULL, NULL, NULL, &count, -1);
	return count;
}

char *roxml_get_attr_val_nth(node_t *n, int nb)
{
	char * val = (char*)malloc(sizeof(char)*512);
	memset(val, 0, sizeof(char)*512);
	
	roxml_parse_node(n, NULL, NULL, val, NULL, nb);

	return val;
}

char *roxml_get_attr_nth(node_t *n, int nb)
{
	char * arg = (char*)malloc(sizeof(char)*512);
	memset(arg, 0, sizeof(char)*512);
	
	roxml_parse_node(n, NULL, arg, NULL, NULL, nb);

	return arg;
}

void roxml_close(node_t *n)
{
	node_t *root = n;
	if(root == NULL)	{
		return;
	}
	while(root->fat != NULL)	{
		root = root->fat;
	}
	roxml_del_tree(root->son);
	roxml_del_tree(root->bra);
	if(root->type == FILE_FILE)	{
		fclose(root->fil);
	} else if(root->type == FILE_BUFF)	{
		free(root->idx);
	}
	free(root);
}

void roxml_del_tree(node_t *n)
{
	if(n == NULL)	{
		return;
	}
	roxml_del_tree(n->son);
	roxml_del_tree(n->bra);
	free(n);
	return;
}

int roxml_get_son_nb(node_t *n)
{
	node_t *ptr = n;
	int nb = 0;
	if(ptr->son)	{
		ptr = ptr->son;
		nb++;
		while(ptr->bra)	{
			nb++;
			ptr = ptr->bra;
		}
	}
	return nb;
}

node_t *roxml_get_son_nth(node_t *n, int nb)
{
	int count = 0;
	node_t *ptr = n;
	if(nb == 0)	{
		return n->son;
	}
	ptr = n->son;
	while((ptr->bra)&&(nb > count)) {
		count++;
		ptr = ptr->bra;
	}
	return ptr;
}

node_t *roxml_get_parent(node_t *n)
{
	if(n->fat == NULL)	{
		return n;
	} else	{
		return n->fat;
	}
	return NULL;
}

node_t * roxml_load_doc(char *filename)
{
	node_t *current_node = NULL;
	FILE* file = fopen(filename, "r");
	current_node = roxml_new_node(0, file, NULL, NULL);
	current_node = roxml_parent_node(NULL, current_node);
	return roxml_load(current_node,  file, NULL);
}

node_t * roxml_load_buf(char *buffer)
{
	unsigned int *index = (unsigned int *)malloc(sizeof(unsigned int));
	node_t *current_node = NULL;
	*index = 0;
	current_node = roxml_new_node(0, NULL, buffer, index);
	current_node = roxml_parent_node(NULL, current_node);
	return roxml_load(current_node, NULL, buffer);
}

node_t * roxml_load(node_t *current_node, FILE *file, char *buffer)
{
	int state = STATE_NODE_NONE;
	int mode = MODE_COMMENT_NONE;
	node_t *candidat_node = NULL;

	while(!ROXML_FEOF(current_node))	{
		char c = ROXML_FGETC(current_node);

		if(state != STATE_NODE_CONTENT)	{
			if(c == '"')	{
				if(mode == MODE_COMMENT_NONE)	{
					mode = MODE_COMMENT_DQUOTE;
				} else if(mode == MODE_COMMENT_DQUOTE)	{
					mode = MODE_COMMENT_NONE;
				}
			} else if(c == '\'')	{
				if(mode == MODE_COMMENT_NONE)	{
					mode = MODE_COMMENT_QUOTE;
				} else if(mode == MODE_COMMENT_QUOTE)	{
					mode = MODE_COMMENT_NONE;
				}
			}
		}
		
		if(mode == MODE_COMMENT_NONE)	{
			if(c == '<')	{
				state = STATE_NODE_BEG;
				candidat_node = roxml_new_node(ROXML_FTELL(current_node), file, buffer, current_node->idx);
			} else if(c == '>')	{
				if(state == STATE_NODE_NAME)	{
					state = STATE_NODE_CONTENT;
					current_node = roxml_parent_node(current_node, candidat_node);
#ifdef BUILD_AS_BIN
					fprintf(stderr,"new node '%s'\n",roxml_get_name(current_node));
#endif /* BUILD_AS_BIN */
				} else if(state == STATE_NODE_ATTR)	{
#ifdef BUILD_AS_BIN
					int nb, i;
#endif /* BUILD_AS_BIN */
					state = STATE_NODE_CONTENT;
					current_node = roxml_parent_node(current_node, candidat_node);
#ifdef BUILD_AS_BIN
					nb = roxml_get_nb_attr(current_node);
					fprintf(stderr,"new node with attr '%s'\n",roxml_get_name(current_node));
					fprintf(stderr,"%d args\n",nb);
					for(i = 0; i < nb; i++)	{
						fprintf(stderr,"%d arg : %s = %s\n",i,
							roxml_get_attr_nth(current_node, i),
							roxml_get_attr_val_nth(current_node, i));
					}
#endif /* BUILD_AS_BIN */
				} else if(state == STATE_NODE_SINGLE)	{
					current_node = roxml_parent_node(current_node, candidat_node);
#ifdef BUILD_AS_BIN
					fprintf(stderr,"new single node '%s'\n",roxml_get_name(current_node));
#endif /* BUILD_AS_BIN */
					current_node = current_node->fat;
				} else if(state == STATE_NODE_END)	{
#ifdef BUILD_AS_BIN
					int len;
					char content[1024] = "";
#endif /* BUILD_AS_BIN */
					roxml_close_node(current_node, candidat_node);
#ifdef BUILD_AS_BIN
					len = roxml_get_content(current_node, NULL);
					if(len < 1024)	{
						roxml_get_content(current_node, content);
					}
					fprintf(stderr,"close %s\n",roxml_get_name(current_node));
					fprintf(stderr,"content %d %s\n",len, content);
#endif /* BUILD_AS_BIN */
					current_node = current_node->fat;
				}
			} else if(c == '/')	{
				if(state == STATE_NODE_BEG)	{
					state = STATE_NODE_END;
				} else if(state == STATE_NODE_NAME)	{
					state = STATE_NODE_SINGLE;
				} else if(state == STATE_NODE_ATTR)	{
					state = STATE_NODE_SINGLE;
				}
			} else if((c == ' ')||(c == '\t'))	{
				if(state == STATE_NODE_NAME)	state = STATE_NODE_ATTR;
			} else	{
				if(state == STATE_NODE_BEG)	state = STATE_NODE_NAME;
			}
		}
	}
	return current_node;
}


#endif /* ROXML_C */

