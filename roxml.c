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
		if(roxml_get_type(n) == ROXML_ATTR_NODE)	{
			if(parent->attr) {
				parent->attr->next->sibl = n;
				parent->attr->next = n;
			} else {
				parent->attr = n;
				parent->attr->next = n;
			}
		} else if(roxml_get_type(n) == ROXML_TXT_NODE){
			if(parent->text) {
				parent->text->next->sibl = n;
				parent->text->next = n;
			} else {
				parent->text = n;
				parent->text->next = n;
			}
		} else if(roxml_get_type(n) == ROXML_STD_NODE){
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

node_t * roxml_create_node(int pos, FILE *file, char * buf, int type)
{
	node_t *n = (node_t*)calloc(1, sizeof(node_t));
	n->type = type;
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

	if((n->type & ROXML_STD_NODE) == ROXML_STD_NODE) {
		n->next = NULL;
	}

#ifdef __DEBUG
	if(n->type & ROXML_STD_NODE) _nb_node++;
	if(n->type & ROXML_ATTR_NODE) _nb_attr++;
	if(n->type & ROXML_TXT_NODE) _nb_text++;
#endif

}

void roxml_free_node(node_t *n)
{
	if(n->type & ROXML_PENDING) {
		if(n->pos == 0) {
			free(n->src.buf);
		}
	}
	free(n);
}

char * roxml_get_content(node_t *n, char * name, int bufsize, int *size)
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

	if(n->type & ROXML_STD_NODE)	{
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
	} else if(n->type & ROXML_TXT_NODE)	{
		total = n->end - n->pos;
		content = roxml_malloc(sizeof(char), total+1, PTR_CHAR);
		if(content == NULL) { return NULL; }
		if(n->type & ROXML_FILE) {
			fseek(n->src.fil, n->pos, SEEK_SET);
			fread(content, n->end - n->pos, 1, n->src.fil);
		} else {
			memcpy(content, n->src.buf+n->pos, n->end - n->pos);
		}
	} else if(n->type & ROXML_ATTR_NODE)	{
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
		strncpy(name, content, bufsize);
	}
	if(size) {
		*size = total;
	}
	return content;
}

char * roxml_get_name(node_t *n, char * name, int size)
{
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
	} else if(n->type & ROXML_ATTR_NODE)	{
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
	} else if(n->type & ROXML_STD_NODE)	{
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
	} else if(n->type & ROXML_TXT_NODE) {
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

int ROXML_API roxml_get_text_nb(node_t *n)
{
	node_t *ptr = n;
	int nb = 0;
	if(ptr->text)	{
		ptr = ptr->text;
		nb++;
		while(ptr->sibl)	{
			nb++;
			ptr = ptr->sibl;
		}
	}
	return nb;
}

node_t *roxml_get_text(node_t *n, int nb)
{
	int count = 0;
	node_t *ptr = n;
	ptr = n->text;
	
	if(ptr == NULL)	{
		return NULL;
	}

	if(nb == 0)	{
		return ptr;
	}
	while((ptr->sibl)&&(nb > count)) {
		count++;
		ptr = ptr->sibl;
	}
	if(nb > count)	{ return NULL; }
	return ptr;
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
	roxml_free_node(root);
}

void roxml_del_tree(node_t *n)
{
	if(n == NULL)	{ return; }
	roxml_del_tree(n->chld);
	roxml_del_tree(n->text);
	roxml_del_tree(n->sibl);
	roxml_del_tree(n->attr);
	roxml_free_node(n);
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
	return (n->type & (ROXML_ATTR_NODE | ROXML_STD_NODE | ROXML_TXT_NODE));
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
	current_node = roxml_create_node(0, file, NULL, ROXML_STD_NODE | ROXML_FILE);
	current_node = roxml_parent_node(NULL, current_node);
	return roxml_load(current_node,  file, NULL);
}

node_t * roxml_load_buf(char *buffer)
{
	if(buffer == NULL)	{ return NULL; }
	node_t *current_node = NULL;
	current_node = roxml_create_node(0, NULL, buffer, ROXML_STD_NODE | ROXML_BUFF);
	current_node = roxml_parent_node(NULL, current_node);
	return roxml_load(current_node, NULL, buffer);
}

node_t * roxml_load(node_t *current_node, FILE *file, char *buffer)
{
	char int_buffer[ROXML_BULK_READ];
	int int_buffer_len = 0;
	int int_abs_pos = 0;
	int int_len = 0;
	int empty_text_node = 1;
	int state = STATE_NODE_NONE;
	int mode = MODE_COMMENT_NONE;
	int inside_node_state = STATE_INSIDE_ARG_BEG;
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
			int_len = ROXML_BULK_READ>strlen(int_ptr)?strlen(int_ptr):ROXML_BULK_READ;
		}

		for(int_rel_pos = int_abs_pos; int_rel_pos < int_abs_pos+int_len; int_rel_pos++) {
			char c = int_ptr[int_rel_pos-int_abs_pos];

#ifdef IGNORE_EMPTY_TEXT_NODES
			if(state == STATE_NODE_CONTENT)	{
				if(!ROXML_WHITE(c) && (c != '<')) {
					empty_text_node = 0;
				}
			}
#endif /* IGNORE_EMPTY_TEXT_NODES */
			if((state == STATE_NODE_ATTR)&&(inside_node_state == STATE_INSIDE_VAL_BEG))	{
				if(mode == MODE_COMMENT_NONE)	{ current_attr_quoted = 0; }
				else { current_attr_quoted = 1; }
				candidat_val = roxml_create_node(int_rel_pos, file, buffer, ROXML_TXT_NODE | type);
				candidat_val = roxml_parent_node(candidat_arg, candidat_val);
				inside_node_state = STATE_INSIDE_VAL;
			}
			if(mode == MODE_COMMENT_NONE)	{
				if(c == '<')	{
					state = STATE_NODE_BEG;
					if(candidat_txt)	{
#ifdef IGNORE_EMPTY_TEXT_NODES
						if(empty_text_node == 0) {
#endif /* IGNORE_EMPTY_TEXT_NODES */
							node_t * to_be_closed = roxml_create_node(int_rel_pos, file, buffer, ROXML_TXT_NODE | type);
							roxml_close_node(candidat_txt, to_be_closed);
#ifdef IGNORE_EMPTY_TEXT_NODES
						} else {
							roxml_del_node(candidat_txt);
						}
#endif /* IGNORE_EMPTY_TEXT_NODES */
						candidat_txt = NULL;
					}
					candidat_node = roxml_create_node(int_rel_pos, file, buffer, ROXML_STD_NODE | type);
				} else if(c == '>')	{
					if(state == STATE_NODE_NAME)	{
						empty_text_node = 1;
						state = STATE_NODE_CONTENT;
						current_node = roxml_parent_node(current_node, candidat_node);
						candidat_txt = roxml_create_node(int_rel_pos+1, file, buffer, ROXML_TXT_NODE | type);
						candidat_txt = roxml_parent_node(current_node, candidat_txt);
					} else if(state == STATE_NODE_ATTR)	{
						if(inside_node_state == STATE_INSIDE_VAL)	{
							int off = current_attr_quoted;
							node_t * to_be_closed = roxml_create_node(int_rel_pos-off, file, buffer, ROXML_ATTR_NODE | type);
							roxml_close_node(candidat_val, to_be_closed);
						}
						current_node = roxml_parent_node(current_node, candidat_node);
						state = STATE_NODE_CONTENT;
						inside_node_state = STATE_INSIDE_ARG_BEG;
						candidat_txt = roxml_create_node(int_rel_pos+1, file, buffer, ROXML_TXT_NODE | type);
						candidat_txt = roxml_parent_node(current_node, candidat_txt);
					} else if(state == STATE_NODE_SINGLE)	{
						empty_text_node = 1;
						state = STATE_NODE_CONTENT;
						current_node = roxml_parent_node(current_node, candidat_node);
						if(current_node->prnt != NULL) { current_node = current_node->prnt; }
						candidat_txt = roxml_create_node(int_rel_pos+1, file, buffer, ROXML_TXT_NODE | type);
						candidat_txt = roxml_parent_node(current_node, candidat_txt);
					} else if(state == STATE_NODE_END)	{
						empty_text_node = 1;
						state = STATE_NODE_CONTENT;
						roxml_close_node(current_node, candidat_node);
						if(current_node->prnt != NULL) { current_node = current_node->prnt; }
						candidat_txt = roxml_create_node(int_rel_pos+1, file, buffer, ROXML_TXT_NODE | type);
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
							node_t * to_be_closed = roxml_create_node(int_rel_pos-off, file, buffer, ROXML_ATTR_NODE | type);
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
							node_t * to_be_closed = roxml_create_node(int_rel_pos-off, file, buffer, ROXML_ATTR_NODE | type);
							roxml_close_node(candidat_val, to_be_closed);
							inside_node_state = STATE_INSIDE_ARG_BEG;
						}
					}
				} else	{
					if(state == STATE_NODE_BEG) {
						state = STATE_NODE_NAME;
					}
					if(state == STATE_NODE_ATTR)	{
						if(inside_node_state == STATE_INSIDE_ARG_BEG)	{
							candidat_arg = roxml_create_node(int_rel_pos-1, file, buffer, ROXML_ATTR_NODE | type);
							candidat_arg = roxml_parent_node(candidat_node, candidat_arg);
							inside_node_state = STATE_INSIDE_ARG;
						} else if((inside_node_state == STATE_INSIDE_ARG)&&(c == '='))	{
							inside_node_state = STATE_INSIDE_VAL_BEG;
							node_t * to_be_closed = roxml_create_node(int_rel_pos, file, buffer, ROXML_ATTR_NODE | type);
							roxml_close_node(candidat_arg, to_be_closed);
						}
					}
				}
			}
		}
		int_abs_pos += int_len;
	} while(int_len == ROXML_BULK_READ);

#ifdef IGNORE_EMPTY_TEXT_NODES
	if(empty_text_node == 1) {
		roxml_del_node(candidat_txt);
	}
#endif /* IGNORE_EMPTY_TEXT_NODES */

	while(current_node->prnt)	{ current_node = current_node->prnt; }
	return current_node;
}

xpath_node_t * roxml_set_axis(xpath_node_t *node, char *axis, int *offset)
{
	xpath_node_t *tmp_node;
	if(axis[0] == '/') { 
		axis[0] = '\0'; 
		*offset += 1;
		axis++;
	}
	if(axis[0] == '/') {
		// ROXML_S_DESC_O_SELF
		node->axis = ROXML_ID_DESC_O_SELF;
		node->name = axis+1;
		tmp_node = (xpath_node_t*)calloc(1, sizeof(xpath_node_t));
		tmp_node->axis = ROXML_ID_CHILD;
		node->next = tmp_node;
		if(strlen(node->name) > 0) {
			tmp_node = (xpath_node_t*)calloc(1, sizeof(xpath_node_t));
			node->next->next = tmp_node;
			node = roxml_set_axis(tmp_node, axis+1, offset);
		}
	} else if(strncmp(ROXML_L_DESC_O_SELF, axis, strlen(ROXML_L_DESC_O_SELF))==0) {
		// ROXML_L_DESC_O_SELF
		node->axis = ROXML_ID_DESC_O_SELF;
		node->name = axis+strlen(ROXML_L_DESC_O_SELF);
		*offset += strlen(ROXML_L_DESC_O_SELF);
		tmp_node = (xpath_node_t*)calloc(1, sizeof(xpath_node_t));
		tmp_node->axis = ROXML_ID_CHILD;
		node->next = tmp_node;
		node = roxml_set_axis(tmp_node, axis+strlen(ROXML_L_DESC_O_SELF), offset);
	} else if(strncmp(ROXML_L_DESC, axis, strlen(ROXML_L_DESC))==0) {
		// ROXML_L_DESC
		node->axis = ROXML_ID_DESC;
		node->name = axis+strlen(ROXML_L_DESC);
		*offset += strlen(ROXML_L_DESC);
		tmp_node = (xpath_node_t*)calloc(1, sizeof(xpath_node_t));
		tmp_node->axis = ROXML_ID_CHILD;
		node->next = tmp_node;
		node = roxml_set_axis(tmp_node, axis+strlen(ROXML_L_DESC), offset);
	} else if(strncmp(ROXML_L_SELF, axis, strlen(ROXML_L_SELF))==0) {
		// ROXML_L_SELF
		node->axis = ROXML_ID_SELF;
		node->name = axis+strlen(ROXML_L_SELF);
	} else if(strncmp(ROXML_S_SELF, axis, strlen(ROXML_S_SELF))==0) {
		// ROXML_S_SELF
		node->axis = ROXML_ID_SELF;
		node->name = axis+strlen(ROXML_S_SELF);
	} else if(strncmp(ROXML_L_PARENT, axis, strlen(ROXML_L_PARENT))==0) {
		// ROXML_L_PARENT
		node->axis = ROXML_ID_PARENT;
		node->name = axis+strlen(ROXML_L_PARENT);
	} else if(strncmp(ROXML_S_PARENT, axis, strlen(ROXML_S_PARENT))==0) {
		// ROXML_S_PARENT
		node->axis = ROXML_ID_PARENT;
		node->name = axis+strlen(ROXML_S_PARENT);
	} else if(strncmp(ROXML_L_ATTR, axis, strlen(ROXML_L_ATTR))==0) {
		// ROXML_L_ATTR
		node->axis = ROXML_ID_ATTR;
		node->name = axis+strlen(ROXML_L_ATTR);
	} else if(strncmp(ROXML_S_ATTR, axis, strlen(ROXML_S_ATTR))==0) {
		// ROXML_S_ATTR
		node->axis = ROXML_ID_ATTR;
		node->name = axis+strlen(ROXML_S_ATTR);
	} else if(strncmp(ROXML_L_ANC, axis, strlen(ROXML_L_ANC))==0) {
		// ROXML_L_ANC
		node->axis = ROXML_ID_ANC;
		node->name = axis+strlen(ROXML_L_ANC);
	} else if(strncmp(ROXML_L_ANC_O_SELF, axis, strlen(ROXML_L_ANC_O_SELF))==0) {
		// ROXML_L_ANC_O_SELF
		node->axis = ROXML_ID_ANC_O_SELF;
		node->name = axis+strlen(ROXML_L_ANC_O_SELF);
	} else if(strncmp(ROXML_L_NEXT_SIBL, axis, strlen(ROXML_L_NEXT_SIBL))==0) {
		// ROXML_L_NEXT_SIBL
		node->axis = ROXML_ID_NEXT_SIBL;
		node->name = axis+strlen(ROXML_L_NEXT_SIBL);
	} else if(strncmp(ROXML_L_PREV_SIBL, axis, strlen(ROXML_L_PREV_SIBL))==0) {
		// ROXML_L_PREV_SIBL
		node->axis = ROXML_ID_PREV_SIBL;
		node->name = axis+strlen(ROXML_L_PREV_SIBL);
	} else if(strncmp(ROXML_L_NEXT, axis, strlen(ROXML_L_NEXT))==0) {
		// ROXML_L_NEXT
		node->axis = ROXML_ID_NEXT;
		node->name = axis+strlen(ROXML_L_NEXT);
	} else if(strncmp(ROXML_L_PREV, axis, strlen(ROXML_L_PREV))==0) {
		// ROXML_L_PREV
		node->axis = ROXML_ID_PREV;
		node->name = axis+strlen(ROXML_L_PREV);
	} else if(strncmp(ROXML_L_NS, axis, strlen(ROXML_L_NS))==0) {
		// ROXML_L_NS is not handled
		node->axis = ROXML_ID_NS;
		node->name = axis+strlen(ROXML_L_NS);
	} else if(strncmp(ROXML_L_CHILD, axis, strlen(ROXML_L_CHILD))==0) {
		// ROXML_L_CHILD
		node->axis = ROXML_ID_CHILD;
		node->name = axis+strlen(ROXML_L_CHILD);
	} else {
		// ROXML_S_CHILD
		node->axis = ROXML_ID_CHILD;
		node->name = axis;
	}
	return node;
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
	int is_first_node = 1;
	int wait_first_node = 1;

	xpath_node_t * first_node = (xpath_node_t*)calloc(1, sizeof(xpath_node_t));
	xpath_node_t * new_node = first_node;
	xpath_cond_t * new_cond = NULL;

	for(cur = 0; cur < len; cur++)	{
		if(wait_first_node) {
			if(ROXML_WHITE(path[cur])) { continue; }
		}
		if((path[cur] == '/')||(is_first_node)) {
			if(!quoted && !dquoted && !parenthesys && !bracket)	{
				int offset = 0;
				xpath_node_t * tmp_node = (xpath_node_t*)calloc(1, sizeof(xpath_node_t));
				if((path[cur] == '/')&&(is_first_node)) { 
					free(first_node);
					first_node = tmp_node;
					new_node = tmp_node;
					first_node->abs = 1;
				} else if(is_first_node) {
					free(tmp_node);
				} else {
					if(new_node)	{ new_node->next = tmp_node; }
					new_node = tmp_node;
				}
				i = 0;
				is_first_node = 0;
				wait_first_node = 0;
				new_node = roxml_set_axis(new_node, path+cur, &offset);
				cur += offset;
			}
		} else if(path[cur] == '"') {
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
					wait_first_node = 1;
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
					wait_first_node = 1;
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
					if(new_cond->op > 0) { new_cond->func2 = ROXML_FUNC_FIRST; }
					else { new_cond->func = ROXML_FUNC_FIRST; }
				} else if(strncmp(path+cur, ROXML_FUNC_LAST_STR, strlen(ROXML_FUNC_LAST_STR)) == 0) {
					cur += strlen(ROXML_FUNC_LAST_STR)-1;
					if(new_cond->op > 0) { new_cond->func2 = ROXML_FUNC_LAST; }
					else { new_cond->func = ROXML_FUNC_LAST; }
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
					if((new_cond->arg2[0] > '9')||(new_cond->arg2[0] < '0')) {
						new_cond->func = ROXML_FUNC_STRCOMP;
					}
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
					if((new_cond->arg2[0] > '9')||(new_cond->arg2[0] < '0')) {
						new_cond->func = ROXML_FUNC_STRCOMP;
					}
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

	if(xn == NULL) { return 1; }

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
			status = 0;
			iarg1 = roxml_get_node_index(candidat, NULL);
			iarg2 = atoi(condition->arg2);
			status = roxml_int_cmp(iarg1, iarg2, condition->op);
		} else if(condition->func == ROXML_FUNC_LAST) {
			status = 0;
			iarg1 = roxml_get_node_index(candidat, &iarg2);
			status = roxml_int_cmp(iarg1, iarg2, ROXML_OPERATOR_EQU);
		} else if(condition->func == ROXML_FUNC_FIRST) {
			status = 0;
			iarg1 = roxml_get_node_index(candidat, NULL);
			status = roxml_int_cmp(iarg1, 0, ROXML_OPERATOR_EQU);
		} else if(condition->func == ROXML_FUNC_INTCOMP) {
			status = 0;
			char strval[ROXML_BASE_LEN];
			node_t *val = roxml_get_attr(candidat, condition->arg1+1, 0);
			if(val) {
				iarg1 = atoi(roxml_get_content(val, strval, ROXML_BASE_LEN, &status));
				if(status >= ROXML_BASE_LEN) {
					iarg1 = atoi(roxml_get_content(val, NULL, 0, &status));
				}
				iarg2 = atoi(condition->arg2);
				status = roxml_int_cmp(iarg1, iarg2, condition->op);
				roxml_release(RELEASE_LAST);
			}
		} else if(condition->func == ROXML_FUNC_STRCOMP) {
			status = 0;
			char strval[ROXML_BASE_LEN];
			node_t *val = roxml_get_attr(candidat, condition->arg1+1, 0);
			if(val) {
				sarg1 = roxml_get_content(val, strval, ROXML_BASE_LEN, &status);
				if(status >= ROXML_BASE_LEN) {
					sarg1 = roxml_get_content(val, NULL, 0, &status);
				}
				sarg2 = condition->arg2;
				status = (strcmp(sarg1, sarg2)==0);
				roxml_release(sarg1);
			}
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

int roxml_validate_axes(node_t *candidat, node_t ***ans, int *nb, int *max, xpath_node_t *xn)
{
	
	int valid = 0;
	int path_end = 0;
	char * axes = NULL;
	char intern_buff[INTERNAL_BUF_SIZE];

	if(xn == NULL) {
		valid = 1;
		path_end = 1;
	} else {
		axes = xn->name;
		if((axes == NULL) || (strcmp("node()", axes) == 0))  { 
			valid = 1;
		} else if(strcmp("*", axes) == 0)  { 
			if(candidat->type & ROXML_STD_NODE) { valid = 1; }
		} else if(strcmp("text()", axes) == 0)  { 
			if(candidat->type & ROXML_TXT_NODE) { valid = 1; }
		}
		if(xn->next == NULL) { path_end = 1; }
		if((xn->axis == ROXML_ID_SELF)||(xn->axis == ROXML_ID_PARENT)) { valid = 1; }
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

	if((valid)&&(path_end)) {
		if((*nb) >= (*max))	{
			int new_max = (*max)*2;
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
	int validate_node = 0;

	if(!xp)	{ return; }

	// if found a "all document" axes
	if(ignore == ROXML_DESC_ONLY)	{
		node_t *current = context->chld;
		while(current)	{
			roxml_check_node(xp, current, ans, nb, max, ignore);
			current = current->sibl;
		}
	}

	switch(xp->axis) {
		case ROXML_ID_CHILD: {
			node_t *current = context->chld;
			while(current)	{
				validate_node = roxml_validate_axes(current, ans, nb, max, xp);
				if(validate_node)	{
					roxml_check_node(xp->next, current, ans, nb, max, ROXML_DIRECT);
				}
				current = current->sibl;
			}
			if((xp->name == NULL)||(strcmp(xp->name, "text()") == 0)||(strcmp(xp->name, "node()") == 0)) {
				node_t *current = context->text;
				while(current)	{
					validate_node = roxml_validate_axes(current, ans, nb, max, xp);
					current = current->sibl;
				}
			} 
			if((xp->name == NULL)||(strcmp(xp->name, "node()") == 0)) {
				node_t *current = context->attr;
				while(current)	{
					validate_node = roxml_validate_axes(current, ans, nb, max, xp);
					current = current->sibl;
				}
			}
		} break;
		case ROXML_ID_DESC: {
			xp = xp->next;
			roxml_check_node(xp, context, ans, nb, max, ROXML_DESC_ONLY);
		} break;
		case ROXML_ID_DESC_O_SELF: {
			xp = xp->next;
			validate_node = roxml_validate_axes(context, ans, nb, max, xp);
			if(validate_node) {
				roxml_check_node(xp->next, context, ans, nb, max, ROXML_DIRECT);
			}
			roxml_check_node(xp, context, ans, nb, max, ROXML_DESC_ONLY);
		} break;
		case ROXML_ID_SELF: {
			validate_node = roxml_validate_axes(context, ans, nb, max, xp);
			roxml_check_node(xp->next, context, ans, nb, max, ROXML_DIRECT);
		} break;
		case ROXML_ID_PARENT: {
			validate_node = roxml_validate_axes(context->prnt, ans, nb, max, xp);
			roxml_check_node(xp->next, context->prnt, ans, nb, max, ROXML_DIRECT);
		} break;
		case ROXML_ID_ATTR: {
			node_t *attribute = context->attr;
			while(attribute)  {
				validate_node = roxml_validate_axes(attribute, ans, nb, max, xp);
				if(validate_node)	{
					roxml_check_node(xp->next, context, ans, nb, max, ROXML_DIRECT);
				}
				attribute = attribute->sibl;
			}
		} break;
		case ROXML_ID_ANC: {
			node_t *current = context->prnt;
			while(current)	{
				validate_node = roxml_validate_axes(current, ans, nb, max, xp);
				if(validate_node)	{
					roxml_check_node(xp->next, current, ans, nb, max, ROXML_DIRECT);
				}
				current = current->prnt;
			}
		} break;
		case ROXML_ID_NEXT_SIBL: {
			node_t *current = context->sibl;
			while(current)	{
				validate_node = roxml_validate_axes(current, ans, nb, max, xp);
				if(validate_node)	{
					roxml_check_node(xp->next, current, ans, nb, max, ROXML_DIRECT);
				}
				current = current->sibl;
			}
		} break;
		case ROXML_ID_PREV_SIBL: {
			node_t *current = context->prnt->chld;
			while(current != context)	{
				validate_node = roxml_validate_axes(current, ans, nb, max, xp);
				if(validate_node)	{
					roxml_check_node(xp->next, current, ans, nb, max, ROXML_DIRECT);
				}
				current = current->sibl;
			}
		} break;
		case ROXML_ID_NEXT: {
			node_t *current = context;
			while(current)  {
				node_t * following = current->sibl;
				while(following) {
					validate_node = roxml_validate_axes(following, ans, nb, max, xp);
					if(validate_node)	{
						roxml_check_node(xp->next, following, ans, nb, max, ROXML_DIRECT);
					} else {
						xp->axis = ROXML_ID_CHILD;
						roxml_check_node(xp, following, ans, nb, max, ROXML_DESC_ONLY);
						xp->axis = ROXML_ID_NEXT;
					}
					following = following->sibl;
				}
				following = current->prnt->chld;
				while(following != current) { following = following->sibl; }
				current = following->sibl;
			}
		} break;
		case ROXML_ID_PREV: {
			node_t *current = context;
			while(current && current->prnt) {
				node_t *preceding = current->prnt->chld;
				while(preceding != current)  {
					validate_node = roxml_validate_axes(preceding, ans, nb, max, xp);
					if(validate_node)	{
						roxml_check_node(xp->next, preceding, ans, nb, max, ROXML_DIRECT);
					} else {
						xp->axis = ROXML_ID_CHILD;
						roxml_check_node(xp, preceding, ans, nb, max, ROXML_DESC_ONLY);
						xp->axis = ROXML_ID_PREV;
					}
					preceding = preceding->sibl;
				}
				current = current->prnt;
			}
		} break;
		case ROXML_ID_NS: {
			// ROXML_L_NS is not handled
		} break;
		case ROXML_ID_ANC_O_SELF: {
			node_t *current = context;
			while(current)	{
				validate_node = roxml_validate_axes(current, ans, nb, max, xp);
				if(validate_node)	{
					roxml_check_node(xp->next, current, ans, nb, max, ROXML_DIRECT);
				}
				current = current->prnt;
			}
		} break;
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
		xpath_node_t *cur_xpath = &xpath[path_id];
		if(cur_xpath->abs)	{
			// context node is root
			while(orig->prnt) { orig = orig->prnt; }
		}
		roxml_check_node(cur_xpath, orig, &ans, &ansnb, &ansmax, ROXML_DIRECT);
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

void roxml_print_space(FILE *f, char * buf, int * offset, int * len, int lvl)
{
	int i = 0;
	if(buf) {
		int pos = *offset + lvl;
		if(pos >= *len) { reallocf(buf, *len+ROXML_LONG_LEN); }
		for(; i < lvl; i++) {
			strcat(buf, " ");
		}
		*offset = pos;
	}
	if(f) {
		for(; i < lvl; i++) {
			fwrite(" ", 1, 1, f);
		}
	}
}

void roxml_write_string(char * buf, FILE * f, char * str, int *offset, int * len)
{
	int pos = *offset + strlen(str);
	if(pos >= *len) { reallocf(buf, *len+ROXML_LONG_LEN); }
	if(f) { fprintf(f, "%s", str); }
	if(buf) { strcat(buf+(*offset), str); }
	*offset = pos;
}

void roxml_write_node(node_t * n, FILE *f, char * buf, int human, int lvl, int *offset, int *len)
{
	char name[ROXML_BASE_LEN];
	roxml_get_name(n, name, ROXML_BASE_LEN);
	if(human) {
		roxml_print_space(f, buf, offset, len, lvl);
	}
	roxml_write_string(buf, f, "<", offset, len);
	roxml_write_string(buf, f, name, offset, len);
	if(n->chld || n->text)	{
		node_t *chld = n->chld;
		node_t *text = n->text;
		node_t *attr = n->attr;
		while(attr)	{
			int status = 0;
			char *value;
			char arg[ROXML_BASE_LEN];
			char val[ROXML_BASE_LEN];
			roxml_get_name(attr, arg, ROXML_BASE_LEN);
			value = roxml_get_content(attr, val, ROXML_BASE_LEN, &status);
			if(status >= ROXML_BASE_LEN) {
				value = roxml_get_content(attr, NULL, 0, &status);
			}
			roxml_write_string(buf, f, " ", offset, len);
			roxml_write_string(buf, f, arg, offset, len);
			roxml_write_string(buf, f, "=", offset, len);
			roxml_write_string(buf, f, value, offset, len);
			attr = attr->sibl;
		}
		roxml_write_string(buf, f, ">", offset, len);
		if(human) {
			roxml_write_string(buf, f, "\n", offset, len);
		}
		while(chld || text)	{
			char val[ROXML_LONG_LEN];
			if(text) {
				char *value;
				int status;
				if(human) {
					roxml_print_space(f, buf, offset, len, lvl+1);
				}
				value = roxml_get_content(text, val, ROXML_LONG_LEN, &status);
				if(status >= ROXML_LONG_LEN) {
					value = roxml_get_content(text, NULL, 0, &status);
				}
				roxml_write_string(buf, f, value, offset, len);
				if(human) {
					roxml_write_string(buf, f, "\n", offset, len);
				}
				text = text->sibl;
			}
			if(chld) {
				roxml_write_node(chld, f, buf, human, lvl+1, offset, len);
				chld = chld->sibl;
			}
		}
		if(human) {
			roxml_print_space(f, buf, offset, len, lvl);
		}
		roxml_write_string(buf, f, "</", offset, len);
		roxml_write_string(buf, f, name, offset, len);
		roxml_write_string(buf, f, ">", offset, len);
		if(human) {
			roxml_write_string(buf, f, "\n", offset, len);
		}
	} else {
		if(human) {
			roxml_print_space(f, buf, offset, len, lvl);
		}
		roxml_write_string(buf, f, "/>", offset, len);
		if(human) {
			roxml_write_string(buf, f, "\n", offset, len);
		}
	}
}

void roxml_del_node(node_t * n) {
	if(n == NULL) return;
	if(n->type & ROXML_STD_NODE) {
		roxml_del_std_node(n);
	} else if(n->type & ROXML_ATTR_NODE) {
		roxml_del_arg_node(n);
	} else if(n->type & ROXML_TXT_NODE) {
		roxml_del_txt_node(n);
	}
	roxml_free_node(n);
}

void roxml_del_arg_node(node_t * n) 
{
	node_t *current = n->prnt->attr;
	if(current == n) {
		n->prnt->attr = n->sibl;
	} else if(current) {
		while(current->sibl && current->sibl != n) {
			current = current->sibl;
		}
		current->sibl = n->sibl;
	}
	return;
}

void roxml_del_txt_node(node_t * n)
{
	node_t *current = n->prnt->text;
	if(current == n) {
		n->prnt->text = n->sibl;
	} else if(current) {
		while(current->sibl && current->sibl != n) {
			current = current->sibl;
		}
		current->sibl = n->sibl;
	}
} 

void roxml_del_std_node(node_t * n)
{
	node_t *current = n->prnt->chld;
	if(current == n) {
		n->prnt->chld = n->sibl;
	} else if(current) {
		while(current->sibl && current->sibl != n) {
			current = current->sibl;
		}
		current->sibl = n->sibl;
	}
	roxml_del_tree(n->chld);
	roxml_del_tree(n->text);
	roxml_del_tree(n->attr);
} 

node_t * roxml_add_node(node_t * parent, int type, char *name, char *value) 
{
	int name_l = 0;
	int end_node = 0;
	int content_l = 0;
	int content_pos = 0;
	int end_content = 0;
	char * buffer = NULL;

	if(parent && !(parent->type & ROXML_STD_NODE)) {
		return NULL;
	}
	if(value) {
		content_l = strlen(value);
	}
	if(name) {
		name_l = strlen(name);
	}

	if(type & ROXML_ATTR_NODE) {
		if(!name || !value) { return NULL; }
		buffer = (char*)malloc(sizeof(char)*(name_l+content_l+2));
		sprintf(buffer,"%s=%s",name, value);
		content_pos = name_l+1;
		end_node = name_l + 1;
		end_content = name_l + content_l + 2;
	} else if(type & ROXML_TXT_NODE) {
		if(!value) { return NULL; }
		buffer = (char*)malloc(sizeof(char)*(content_l+1));
		sprintf(buffer,"%s", value);
		content_pos = 0;
		end_node = content_l + 1;
		end_content = content_l + 1;
	} else if(type & ROXML_STD_NODE) {
		if(!name) { return NULL; }
		if(content_l)	{
			if(!value) { return NULL; }
			buffer = (char*)malloc(sizeof(char)*(name_l*2+content_l+6));
			sprintf(buffer,"<%s>%s</%s>",name, value, name);
			content_pos = name_l+2;
			end_node = name_l + content_l + 2;
			end_content = end_node;
		} else {
			buffer = (char*)malloc(sizeof(char)*(name_l+4));
			sprintf(buffer,"<%s/>",name);
		}
	}

	node_t *new_node = roxml_create_node(0, NULL, buffer, type | ROXML_PENDING | ROXML_BUFF);
	new_node->end = end_node;

	if(content_l && name_l) {
		node_t *new_txt = roxml_create_node(content_pos, NULL, buffer, ROXML_TXT_NODE | ROXML_PENDING | ROXML_BUFF);
		roxml_parent_node(new_node, new_txt);
		new_txt->end = end_content;
	}

	roxml_parent_node(parent, new_node);

	return new_node;
}

void roxml_commit_changes(node_t *n, char * dest, char ** buffer, int human)
{
	int size;
	int len = ROXML_LONG_LEN;
	FILE *fout = NULL;
	char * buf = NULL;

	if(dest) { 
		fout = fopen(dest, "w");
	}
	if(buffer) {
		buf = (char*)malloc(ROXML_LONG_LEN);
		*buffer = buf;
	}
	roxml_write_node(n, fout, buf, human, 0, &size, &len);
	if(fout) {
		fclose(fout);
	}
}

