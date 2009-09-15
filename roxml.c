/** \file roxml.c
 *  \brief source for libroxml.so
 *         
 * This is the source file for lib libroxml.so
 * \author blunderer <blunderer@blunderer.org>
 * \date 23 Dec 2008
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

#ifndef ROXML_C
#define ROXML_C

#include "roxml-internal.h"

memory_cell_t head_cell = {PTR_NONE, 0, NULL, 0, NULL, NULL};

node_t * roxml_parent_node(node_t *parent, node_t *n)
{
	n->prnt = parent;
	if(parent)	{
		if(roxml_get_type(n) == ROXML_ARG)	{
			if(parent->attr == NULL)	{
				parent->attr = n;
			} else	{
				node_t *brother = parent->attr;
				while(brother->sibl != NULL)	{
					brother = brother->sibl;
				}
				brother->sibl = n;
			}
		} else if(roxml_get_type(n) == ROXML_TXT){
			if(parent->text == NULL)	{
				parent->text = n;
			} else	{
				node_t *brother = parent->text;
				while(brother->sibl != NULL)	{
					brother = brother->sibl;
				}
				brother->sibl = n;
			}
		} else if(roxml_get_type(n) == ROXML_VAL){
			if(parent->chld == NULL)	{
				parent->chld = n;
			} else	{
				node_t *brother = parent->chld;
				while(brother->sibl != NULL)	{
					brother = brother->sibl;
				}
				brother->sibl = n;
			}
		}
	}

	return n;
}

node_t * roxml_create_node(int pos, FILE *file, char * buf, unsigned int * idx, int type)
{
	node_t *n = (node_t*)malloc(sizeof(node_t));
	n->type = type;
	n->idx = idx;
	n->buf = buf;
	n->fil = file;
	n->pos = pos;
	n->end = pos;
	n->prv = 0;
	n->sibl = NULL;
	n->chld = NULL;
	n->text = NULL;
	n->prnt = NULL;
	n->attr = NULL;

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
			if(ROXML_WHITE(c))	{
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

char * roxml_get_content(node_t *n, char * name, int size)
{
	node_t * ptr;
	int total = 0;
	char * content = NULL;

	if(n == NULL)	{
		if(name)	{
			strcpy(name, "");
		}
		return NULL;
	}

	if(n->type & ROXML_VAL)	{
		ptr = n->text;
		while(ptr)	{
			total += ptr->end - ptr->pos;
			ptr = ptr->sibl;
		}
		
		content = roxml_malloc(sizeof(char), total+1, PTR_CHAR);
		if(content == NULL) { return NULL; }
		memset(content, 0, sizeof(char) * (total+1));

		total = 0;
		ptr = n->text;
		while(ptr)	{
			PUSH(ptr)
			ROXML_FREAD(content+total, ptr->end - ptr->pos, 1, n)
			total += ptr->end - ptr->pos;
			POP(ptr)
			ptr = ptr->sibl;
		}

	} else if(n->type & ROXML_ARG)	{
		PUSH(n->text)

		total = n->text->end - n->text->pos;
		content = roxml_malloc(sizeof(char), total+1, PTR_CHAR);
		if(content == NULL) { return NULL; }
		memset(content, 0, sizeof(char) * (total+1));
		ROXML_FREAD(content, total, 1, n)

		POP(n->text)
	}
	if(name)	{
		strncpy(name, content, size);
	}
	
	return content;
}

char * roxml_get_name(node_t *n, char * name, int size)
{
	char c;
	int count = 0;
	char tmp_name[512];
	memset(tmp_name, 0, 512*sizeof(char));

	if(n == NULL)	{
		if(name)	{
			strcpy(name, "");
		}
		return NULL;
	}

	PUSH(n)

	if(n->prnt == NULL)	{
		strcpy(tmp_name, "root");
	} else if(n->type & ROXML_ARG)	{
		c = ROXML_FGETC(n); 
		while(ROXML_WHITE(c)) { c = ROXML_FGETC(n); }
		while(c != '=') { 
			tmp_name[count] = c;
			c = ROXML_FGETC(n); 
			count++;
		}
	} else if(n->type & ROXML_VAL)	{
		c = ROXML_FGETC(n); 
		while((c == '<')||(ROXML_WHITE(c))) { c = ROXML_FGETC(n); }
		while((c != '>')&&(!ROXML_WHITE(c))) { 
			tmp_name[count] = c;
			c = ROXML_FGETC(n); 
			count++;
		}
	} else if(n->type & ROXML_TXT) {
		strcpy(tmp_name, "");
	}

	POP(n)

	
	if(name == NULL)	{
		name = (char*)roxml_malloc(sizeof(char), strlen(tmp_name)+1, PTR_CHAR);
		strcpy(name, tmp_name);
	} else	{
		strncpy(name, tmp_name, size);
	}
	return name;
}

int ROXML_API roxml_get_attr_nb(node_t *n)
{
	node_t *ptr = n;
	int nb = 0;
	if(ptr->attr)	{
		ptr = ptr->attr;
		nb++;
		while(ptr->sibl)	{
			nb++;
			ptr = ptr->sibl;
		}
	}
	return nb;
}

node_t *roxml_get_attr(node_t *n, char * name, int nb)
{
	int count = 0;
	node_t *ptr = n;
	ptr = n->attr;
	
	if(ptr == NULL)	{
		return NULL;
	}

	if(name == NULL)	{
		if(nb == 0)	{
			return ptr;
		}
		while((ptr->sibl)&&(nb > count)) {
			count++;
			ptr = ptr->sibl;
		}
		if(nb > count)	{ return NULL; }
	} else	{
		while(ptr) {
			int ans = strcmp(roxml_get_name(ptr, NULL, 0), name);
			roxml_release(RELEASE_LAST);
			if(ans == 0)	{
				return ptr;
			}
			ptr = ptr->sibl;
		}
	}
	return ptr;
}

void roxml_close(node_t *n)
{
	node_t *root = n;
	if(root == NULL)	{
		return;
	}
	while(root->prnt != NULL)	{
		root = root->prnt;
	}
	roxml_del_tree(root->chld);
	roxml_del_tree(root->text);
	roxml_del_tree(root->sibl);
	roxml_del_tree(root->attr);
	if((root->type & ROXML_FILE) == ROXML_FILE)	{
		fclose(root->fil);
	} else if((root->type & ROXML_BUFF) == ROXML_BUFF)	{
		free(root->idx);
	}
	free(root);
}

void roxml_del_tree(node_t *n)
{
	if(n == NULL)	{
		return;
	}
	roxml_del_tree(n->chld);
	roxml_del_tree(n->text);
	roxml_del_tree(n->sibl);
	roxml_del_tree(n->attr);
	free(n);
	return;
}

int roxml_get_chld_nb(node_t *n)
{
	node_t *ptr = n;
	if(ptr == NULL)	{ return -1; }
	int nb = 0;
	if(ptr->chld)	{
		ptr = ptr->chld;
		nb++;
		while(ptr->sibl)	{
			nb++;
			ptr = ptr->sibl;
		}
	}
	return nb;
}

node_t *roxml_get_chld(node_t *n, char * name, int nb)
{
	int count = 0;
	node_t *ptr = n;
	ptr = n->chld;
	if(name == NULL)	{
		if(nb == 0)	{
			return ptr;
		}
		while((ptr->sibl)&&(nb > count)) {
			count++;
			ptr = ptr->sibl;
		}
		if(nb > count)	{ return NULL; }
	} else	{
		while(ptr) {
			int ans = strcmp(roxml_get_name(ptr, NULL, 0), name);
			roxml_release(RELEASE_LAST);
			if(ans == 0)	{
				return ptr;
			}
			ptr = ptr->sibl;
		}
	}
	return ptr;
}

node_t *roxml_get_parent(node_t *n)
{
	if(n->prnt == NULL)	{
		return n;
	} else	{
		return n->prnt;
	}
	return NULL;
}

node_t * roxml_load_doc(char *filename)
{
	node_t *current_node = NULL;
	FILE* file = fopen(filename, "r");
	if(file == NULL)	{
		return NULL;
	}
	current_node = roxml_create_node(0, file, NULL, NULL, ROXML_VAL | ROXML_FILE);
	current_node = roxml_parent_node(NULL, current_node);
	return roxml_load(current_node,  file, NULL);
}

node_t * roxml_load_buf(char *buffer)
{
	if(buffer == NULL)	{ return NULL; }
	unsigned int *index = (unsigned int *)malloc(sizeof(unsigned int));
	node_t *current_node = NULL;
	*index = 0;
	current_node = roxml_create_node(0, NULL, buffer, index, ROXML_VAL | ROXML_BUFF);
	current_node = roxml_parent_node(NULL, current_node);
	return roxml_load(current_node, NULL, buffer);
}

node_t * roxml_load(node_t *current_node, FILE *file, char *buffer)
{
	int state = STATE_NODE_NONE;
	int mode = MODE_COMMENT_NONE;
	int inside_node_state = STATE_INSIDE_ARG_BEG;
	node_t *candidat_node = NULL;
	node_t *candidat_txt = NULL;
	node_t *candidat_arg = NULL;
	node_t *candidat_val = NULL;
	int current_attr_quoted = 2;
	int type;

	if(file)	{ 
		type = ROXML_FILE;
	} else	{
		type = ROXML_BUFF;
	}

	while(!ROXML_FEOF(current_node))	{
		char c = ROXML_FGETC(current_node);

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
		
		if((state == STATE_NODE_ATTR)&&(inside_node_state == STATE_INSIDE_VAL_BEG))	{
			if(mode == MODE_COMMENT_NONE)	{ current_attr_quoted = 1; }
			else { current_attr_quoted = 2; }
			candidat_val = roxml_create_node(ROXML_FTELL(current_node)-1, file, buffer, current_node->idx, ROXML_TXT | type);
			candidat_val = roxml_parent_node(candidat_arg, candidat_val);
			inside_node_state = STATE_INSIDE_VAL;
		}
		if(mode == MODE_COMMENT_NONE)	{
			if(c == '<')	{
				state = STATE_NODE_BEG;
				if(candidat_txt)	{
					node_t * to_be_closed = roxml_create_node(ROXML_FTELL(current_node)-1, file, buffer, current_node->idx, ROXML_TXT | type);
					roxml_close_node(candidat_txt, to_be_closed);
					candidat_txt = NULL;
				}
				candidat_node = roxml_create_node(ROXML_FTELL(current_node), file, buffer, current_node->idx, ROXML_VAL | type);
			} else if(c == '>')	{
				if(state == STATE_NODE_NAME)	{
					state = STATE_NODE_CONTENT;
					current_node = roxml_parent_node(current_node, candidat_node);
					candidat_txt = roxml_create_node(ROXML_FTELL(current_node), file, buffer, current_node->idx, ROXML_TXT | type);
					candidat_txt = roxml_parent_node(current_node, candidat_txt);
				} else if(state == STATE_NODE_ATTR)	{
					if(inside_node_state == STATE_INSIDE_VAL)	{
						int off = current_attr_quoted;
						node_t * to_be_closed = roxml_create_node(ROXML_FTELL(current_node)-off, file, buffer, current_node->idx, ROXML_ARG | type);
						roxml_close_node(candidat_val, to_be_closed);
					}
					current_node = roxml_parent_node(current_node, candidat_node);
					state = STATE_NODE_CONTENT;
					inside_node_state = STATE_INSIDE_ARG_BEG;
					candidat_txt = roxml_create_node(ROXML_FTELL(current_node), file, buffer, current_node->idx, ROXML_TXT | type);
					candidat_txt = roxml_parent_node(current_node, candidat_txt);
					candidat_txt = roxml_create_node(ROXML_FTELL(current_node), file, buffer, current_node->idx, ROXML_TXT | type);
					candidat_txt = roxml_parent_node(current_node, candidat_txt);
				} else if(state == STATE_NODE_SINGLE)	{
					current_node = roxml_parent_node(current_node, candidat_node);
					current_node = current_node->prnt;
					candidat_txt = roxml_create_node(ROXML_FTELL(current_node), file, buffer, current_node->idx, ROXML_TXT | type);
					candidat_txt = roxml_parent_node(current_node, candidat_txt);
				} else if(state == STATE_NODE_END)	{
					roxml_close_node(current_node, candidat_node);
					current_node = current_node->prnt;
					candidat_txt = roxml_create_node(ROXML_FTELL(current_node), file, buffer, current_node->idx, ROXML_TXT | type);
					candidat_txt = roxml_parent_node(current_node, candidat_txt);
				}
			} else if(c == '/')	{
				if(state == STATE_NODE_BEG)	{
					state = STATE_NODE_END;
				} else if(state == STATE_NODE_NAME)	{
					state = STATE_NODE_SINGLE;
				} else if(state == STATE_NODE_ATTR)	{
					if(inside_node_state == STATE_INSIDE_VAL)	{
						int off = current_attr_quoted;
						node_t * to_be_closed = roxml_create_node(ROXML_FTELL(current_node)-off, file, buffer, current_node->idx, ROXML_ARG | type);
						roxml_close_node(candidat_val, to_be_closed);
					}
					inside_node_state = STATE_INSIDE_ARG_BEG;
					state = STATE_NODE_SINGLE;
				}
			} else if(ROXML_WHITE(c))	{
				if(state == STATE_NODE_NAME)	{
					state = STATE_NODE_ATTR;
					inside_node_state = STATE_INSIDE_ARG_BEG;
				} else if(state == STATE_NODE_ATTR)	{
					if(inside_node_state == STATE_INSIDE_VAL)   {
						int off = current_attr_quoted;
						node_t * to_be_closed = roxml_create_node(ROXML_FTELL(current_node)-off, file, buffer, current_node->idx, ROXML_ARG | type);
						roxml_close_node(candidat_val, to_be_closed);
						inside_node_state = STATE_INSIDE_ARG_BEG;
					}
				}
			} else	{
				if(state == STATE_NODE_BEG)	state = STATE_NODE_NAME;
				if(state == STATE_NODE_ATTR)	{
					if(inside_node_state == STATE_INSIDE_ARG_BEG)	{
						candidat_arg = roxml_create_node(ROXML_FTELL(current_node)-1, file, buffer, current_node->idx, ROXML_ARG | type);
						candidat_arg = roxml_parent_node(candidat_node, candidat_arg);
						inside_node_state = STATE_INSIDE_ARG;
					} else if((inside_node_state == STATE_INSIDE_ARG)&&(c == '='))	{
						inside_node_state = STATE_INSIDE_VAL_BEG;
						node_t * to_be_closed = roxml_create_node(ROXML_FTELL(current_node), file, buffer, current_node->idx, ROXML_ARG | type);
						roxml_close_node(candidat_arg, to_be_closed);
					}
				}
			}
		}
	}
	while(current_node->prnt)	{ current_node = current_node->prnt; }
	return current_node;
}

int roxml_parse_xpath(char *path)
{
	int cur;
	int nbaxes = 1;
	int bracket = 0;
	int parenthesys = 0;
	int quoted = 0;
	int dquoted = 0;
	int len = strlen(path);

	for(cur = 0; cur < len; cur++)	{
		if(path[cur] == '"') {
			if(!quoted) {
				dquoted = (dquoted+1)%2;
			}
		} else if(path[cur] == '\'') {
			if(!dquoted) {
				quoted = (quoted+1)%2;
			}
		} else if((path[cur] == '(')||(path[cur] == ')')) {
			if(!quoted && !dquoted) {
				parenthesys = (parenthesys+1)%2;
			}
		} else if((path[cur] == '[')||(path[cur] == ']')) {
			if(!quoted && !dquoted) {
				bracket = (bracket+1)%2;
			}
		} else if(path[cur] == '/') {
			if(!quoted && !dquoted && !parenthesys && !bracket)	{
				nbaxes++;
				path[cur] = '\0';
			}
		}
	}

	/*for(cur = 0; cur < nbaxes; cur++)	{
		printf("%d '%s'\n",cur, path);
		path+= strlen(path)+1;
	}*/
	return nbaxes;
}

int roxml_validate_axes(node_t *candidat, char *axes, node_t ***ans, int *nb, int *max, int xleave)
{
	int valid = 0;
	char intern_buff[512];

	char * name  = roxml_get_name(candidat, intern_buff, 512);

	if(strncmp(name, axes, strlen(name)) == 0)	{
		valid = 1;
	}
	if(strncmp("*", axes, strlen("*")) == 0)  {
		valid = 1;
	}

	roxml_release(name);

	if((valid)&&(xleave==1)) {
		int i = 0;
		for(i = 0; i < *nb; i++)	{
			if(candidat == (*ans)[i]) {
				return valid;
			}
		}
		if((*nb) >= (*max))	{
			int new_max = (*max)+10;
			node_t ** new_ans = roxml_malloc(sizeof(node_t*), new_max, PTR_NODE_RESULT);
			memcpy(new_ans, (*ans), new_max*sizeof(node_t*)); 
			roxml_release(*ans);
			*ans = new_ans;
			*max = new_max;
		}
		(*ans)[*nb] = candidat;
		(*nb)++;
	}


	return valid;
}

void roxml_check_node(char *xp, int again, node_t *context, node_t ***ans, int *nb, int *max, int ignore)
{
	if(again == 0)	{ return; }

	// tests axes
	if((strncmp(ROXML_L_SELF, xp, strlen(ROXML_L_SELF))==0)||(strncmp(ROXML_S_SELF, xp, strlen(ROXML_S_SELF))==0)) {
		// ROXML_L_SELF & ROXML_S_SELF
		roxml_validate_axes(context, "*", ans, nb, max, again);
		roxml_check_node(xp+strlen(xp)+1, again-1, context, ans, nb, max, ignore);
	} else if((strncmp(ROXML_L_PARENT, xp, strlen(ROXML_L_PARENT))==0)||(strncmp(ROXML_S_SELF, xp, strlen(ROXML_S_SELF))==0)) {
		// ROXML_L_PARENT & ROXML_S_SELF
		roxml_validate_axes(context->prnt, "*", ans, nb, max, again);
		roxml_check_node(xp+strlen(xp)+1, again-1, context->prnt, ans, nb, max, ignore);
	} else if(strncmp(ROXML_L_DESC_O_SELF, xp, strlen(ROXML_L_DESC_O_SELF))==0) {
		// ROXML_L_DESC_O_SELF
		roxml_check_node(xp+strlen(ROXML_L_DESC_O_SELF), again, context, ans, nb, max, ROXML_SELF_AND_DESC);
	} else if(strlen(xp)==0) {
		// ROXML_S_DESC_O_SELF
		roxml_check_node(xp+1, again-1, context, ans, nb, max, ROXML_SELF_AND_DESC);
	} else if(strncmp(ROXML_L_ATTR, xp, strlen(ROXML_L_ATTR))==0) {
		// ROXML_L_ATTR
		node_t *attribute = context->attr;
		while(attribute)  {
			if(roxml_validate_axes(attribute, xp+strlen(ROXML_L_ATTR), ans, nb, max, again))	{
				roxml_check_node(xp+strlen(xp)+1, again-1, context, ans, nb, max, ignore);
			}
			attribute = attribute->sibl;
		}
	} else if(strncmp(ROXML_S_ATTR, xp, strlen(ROXML_S_ATTR))==0) {
		// ROXML_S_ATTR
		node_t *attribute = context->attr;
		while(attribute)  {
			if(roxml_validate_axes(attribute, xp+strlen(ROXML_S_ATTR), ans, nb, max, again))	{
				roxml_check_node(xp+strlen(xp)+1, again-1, context, ans, nb, max, ignore);
			}
			attribute = attribute->sibl;
		}
	} else if(strncmp(ROXML_L_CHILD, xp, strlen(ROXML_L_CHILD))==0) {
		// ROXML_L_CHILD
		node_t *current = context->chld;
		while(current)	{
			if(roxml_validate_axes(current, xp+strlen(ROXML_L_CHILD), ans, nb, max, again))	{
				roxml_check_node(xp+strlen(xp)+1, again-1, current, ans, nb, max, ROXML_DIRECT);
			}
			current = current->sibl;
		}
	} else {
		// ROXML_S_CHILD
		node_t *current = context->chld;
		while(current)	{
			if(roxml_validate_axes(current, xp, ans, nb, max, again))	{
				roxml_check_node(xp+strlen(xp)+1, again-1, current, ans, nb, max, ROXML_DIRECT);
			}
			current = current->sibl;
		}
	}

	// if found a "decendant" axes
	if(ignore)	{
		node_t *current = context->chld;
		while(current)	{
			roxml_check_node(xp, again, current, ans, nb, max, ROXML_DESC_ONLY);
			current = current->sibl;
		}
		if(ignore == ROXML_SELF_AND_DESC)	{
			roxml_validate_axes(context, xp, ans, nb, max, again);
		}
	}

	return;
}

node_t ** roxml_xpath(node_t *n, char * path, int *nb_ans)
{
	int index = 0;
	int ansnb = 0;
	int ansmax = 18;
	node_t **ans = roxml_malloc(sizeof(node_t*), ansmax, PTR_NODE_RESULT);
	char * path_to_find = strdup(path);

	index = roxml_parse_xpath(path_to_find);

	if(path_to_find[0] == 0)	{
		if(path_to_find[1] != 0)	{
			// context node is root
			while(n->prnt) { n = n->prnt; }
		}
		index--;
		path_to_find += strlen(path_to_find)+1;
	}

	roxml_check_node(path_to_find, index, n, &ans, &ansnb, &ansmax, ROXML_DIRECT);

	if(nb_ans)	{
		*nb_ans = ansnb;
	}

	if(ansnb == 0)	{
		roxml_release(ans);
		return NULL;
	}

	return ans;
}


void roxml_resolv_path(node_t *n, char * path, int *idx, node_t ***res)
{
	int i;
	int nb_chld = 0;
	if(strlen(path) == 0)	{ return; }
	if(n == NULL)	{ return; }

	/* relative path special nodes */
	if(strncmp(path, "..", strlen("..")) == 0)	{
		char * new_path = path+strlen("..");
		if((strlen(new_path) > 0) && (strcmp(new_path,"/") != 0))	{
			roxml_resolv_path(n->prnt, new_path+1, idx, res);
		} else	{
			if(res)	{ (*res)[*idx] = n->prnt; }
			(*idx)++;
		}
	}

	/* read only attribute */
	if(strncmp(path, "@", strlen("@")) == 0)	{
		int i;
		int nb_attr = roxml_get_attr_nb(n);
		char * new_path = path+strlen("@");
		for(i = 0; i < nb_attr; i++)	{
			char *name = roxml_get_name(roxml_get_attr(n, NULL, i), NULL, 0);
			if(strcmp(new_path, name) == 0)	{
				if(res)	{ 
					node_t *arg = roxml_get_attr(n, NULL, i);
					(*res)[*idx] = arg; 
				}
				(*idx)++;
			}
			roxml_release(name);
		}
	}
	

	nb_chld = roxml_get_chld_nb(n);
	for(i = 0; i < nb_chld; i++)	{
		node_t *cur = roxml_get_chld(n, NULL, i);
		char *name = roxml_get_name(cur, NULL, 0);

		if(strncmp(name, path, strlen(name)) == 0)	{
			char * new_path = path+strlen(name);
			if(new_path[0] == '[')	{
				/* conditionnal xpath */
				if(roxml_xpath_conditionnal(cur, new_path))	{
					new_path = strstr(new_path, "]");
					if(new_path)	{
						new_path++;
						if((strlen(new_path) > 0) && (strcmp(new_path,"/") != 0))	{
							roxml_resolv_path(cur, new_path+1, idx, res);
						} else	{
							if(res)	{ (*res)[*idx] = cur; }
							(*idx)++;
						}
					}
				}
			} else	{
				if((strlen(new_path) > 0) && (strcmp(new_path,"/") != 0))	{
					roxml_resolv_path(cur, new_path+1, idx, res);
				} else	{
					if(res)	{ (*res)[*idx] = cur; }
					(*idx)++;
				}
			}
		} else /*if((path[0] == '/') && (path[1] == '/'))*/	{
			roxml_resolv_path(cur, path+0, idx, res);
		}
		roxml_release(name);
	}
}

node_t* roxml_new_node(int type, char * name, char * value)
{
	int size, close;
	node_t *node;
	
	size = 2*strlen(name) + strlen(value) + 6;
	close = strlen(name) + strlen(value) + 3;
	node = (node_t*)roxml_malloc(sizeof(node_t), 1, PTR_NODE);
	node->buf = (char*)roxml_malloc(sizeof(char), size, PTR_CHAR);
	node->idx = (unsigned int*)roxml_malloc(sizeof(unsigned int), 1, PTR_INT);
	sprintf(node->buf,"<%s>%s</%s>",name,value,name);
	node->type = ROXML_BUFF & type;
	node->chld = NULL;
	node->sibl = NULL;
	node->prnt = NULL;
	node->fil = NULL;
	*node->idx = 0;
	node->pos = 0;
	node->end = close;
	node->prv = 0;

	return node;
}

int roxml_get_type(node_t *n)
{
	return (n->type & (ROXML_ARG | ROXML_VAL | ROXML_TXT));
}

int roxml_get_node_index(node_t *n, int * last)
{
	int i = 0, idx = 0;
	int nb_sibl = 0, nb_same_sibl = 0;
	char intern_buff[512];
	char intern_buff2[512];
	char * realn = roxml_get_name(n, intern_buff, 512);

	nb_sibl = roxml_get_chld_nb(n->prnt);
	for(i = 0; i < nb_sibl; i++)	{
		node_t *tmp = roxml_get_chld(n->prnt, NULL, i);
		char * name = roxml_get_name(tmp, intern_buff2, 512);
		if(tmp == n)	{ idx = nb_same_sibl; }
		if(strcmp(name, realn) == 0)	{ nb_same_sibl++; }
		roxml_release(name);
	}
	roxml_release(realn);		

	if(last) { *last = nb_same_sibl-1; }

	if(nb_same_sibl > 1)	{ return idx; }
	return -1;
}

int roxml_xpath_conditionnal(node_t *n, char *condition)
{
	char *cond = condition+1;
	while(ROXML_WHITE(*cond))	{ cond++; }
	if(cond[0] == '@')	{
		/* condition on attribut */
		int i;
		int nb_attr = roxml_get_attr_nb(n);
		if(nb_attr == 0) { return 0; }
		for(i = 0; i < nb_attr; i++)	{
			char *name = roxml_get_name(roxml_get_attr(n, NULL, i), NULL, 0);
			if(strncmp(name, cond+1, strlen(name)) == 0)	{
				char *value = roxml_get_content(roxml_get_attr(n, NULL, i), NULL, 0);
				char *request = strstr(cond+1, "=");
				char *end = strstr(cond+1, "]");
				if((request) && (request<end))	{
					request++;
					if(strncmp(value, request, strlen(value)) == 0)	{
						roxml_release(name);
						roxml_release(value);
						return 1;
					}
				} else {
					roxml_release(value);
					roxml_release(name);
					return 1;
				}
				roxml_release(value);
			}
			roxml_release(name);
		}
		return 0;
	} else if((cond[0] >= 0x30)&&(cond[0] <= 0x39))	{
		/* condition on table id */
		int ask = atoi(cond);
		int idx = roxml_get_node_index(n, NULL);
		if(ask == idx)	{ return 1; }
		return 0;
	} else if(strncmp(cond, "last()", strlen("last()")) == 0)	{
		char *ptr = cond+strlen("last()");
		int last = 0;
		int offset = atoi(ptr);
		int idx = roxml_get_node_index(n, &last);
		if(idx == last+offset) { return 1; }
		return 0;
	} else if(strncmp(cond, "first()", strlen("first()")) == 0)	{
		char *ptr = cond+strlen("first()");
		int idx = roxml_get_node_index(n, NULL);
		int offset = atoi(ptr);
		if(idx == offset) { return 1; }
		return 0;
	} else if(strncmp(cond, "position()", strlen("position()")) == 0)	{
		/* select operator between: < > = != <= >= */
		char *op = NULL;
		char *ptr = cond+strlen("position()");
		int idx = roxml_get_node_index(n, NULL);
		if((op = strstr(ptr, "!=")))	{
			int val = atoi(op+strlen("!="));
			if(idx != val)	{ return 1; }
		} else if((op = strstr(ptr, ">=")))	{
			int val = atoi(op+strlen(">="));
			if(idx >= val)	{ return 1; }
		} else if((op = strstr(ptr, "<=")))	{
			int val = atoi(op+strlen("<="));
			if(idx <= val)	{ return 1; }
		} else if((op = strstr(ptr, "=")))	{
			int val = atoi(op+strlen("="));
			if(idx == val)	{ return 1; }
		} else if((op = strstr(ptr, "<")))	{
			int val = atoi(op+strlen("<"));
			if(idx < val)	{ return 1; }
		} else if((op = strstr(ptr, ">")))	{
			int val = atoi(op+strlen(">"));
			if(idx > val)	{ return 1; }
		}
		return 0;
	} else if(strncmp(cond, "position()", strlen("position()")) == 0)	{
		/* other not yet handled */
		return 0;
	}
	return 0;
}

void * roxml_malloc(int size, int num, int type)
{
	memory_cell_t *cell = &head_cell;
	while(cell->next != NULL) { cell = cell->next; }
	cell->next = (memory_cell_t*)malloc(sizeof(memory_cell_t));
	if(!cell->next) { return NULL; }
	cell->next->next = NULL;
	cell->next->prev = cell;
	cell = cell->next;
	cell->type = type;
	cell->id = pthread_self();
	cell->occ = size;
	cell->ptr = malloc(size*num);
	head_cell.prev = cell;
	return cell->ptr;
}

void roxml_release(void * data)
{
	memory_cell_t *ptr = &head_cell;
	memory_cell_t *to_delete = NULL;

	if(data == RELEASE_LAST)	{
		while((ptr->prev != NULL)&&(ptr->prev->id != pthread_self())) { ptr = ptr->prev; } 
		if(ptr->prev == NULL)	{ return; }

		to_delete = ptr->prev;

		if(to_delete->next) { 
			to_delete->prev->next = to_delete->next;
			to_delete->next->prev = to_delete->prev;
		} else {
			if(to_delete->prev != &head_cell)	{
				head_cell.prev = to_delete->prev;
			} else {
				head_cell.prev = NULL;
			}
			to_delete->prev->next = NULL;
		}

		if(PTR_IS_STAR(to_delete))	{
			int i = 0;
			for(i = 0; i < to_delete->occ; i++) { free(((void**)(to_delete->ptr))[i]); }
		}
		if(to_delete->type != PTR_NONE)	{
			free(to_delete->ptr);
			to_delete->type = PTR_NONE;
			free(to_delete);
		}
	} else if(data == RELEASE_ALL) {
		head_cell.prev = NULL;
		while((head_cell.next != NULL)) { 
			to_delete = head_cell.next;
			if(to_delete->next) { to_delete->next->prev = &head_cell; }
			head_cell.next = to_delete->next;

			if(PTR_IS_STAR(to_delete))	{
				int i = 0;
				for(i = 0; i < to_delete->occ; i++) { free(((void**)(to_delete->ptr))[i]); }
			}
			free(to_delete->ptr);
			to_delete->ptr = NULL;
			to_delete->type = PTR_NONE;
			free(to_delete);
		}
	} else	{
		while((ptr->next != NULL)&&(ptr->next->ptr != data)) { ptr = ptr->next; }
		if(ptr->next == NULL)	{ 
			return;
		}

		to_delete = ptr->next;
		if(to_delete->next) {
			to_delete->next->prev = ptr; 
		} else {
			if(ptr == &head_cell)	{
				head_cell.prev = NULL;
			} else {
				head_cell.prev = to_delete->prev;
			}
		}
		ptr->next = to_delete->next;
		if(PTR_IS_STAR(to_delete))	{
			int i = 0;
			for(i = 0; i < to_delete->occ; i++) { free(((void**)(to_delete->ptr))[i]); }
		}
		free(to_delete->ptr);
		to_delete->type = PTR_NONE;
		free(to_delete);
	}
	if(head_cell.next == &head_cell) { head_cell.next = NULL; }
	if(head_cell.prev == &head_cell) { head_cell.prev = NULL; }
}

void roxml_write_node(node_t * n, FILE *f)
{
	char *name = roxml_get_name(n, NULL, 0);
	int nb = roxml_get_chld_nb(n);
	if(nb > 0)	{
		int i = 0;
		fprintf(f, "<%s>", name);
		for(i = 0; i < nb; i++)	{
			roxml_write_node(roxml_get_chld(n, NULL, i), f);
		}
		fprintf(f, "</%s>", name);
	} else {
		fprintf(f, "<%s/>", name);
	}
	roxml_release(name);
}

void roxml_commit_changes(node_t *n)
{
	FILE *fout = fopen("/tmp/out.xml", "w");
	if(fout)	{
		roxml_write_node(n, fout);
		fclose(fout);	
	}
}


#endif /* ROXML_C */

