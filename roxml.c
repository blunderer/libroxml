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

#ifdef __DEBUG
unsigned int _nb_node = 0;
unsigned int _nb_attr = 0;
unsigned int _nb_text = 0;
#endif

memory_cell_t head_cell = {PTR_NONE, 0, NULL, 0, NULL, NULL};

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
	cell->ptr = calloc(num, size);
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

node_t * roxml_parent_node(node_t *parent, node_t *n)
{
	n->prnt = parent;
	if(parent)	{
		if(roxml_get_type(n) == ROXML_ARG)	{
			if(parent->attr) {
				parent->attr->next->sibl = n;
				parent->attr->next = n;
			} else {
				parent->attr = n;
				parent->attr->next = n;
			}
		} else if(roxml_get_type(n) == ROXML_TXT){
			if(parent->text) {
				parent->text->next->sibl = n;
				parent->text->next = n;
			} else {
				parent->text = n;
				parent->text->next = n;
			}
		} else if(roxml_get_type(n) == ROXML_VAL){
			if(parent->chld) {
				parent->next->sibl = n;
				parent->next->next = n;
				parent->next = n;
			} else {
				parent->chld = n;
				parent->next = n;
			}
		}
	}

	return n;
}

node_t * roxml_create_node(int pos, FILE *file, char * buf, unsigned int * idx, int type)
{
	node_t *n = (node_t*)calloc(1, sizeof(node_t));
	n->type = type;
	n->idx = idx;
	if(buf)	{
		n->src.buf = buf;
	} else if(file)	{
		n->src.fil = file;
	}
	n->pos = pos;
	n->end = pos;

	return n;
}

void roxml_close_node(node_t *n, node_t *close)
{
	n->end = close->pos;
	roxml_free_node(close);

	if((n->type & ROXML_VAL) == ROXML_VAL) {
		n->next = NULL;
	}

#ifdef __DEBUG
	if(n->type & ROXML_VAL) _nb_node++;
	if(n->type & ROXML_ARG) _nb_attr++;
	if(n->type & ROXML_TXT) _nb_text++;
#endif

}

void roxml_free_node(node_t *n)
{
	free(n);
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

		total = 0;
		ptr = n->text;
		while(ptr)	{
			if(ptr->type & ROXML_FILE) {
				fseek(ptr->src.fil, ptr->pos, SEEK_SET);
				fread(content+total, ptr->end - ptr->pos, 1, ptr->src.fil);
			} else {
				memcpy(content+total, ptr->src.buf+ptr->pos, ptr->end - ptr->pos);
			}
			total += ptr->end - ptr->pos;
			ptr = ptr->sibl;
		}
	} else if(n->type & ROXML_ARG)	{
		node_t *ptr = n->text;
		total = ptr->end - ptr->pos;
		content = roxml_malloc(sizeof(char), total+1, PTR_CHAR);
		if(content == NULL) { return NULL; }
		if(ptr->type & ROXML_FILE) {
			fseek(ptr->src.fil, ptr->pos, SEEK_SET);
			fread(content, total, 1, ptr->src.fil);
		} else {
			memcpy(content, ptr->src.buf+ptr->pos, total);
		}
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
	char tmp_name[INTERNAL_BUF_SIZE];
	memset(tmp_name, 0, INTERNAL_BUF_SIZE*sizeof(char));

	if(n == NULL)	{
		if(name)	{
			strcpy(name, "");
		}
		return NULL;
	}

	if(n->prnt == NULL)	{
		strcpy(tmp_name, "root");
	} else if(n->type & ROXML_ARG)	{
		int offset = 0;
		char *internal_ptr;
		char internal_buf[INTERNAL_BUF_SIZE];
		if(n->type & ROXML_FILE) {
			fseek(n->src.fil, n->pos, SEEK_SET);
			fread(internal_buf, INTERNAL_BUF_SIZE, 1, n->src.fil);
			internal_ptr = internal_buf;
		} else {
			internal_ptr = n->src.buf + n->pos;
		}
		while(ROXML_WHITE(internal_ptr[offset])) { offset++; }
		while(internal_ptr[offset] != '=') { 
			tmp_name[count] = internal_ptr[offset];
			offset++;
			count++;
		}
	} else if(n->type & ROXML_VAL)	{
		int offset = 0;
		char *internal_ptr;
		char internal_buf[INTERNAL_BUF_SIZE];
		if(n->type & ROXML_FILE) {
			fseek(n->src.fil, n->pos, SEEK_SET);
			fread(internal_buf, INTERNAL_BUF_SIZE, 1, n->src.fil);
			internal_ptr = internal_buf;
		} else {
			internal_ptr = n->src.buf + n->pos;
		}
		while((internal_ptr[offset] == '<')||(ROXML_WHITE(internal_ptr[offset]))) { offset++; }
		while((internal_ptr[offset] != '>')&&(!ROXML_WHITE(internal_ptr[offset]))) { 
			tmp_name[count] = internal_ptr[offset];
			offset++;
			count++;
		}
	} else if(n->type & ROXML_TXT) {
		strcpy(tmp_name, "");
	}

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
		fclose(root->src.fil);
	}
	free(root->idx);
	free(root);
}

void roxml_del_tree(node_t *n)
{
	if(n == NULL)	{ return; }
	roxml_del_tree(n->chld);
	roxml_del_tree(n->text);
	roxml_del_tree(n->sibl);
	roxml_del_tree(n->attr);
	free(n);
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

int roxml_get_type(node_t *n)
{
	return (n->type & (ROXML_ARG | ROXML_VAL | ROXML_TXT));
}

node_t* roxml_new_node(int type, char * name, char * value)
{
	int size, close;
	node_t *node;
	
	size = 2*strlen(name) + strlen(value) + 6;
	close = strlen(name) + strlen(value) + 3;
	node = (node_t*)roxml_malloc(sizeof(node_t), 1, PTR_NODE);
	node->src.buf = (char*)roxml_malloc(sizeof(char), size, PTR_CHAR);
	node->idx = (unsigned int*)roxml_malloc(sizeof(unsigned int), 1, PTR_INT);
	sprintf(node->src.buf,"<%s>%s</%s>",name,value,name);
	node->type = ROXML_BUFF & type;
	node->end = close;

	return node;
}

node_t * roxml_set_prev_next(node_t *candidat, node_t *parent)
{
	node_t *node = parent;		// node5
	candidat->prev = parent;	// node6
	while((node)&&(node->next == NULL))	{
		node->next = candidat;
		node = node->prev;
	}
	return candidat;
}

int roxml_get_node_index(node_t *n, int * last)
{
	int i = 0, idx = 0;
	int nb_sibl = 0, nb_same_sibl = 0;
	char intern_buff[INTERNAL_BUF_SIZE];
	char intern_buff2[INTERNAL_BUF_SIZE];
	char * realn = roxml_get_name(n, intern_buff, INTERNAL_BUF_SIZE);

	nb_sibl = roxml_get_chld_nb(n->prnt);
	for(i = 0; i < nb_sibl; i++)	{
		node_t *tmp = roxml_get_chld(n->prnt, NULL, i);
		char * name = roxml_get_name(tmp, intern_buff2, INTERNAL_BUF_SIZE);
		if(tmp == n)	{ idx = nb_same_sibl; }
		if(strcmp(name, realn) == 0)	{ nb_same_sibl++; }
		roxml_release(name);
	}
	roxml_release(realn);		

	if(last) { *last = nb_same_sibl-1; }

	if(nb_same_sibl > 1)	{ return idx; }
	return -1;
}

node_t * roxml_load_doc(char *filename)
{
	node_t *current_node = NULL;
	FILE* file = fopen(filename, "r");
	if(file == NULL)	{
		return NULL;
	}
	unsigned int *index = (unsigned int *)malloc(sizeof(unsigned int));
	*index = 0;
	current_node = roxml_create_node(0, file, NULL, index, ROXML_VAL | ROXML_FILE);
	current_node = roxml_parent_node(NULL, current_node);
	return roxml_load(current_node,  file, NULL);
}

node_t * roxml_load_buf(char *buffer)
{
	if(buffer == NULL)	{ return NULL; }
	node_t *current_node = NULL;
	unsigned int *index = (unsigned int *)malloc(sizeof(unsigned int));
	*index = 0;
	current_node = roxml_create_node(0, NULL, buffer, index, ROXML_VAL | ROXML_BUFF);
	current_node = roxml_parent_node(NULL, current_node);
	return roxml_load(current_node, NULL, buffer);
}

node_t * roxml_load(node_t *current_node, FILE *file, char *buffer)
{
	char int_buffer[ROXML_BULK_READ];
	int int_buffer_len = 0;
	int int_abs_pos = 0;
	int int_len = 0;
	int state = STATE_NODE_NONE;
	int mode = MODE_COMMENT_NONE;
	int inside_node_state = STATE_INSIDE_ARG_BEG;
	node_t *previous_node = current_node;
	node_t *candidat_node = NULL;
	node_t *candidat_txt = NULL;
	node_t *candidat_arg = NULL;
	node_t *candidat_val = NULL;
	int current_attr_quoted = 1;
	int type;

	if(file)	{ 
		type = ROXML_FILE;
	} else	{
		type = ROXML_BUFF;
		int_buffer_len = strlen(buffer);
	}

	do {
		int int_rel_pos = 0;
		char * int_ptr = int_buffer;

		if(type == ROXML_FILE)	{
			int_len = fread(int_buffer, 1, ROXML_BULK_READ, file);
		} else {
			int_ptr = buffer + int_abs_pos;
			int_len = ((int_abs_pos+ROXML_BULK_READ)<int_buffer_len)?ROXML_BULK_READ:(int_abs_pos+ROXML_BULK_READ);
		}

		for(int_rel_pos = int_abs_pos; int_rel_pos < int_abs_pos+int_len; int_rel_pos++) {
			char c = int_ptr[int_rel_pos-int_abs_pos];

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
				if(mode == MODE_COMMENT_NONE)	{ current_attr_quoted = 0; }
				else { current_attr_quoted = 1; }
				candidat_val = roxml_create_node(int_rel_pos, file, buffer, current_node->idx, ROXML_TXT | type);
				candidat_val = roxml_parent_node(candidat_arg, candidat_val);
				inside_node_state = STATE_INSIDE_VAL;
			}
			if(mode == MODE_COMMENT_NONE)	{
				if(c == '<')	{
					state = STATE_NODE_BEG;
					if(candidat_txt)	{
						node_t * to_be_closed = roxml_create_node(int_rel_pos, file, buffer, current_node->idx, ROXML_TXT | type);
						roxml_close_node(candidat_txt, to_be_closed);
						candidat_txt = NULL;
					}
					candidat_node = roxml_create_node(int_rel_pos, file, buffer, current_node->idx, ROXML_VAL | type);
				} else if(c == '>')	{
					if(state == STATE_NODE_NAME)	{
						state = STATE_NODE_CONTENT;
						current_node = roxml_parent_node(current_node, candidat_node);
						candidat_txt = roxml_create_node(int_rel_pos+1, file, buffer, current_node->idx, ROXML_TXT | type);
						candidat_txt = roxml_parent_node(current_node, candidat_txt);
					} else if(state == STATE_NODE_ATTR)	{
						if(inside_node_state == STATE_INSIDE_VAL)	{
							int off = current_attr_quoted;
							node_t * to_be_closed = roxml_create_node(int_rel_pos-off, file, buffer, current_node->idx, ROXML_ARG | type);
							roxml_close_node(candidat_val, to_be_closed);
						}
						current_node = roxml_parent_node(current_node, candidat_node);
						state = STATE_NODE_CONTENT;
						inside_node_state = STATE_INSIDE_ARG_BEG;
						candidat_txt = roxml_create_node(int_rel_pos+1, file, buffer, current_node->idx, ROXML_TXT | type);
						candidat_txt = roxml_parent_node(current_node, candidat_txt);
					} else if(state == STATE_NODE_SINGLE)	{
						current_node = roxml_parent_node(current_node, candidat_node);
						if(current_node->prnt != NULL) { current_node = current_node->prnt; }
						candidat_txt = roxml_create_node(int_rel_pos+1, file, buffer, current_node->idx, ROXML_TXT | type);
						candidat_txt = roxml_parent_node(current_node, candidat_txt);
					} else if(state == STATE_NODE_END)	{
						roxml_close_node(current_node, candidat_node);
						if(current_node->prnt != NULL) { current_node = current_node->prnt; }
						candidat_txt = roxml_create_node(int_rel_pos+1, file, buffer, current_node->idx, ROXML_TXT | type);
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
							node_t * to_be_closed = roxml_create_node(int_rel_pos-off, file, buffer, current_node->idx, ROXML_ARG | type);
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
							node_t * to_be_closed = roxml_create_node(int_rel_pos-off, file, buffer, current_node->idx, ROXML_ARG | type);
							roxml_close_node(candidat_val, to_be_closed);
							inside_node_state = STATE_INSIDE_ARG_BEG;
						}
					}
				} else	{
					if(state == STATE_NODE_BEG) {
						state = STATE_NODE_NAME;
						previous_node = roxml_set_prev_next(candidat_node, previous_node);
					}
					if(state == STATE_NODE_ATTR)	{
						if(inside_node_state == STATE_INSIDE_ARG_BEG)	{
							candidat_arg = roxml_create_node(int_rel_pos-1, file, buffer, current_node->idx, ROXML_ARG | type);
							candidat_arg = roxml_parent_node(candidat_node, candidat_arg);
							inside_node_state = STATE_INSIDE_ARG;
						} else if((inside_node_state == STATE_INSIDE_ARG)&&(c == '='))	{
							inside_node_state = STATE_INSIDE_VAL_BEG;
							node_t * to_be_closed = roxml_create_node(int_rel_pos, file, buffer, current_node->idx, ROXML_ARG | type);
							roxml_close_node(candidat_arg, to_be_closed);
						}
					}
				}
			}
		}
		int_abs_pos += int_len;
		*current_node->idx += int_len;
	} while(int_len == ROXML_BULK_READ);

	while(current_node->prnt)	{ current_node = current_node->prnt; }
	return current_node;
}

/** \comment xpath is something like "/node[first() or last()]/item/title/@version || /node/item/title[@version < 3]" */
int roxml_parse_xpath(char *path, xpath_node_t ** xpath)
{
	int cur;
	int i = 0;
	int nbpath = 1;
	int bracket = 0;
	int parenthesys = 0;
	int quoted = 0;
	int dquoted = 0;
	int len = strlen(path);

	xpath_node_t * first_node = (xpath_node_t*)calloc(1, sizeof(xpath_node_t));
	xpath_node_t * new_node = first_node;
	xpath_cond_t * new_cond = NULL;

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
				path[cur] = '\0';

				if(bracket)	{
					xpath_cond_t * tmp_cond = (xpath_cond_t*)calloc(1, sizeof(xpath_cond_t));
					new_node->cond = tmp_cond;
					new_cond = tmp_cond;
					new_cond->arg1 = path+cur+1;
				}
			}
		} else if(path[cur] == '/') {
			if(!quoted && !dquoted && !parenthesys && !bracket)	{
				path[cur] = '\0';
				i = 0;
				xpath_node_t * tmp_node = (xpath_node_t*)calloc(1, sizeof(xpath_node_t));
				if(new_node)	{ new_node->next = tmp_node; }
				new_node = tmp_node;
				new_node->axis = path+cur+1;
			}
		} else	{
			if(!bracket && !quoted && !dquoted) {
				if(strncmp(path+cur, ROXML_PATH_OR, strlen(ROXML_PATH_OR)) == 0)	{
					// we are in the middle of a multiple xpath
					path[cur-1] = '\0';
					cur += strlen(ROXML_PATH_OR)-1;
					xpath_node_t * tmp_node = (xpath_node_t*)calloc(nbpath+1, sizeof(xpath_node_t));
					memcpy(tmp_node, first_node, nbpath*sizeof(xpath_node_t));
					free(first_node);
					first_node = tmp_node;
					new_node = tmp_node+nbpath;
					new_node->rel = ROXML_OPERATOR_OR;
					nbpath++;
				} else if(strncmp(path+cur, ROXML_PATH_AND, strlen(ROXML_PATH_AND)) == 0)	{
					// we are in the middle of a multiple xpath
					path[cur-1] = '\0';
					cur += strlen(ROXML_PATH_AND)-1;
					xpath_node_t * tmp_node = (xpath_node_t*)calloc(nbpath+1, sizeof(xpath_node_t));
					memcpy(tmp_node, first_node, nbpath*sizeof(xpath_node_t));
					free(first_node);
					first_node = tmp_node;
					new_node = tmp_node+nbpath;
					new_node->rel = ROXML_OPERATOR_AND;
					nbpath++;
				} 
			} else if(bracket && !quoted && !dquoted) {
				if(strncmp(path+cur, ROXML_COND_OR, strlen(ROXML_COND_OR)) == 0)	{
					// we are in the middle of a splitted condition
					i = 0;
					path[cur-1] = '\0';
					cur += strlen(ROXML_COND_OR);
					xpath_cond_t * tmp_cond = (xpath_cond_t*)calloc(1, sizeof(xpath_cond_t));
					if(new_cond) { new_cond->next = tmp_cond; }
					new_cond = tmp_cond;
					new_cond->rel = ROXML_OPERATOR_OR;
					new_cond->arg1 = path+cur+1;
				} else if(strncmp(path+cur, ROXML_COND_AND, strlen(ROXML_COND_AND)) == 0)	{
					// we are in the middle of a splitted condition
					i = 0;
					path[cur-1] = '\0';
					cur += strlen(ROXML_COND_AND);
					xpath_cond_t * tmp_cond = (xpath_cond_t*)calloc(1, sizeof(xpath_cond_t));
					if(new_cond) { new_cond->next = tmp_cond; }
					new_cond = tmp_cond;
					new_cond->rel = ROXML_OPERATOR_AND;
					new_cond->arg1 = path+cur+1;
				}
				if(strncmp(path+cur, ROXML_FUNC_POS_STR, strlen(ROXML_FUNC_POS_STR)) == 0) {
					cur += strlen(ROXML_FUNC_POS_STR)-1;
					new_cond->func = ROXML_FUNC_POS;
				} else if(strncmp(path+cur, ROXML_FUNC_FIRST_STR, strlen(ROXML_FUNC_FIRST_STR)) == 0) {
					cur += strlen(ROXML_FUNC_FIRST_STR)-1;
					new_cond->func = ROXML_FUNC_FIRST;
				} else if(strncmp(path+cur, ROXML_FUNC_LAST_STR, strlen(ROXML_FUNC_LAST_STR)) == 0) {
					cur += strlen(ROXML_FUNC_LAST_STR)-1;
					new_cond->func = ROXML_FUNC_LAST;
				} else if(path[cur] == '=') {
					new_cond->op = ROXML_OPERATOR_EQU;
					path[cur] = '\0';
					if(ROXML_WHITE(path[cur-1])) {
						path[cur-1] = '\0';
					}
					if(ROXML_WHITE(path[cur+1])) {
						cur++;
						path[cur] = '\0';
					}
					new_cond->arg2 = path+cur+1;
				} else if(path[cur] == '>') {
					path[cur] = '\0';
					new_cond->op = ROXML_OPERATOR_SUP;
					if(ROXML_WHITE(path[cur-1])) {
						path[cur-1] = '\0';
					}
					if(path[cur+1] == '=') { 
						cur++;
						path[cur] = '\0';
						new_cond->op = ROXML_OPERATOR_ESUP; 
					}
					if(ROXML_WHITE(path[cur+1])) {
						cur++;
						path[cur] = '\0';
					}
					new_cond->arg2 = path+cur+1;
				} else if(path[cur] == '<') {
					path[cur] = '\0';
					new_cond->op = ROXML_OPERATOR_INF;
					if(ROXML_WHITE(path[cur-1])) {
						path[cur-1] = '\0';
					}
					if(path[cur+1] == '=') { 
						cur++;
						path[cur] = '\0';
						new_cond->op = ROXML_OPERATOR_EINF; 
					}
					if(ROXML_WHITE(path[cur+1])) {
						cur++;
						path[cur] = '\0';
					}
					new_cond->arg2 = path+cur+1;
				} else if(path[cur] == '!') {
					path[cur] = '\0';
					new_cond->op = ROXML_OPERATOR_DIFF;
					path[cur] = '\0';
					if(ROXML_WHITE(path[cur-1])) {
						path[cur-1] = '\0';
					}
					if(ROXML_WHITE(path[cur+1])) {
						cur++;
						path[cur] = '\0';
					}
					new_cond->arg2 = path+cur+1;
				}
			}
		}
	}

	if(xpath)	{
		*xpath = first_node;
	}
	return nbpath;
}

void roxml_free_xcond(xpath_cond_t *xcond)
{
	if(xcond->next) {
		roxml_free_xcond(xcond->next);
	}
	free(xcond);
}

void roxml_free_xpath(xpath_node_t *xpath, int nb)
{
	int i = 0;
	for(i = 0; i < nb; i++)	{
		if(xpath[i].next)	{
			roxml_free_xpath(xpath[i].next, 1);
		}
		if(xpath[i].cond)	{
			roxml_free_xcond(xpath[i].cond);
		}
	}
	free(xpath);
}

int roxml_int_cmp(int a, int b, int op)
{
	if(op == ROXML_OPERATOR_DIFF) {
		return (a!=b);
	} else if(op == ROXML_OPERATOR_EINF) {
		return (a<=b);
	} else if(op == ROXML_OPERATOR_INF) {
		return (a<b);
	} else if(op == ROXML_OPERATOR_ESUP) {
		return (a>=b);
	} else if(op == ROXML_OPERATOR_SUP) {
		return (a>b);
	} else if(op == ROXML_OPERATOR_EQU) {
		return (a==b);
	}
	return 0;
}

int roxml_validate_predicat(xpath_node_t *xn, node_t *candidat)
{
	int first = 1;
	int valid = 0;
	xpath_cond_t *condition = xn->cond;

	if(!condition)	{
		return 1;
	}

	while(condition) {
		int status = 0;
		int iarg1;
		int iarg2;
		char * sarg1;
		char * sarg2;

		if(condition->func == ROXML_FUNC_POS) {
			iarg1 = roxml_get_node_index(candidat, NULL);
			iarg2 = atoi(condition->arg2);
			status = roxml_int_cmp(iarg1, iarg2, condition->op);
		} else if(condition->func == ROXML_FUNC_LAST) {
			iarg1 = roxml_get_node_index(candidat, &iarg2);
			status = roxml_int_cmp(iarg1, iarg2, ROXML_OPERATOR_EQU);
		} else if(condition->func == ROXML_FUNC_FIRST) {
			iarg1 = roxml_get_node_index(candidat, NULL);
			status = roxml_int_cmp(iarg1, 0, ROXML_OPERATOR_EQU);
		} else if(condition->func == ROXML_FUNC_INTCOMP) {
			iarg1 = 0; /* atoi(process arg1); */
			iarg2 = atoi(condition->arg2);
			status = roxml_int_cmp(iarg1, iarg2, condition->op);
		} else if(condition->func == ROXML_FUNC_STRCOMP) {
			sarg1 = NULL; /* process arg1 */;
			sarg2 = condition->arg2;
			status = (strcmp(sarg1, sarg2)==0);
		}

		if(first) {
			valid = status;
			first = 0;
		} else {
			if(condition->rel == ROXML_OPERATOR_OR) {
				valid = valid || status;
			} else if(condition->rel == ROXML_OPERATOR_AND) {
				valid = valid && status;
			}
		}
		condition = condition->next;
	}

	return valid;
}

int roxml_validate_axes(node_t *candidat, char *axes, node_t ***ans, int *nb, int *max, xpath_node_t *xn)
{
	int valid = 0;
	char intern_buff[INTERNAL_BUF_SIZE];
	char axe[INTERNAL_BUF_SIZE];

	strncpy(axe, axes, INTERNAL_BUF_SIZE);

	if(strcmp("*", axes) == 0)  {
		valid = 1;
	}

	if(!valid) {
		char * name  = roxml_get_name(candidat, intern_buff, INTERNAL_BUF_SIZE);
		if(strcmp(name, axes) == 0)	{
			valid = 1;
		}	
		roxml_release(name);
	}

	if(valid)	{
		valid = roxml_validate_predicat(xn, candidat);
	}

	if((valid)&&(xn->next==NULL)) {
		printf("id %lu\n",candidat->end);
		if((*nb) >= (*max))	{
			int new_max = (*max)+10;
			node_t ** new_ans = roxml_malloc(sizeof(node_t*), new_max, PTR_NODE_RESULT);
			memcpy(new_ans, (*ans), *(max)*sizeof(node_t*)); 
			roxml_release(*ans);
			*ans = new_ans;
			*max = new_max;
		}
		(*ans)[*nb] = candidat;
		(*nb)++;
	}


	return valid;
}

void roxml_check_node(xpath_node_t *xp, node_t *context, node_t ***ans, int *nb, int *max, int ignore)
{
	if(!xp)	{ return; }

	// tests axes
	if((strncmp(ROXML_L_SELF, xp->axis, strlen(ROXML_L_SELF))==0)||(strncmp(ROXML_S_SELF, xp->axis, strlen(ROXML_S_SELF))==0)) {
		// ROXML_L_SELF & ROXML_S_SELF
		roxml_validate_axes(context, "*", ans, nb, max, xp);
		roxml_check_node(xp->next, context, ans, nb, max, ignore);
	} else if((strncmp(ROXML_L_PARENT, xp->axis, strlen(ROXML_L_PARENT))==0)||(strncmp(ROXML_S_SELF, xp->axis, strlen(ROXML_S_SELF))==0)) {
		// ROXML_L_PARENT & ROXML_S_SELF
		roxml_validate_axes(context->prnt, "*", ans, nb, max, xp);
		roxml_check_node(xp->next, context->prnt, ans, nb, max, ignore);
	} else if(strncmp(ROXML_L_DESC_O_SELF, xp->axis, strlen(ROXML_L_DESC_O_SELF))==0) {
		// ROXML_L_DESC_O_SELF
		roxml_check_node(xp->next, context, ans, nb, max, ROXML_SELF_AND_DESC);
	} else if(strncmp(ROXML_L_DESC, xp->axis, strlen(ROXML_L_DESC))==0) {
		// ROXML_L_DESC
		roxml_check_node(xp->next, context, ans, nb, max, ROXML_DESC_ONLY);
	} else if(strlen(xp->axis)==0) {
		// ROXML_S_DESC_O_SELF
		roxml_check_node(xp->next, context, ans, nb, max, ROXML_SELF_AND_DESC);
	} else if(strncmp(ROXML_L_ATTR, xp->axis, strlen(ROXML_L_ATTR))==0) {
		// ROXML_L_ATTR
		node_t *attribute = context->attr;
		while(attribute)  {
			if(roxml_validate_axes(attribute, xp->axis+strlen(ROXML_L_ATTR), ans, nb, max, xp))	{
				roxml_check_node(xp->next, context, ans, nb, max, ignore);
			}
			attribute = attribute->sibl;
		}
	} else if(strncmp(ROXML_S_ATTR, xp->axis, strlen(ROXML_S_ATTR))==0) {
		// ROXML_S_ATTR
		node_t *attribute = context->attr;
		while(attribute)  {
			if(roxml_validate_axes(attribute, xp->axis+strlen(ROXML_S_ATTR), ans, nb, max, xp))	{
				roxml_check_node(xp->next, context, ans, nb, max, ignore);
			}
			attribute = attribute->sibl;
		}
	} else if(strncmp(ROXML_L_ANC, xp->axis, strlen(ROXML_L_ANC))==0) {
		// ROXML_L_ANC
		node_t *current = context->prnt;
		while(current)	{
			if(roxml_validate_axes(current, xp->axis+strlen(ROXML_L_ANC), ans, nb, max, xp))	{
				roxml_check_node(xp->next, current, ans, nb, max, ROXML_DIRECT);
			}
			current = current->prnt;
		}
	} else if(strncmp(ROXML_L_ANC_O_SELF, xp->axis, strlen(ROXML_L_ANC_O_SELF))==0) {
		// ROXML_L_ANC_O_SELF
		node_t *current = context;
		while(current)	{
			if(roxml_validate_axes(current, xp->axis+strlen(ROXML_L_ANC_O_SELF), ans, nb, max, xp))	{
				roxml_check_node(xp->next, current, ans, nb, max, ROXML_DIRECT);
			}
			current = current->prnt;
		}
	} else if(strncmp(ROXML_L_NEXT_SIBL, xp->axis, strlen(ROXML_L_NEXT_SIBL))==0) {
		// ROXML_L_NEXT_SIBL
		node_t *current = context->sibl;
		while(current)	{
			if(roxml_validate_axes(current, xp->axis+strlen(ROXML_L_NEXT_SIBL), ans, nb, max, xp))	{
				roxml_check_node(xp->next, current, ans, nb, max, ROXML_DIRECT);
			}
			current = current->sibl;
		}
	} else if(strncmp(ROXML_L_PREV_SIBL, xp->axis, strlen(ROXML_L_PREV_SIBL))==0) {
		// ROXML_L_PREV_SIBL
		node_t *current = context->prnt->chld;
		while(current != context)	{
			if(roxml_validate_axes(current, xp->axis+strlen(ROXML_L_PREV_SIBL), ans, nb, max, xp))	{
				roxml_check_node(xp->next, current, ans, nb, max, ROXML_DIRECT);
			}
			current = current->sibl;
		}
	} else if(strncmp(ROXML_L_NEXT, xp->axis, strlen(ROXML_L_NEXT))==0) {
		// ROXML_L_NEXT
		node_t *current = context->next;
		while(current)  {
			if(roxml_validate_axes(current, xp->axis+strlen(ROXML_L_NEXT), ans, nb, max, xp))	{
				roxml_check_node(xp->next, context, ans, nb, max, ignore);
			}
			current = current->next;
		}
	} else if(strncmp(ROXML_L_PREV, xp->axis, strlen(ROXML_L_PREV))==0) {
		// ROXML_L_PREV
		node_t *current = context->prev;
		while(current)  {
			if(roxml_validate_axes(current, xp->axis+strlen(ROXML_L_PREV), ans, nb, max, xp))	{
				roxml_check_node(xp->next, context, ans, nb, max, ignore);
			}
			current = current->prev;
		}
	} else if(strncmp(ROXML_L_NS, xp->axis, strlen(ROXML_L_NS))==0) {
		// ROXML_L_NS is not handled
	} else if(strncmp(ROXML_L_CHILD, xp->axis, strlen(ROXML_L_CHILD))==0) {
		// ROXML_L_CHILD
		node_t *current = context->chld;
		while(current)	{
			if(roxml_validate_axes(current, xp->axis+strlen(ROXML_L_CHILD), ans, nb, max, xp))	{
				roxml_check_node(xp->next, current, ans, nb, max, ROXML_DIRECT);
			}
			current = current->sibl;
		}
	} else {
		// ROXML_S_CHILD
		node_t *current = context->chld;
		while(current)	{
			if(roxml_validate_axes(current, xp->axis, ans, nb, max, xp))	{
				roxml_check_node(xp->next, current, ans, nb, max, ROXML_DIRECT);
			}
			current = current->sibl;
		}
	}

	// if found a "all document" axes
	if(ignore)	{
		node_t *current = context->chld;
		while(current)	{
			roxml_check_node(xp, current, ans, nb, max, ROXML_DESC_ONLY);
			current = current->sibl;
		}
		if(ignore == ROXML_SELF_AND_DESC)	{
			roxml_validate_axes(context, xp->axis, ans, nb, max, xp);
		}
	}

	return;
}

node_t ** roxml_xpath(node_t *n, char * path, int *nb_ans)
{
	int path_id;
	int index = 0;
	int ansnb = 0;
	int ansmax = 1;
	xpath_node_t *xpath = NULL;

	if(n == NULL)	{ 
		if(nb_ans) { *nb_ans = 0; }
		return NULL; 
	}

	node_t **ans = roxml_malloc(sizeof(node_t*), ansmax, PTR_NODE_RESULT);
	char * full_path_to_find = strdup(path);
	char * path_to_find = full_path_to_find;

	index = roxml_parse_xpath(path_to_find, &xpath);

	for(path_id = 0; path_id < index; path_id++)	{
		node_t *orig = n;
		if(xpath[path_id].axis == NULL)	{
			if(xpath[path_id].next && !xpath[path_id].next->axis)	{
				// context node is root
				while(orig->prnt) { orig = orig->prnt; }
			}
		}
		roxml_check_node(&xpath[path_id], orig, &ans, &ansnb, &ansmax, ROXML_DIRECT);
	}

	roxml_free_xpath(xpath, 0);

	free(full_path_to_find);

	if(nb_ans)	{
		*nb_ans = ansnb;
	}

	if(ansnb == 0)	{
		roxml_release(ans);
		return NULL;
	}

	return ans;
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

