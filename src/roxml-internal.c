/** \file roxml-internal.c
 *  \brief source for libroxml.so
 *         
 * This is the source file for lib libroxml.so internal functions
 * \author blunderer <blunderer@blunderer.org>
 * \date 11 Jan 2010
 *
 * Copyright (C) 2010 blunderer
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

void * ROXML_INT roxml_malloc(int size, int num, int type)
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

node_t * ROXML_INT roxml_create_node(int pos, void *src, int type)
{
	node_t *n = (node_t*)calloc(1, sizeof(node_t));
	n->type = type;
	n->src.src = src;
	n->pos = pos;
	n->end = pos;

	return n;
}

void ROXML_INT roxml_close_node(node_t *n, node_t *close)
{
	n->end = close->pos;
	free(close);

	if((n->type & ROXML_STD_NODE) == ROXML_STD_NODE) {
		n->next = NULL;
	}

#ifdef __DEBUG
	if(n->type & ROXML_STD_NODE) _nb_node++;
	if(n->type & ROXML_ATTR_NODE) _nb_attr++;
	if(n->type & ROXML_TXT_NODE) _nb_text++;
#endif
}

void ROXML_INT roxml_free_node(node_t *n)
{
	if(n->type & ROXML_PENDING) {
		if(n->pos == 0) {
			free(n->src.buf);
		}
	}
	if(n->priv) {
		xpath_tok_t * tok = (xpath_tok_t*)n->priv;
		if(tok->id == ROXML_REQTABLE_ID) {
			xpath_tok_table_t * table = (xpath_tok_table_t*)n->priv;
			tok = table->next;
			free(table);
		}
		while(tok) {
			xpath_tok_t * to_delete = tok;
			tok = tok->next;
			free(to_delete);
		}
	}
	free(n);
}

void ROXML_INT roxml_del_tree(node_t *n)
{
	if(n == NULL)	{ return; }
	roxml_del_tree(n->chld);
	roxml_del_tree(n->text);
	roxml_del_tree(n->sibl);
	roxml_del_tree(n->attr);
	roxml_free_node(n);
}

node_t * ROXML_INT roxml_load(node_t *current_node, FILE *file, char *buffer)
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

	void * src = NULL;

	if(file)	{ 
		type = ROXML_FILE;
		src = (void*)file;
	} else	{
		type = ROXML_BUFF;
		src = (void*)buffer;
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
				candidat_val = roxml_create_node(int_rel_pos, src, ROXML_TXT_NODE | type);
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
							node_t * to_be_closed = roxml_create_node(int_rel_pos, src, ROXML_TXT_NODE | type);
							roxml_close_node(candidat_txt, to_be_closed);
#ifdef IGNORE_EMPTY_TEXT_NODES
						} else {
							roxml_del_node(candidat_txt);
						}
#endif /* IGNORE_EMPTY_TEXT_NODES */
						candidat_txt = NULL;
					}
					candidat_node = roxml_create_node(int_rel_pos, src, ROXML_STD_NODE | type);
				} else if(c == '>')	{
					if(state == STATE_NODE_NAME)	{
						empty_text_node = 1;
						state = STATE_NODE_CONTENT;
						current_node = roxml_parent_node(current_node, candidat_node);
						candidat_txt = roxml_create_node(int_rel_pos+1, src, ROXML_TXT_NODE | type);
						candidat_txt = roxml_parent_node(current_node, candidat_txt);
					} else if(state == STATE_NODE_ATTR)	{
						if(inside_node_state == STATE_INSIDE_VAL)	{
							int off = current_attr_quoted;
							node_t * to_be_closed = roxml_create_node(int_rel_pos-off, src, ROXML_ATTR_NODE | type);
							roxml_close_node(candidat_val, to_be_closed);
						}
						current_node = roxml_parent_node(current_node, candidat_node);
						state = STATE_NODE_CONTENT;
						inside_node_state = STATE_INSIDE_ARG_BEG;
						candidat_txt = roxml_create_node(int_rel_pos+1, src, ROXML_TXT_NODE | type);
						candidat_txt = roxml_parent_node(current_node, candidat_txt);
					} else if(state == STATE_NODE_SINGLE)	{
						empty_text_node = 1;
						state = STATE_NODE_CONTENT;
						current_node = roxml_parent_node(current_node, candidat_node);
						if(current_node->prnt != NULL) { current_node = current_node->prnt; }
						candidat_txt = roxml_create_node(int_rel_pos+1, src, ROXML_TXT_NODE | type);
						candidat_txt = roxml_parent_node(current_node, candidat_txt);
					} else if(state == STATE_NODE_END)	{
						empty_text_node = 1;
						state = STATE_NODE_CONTENT;
						roxml_close_node(current_node, candidat_node);
						if(current_node->prnt != NULL) { current_node = current_node->prnt; }
						candidat_txt = roxml_create_node(int_rel_pos+1, src, ROXML_TXT_NODE | type);
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
							node_t * to_be_closed = roxml_create_node(int_rel_pos-off, src, ROXML_ATTR_NODE | type);
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
							node_t * to_be_closed = roxml_create_node(int_rel_pos-off, src, ROXML_ATTR_NODE | type);
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
							candidat_arg = roxml_create_node(int_rel_pos-1, src, ROXML_ATTR_NODE | type);
							candidat_arg = roxml_parent_node(candidat_node, candidat_arg);
							inside_node_state = STATE_INSIDE_ARG;
						} else if((inside_node_state == STATE_INSIDE_ARG)&&(c == '='))	{
							inside_node_state = STATE_INSIDE_VAL_BEG;
							node_t * to_be_closed = roxml_create_node(int_rel_pos, src, ROXML_ATTR_NODE | type);
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

xpath_node_t * ROXML_INT roxml_set_axes(xpath_node_t *node, char *axes, int *offset)
{
	xpath_node_t *tmp_node;
	if(axes[0] == '/') { 
		axes[0] = '\0'; 
		*offset += 1;
		axes++;
	}
	if(axes[0] == '/') {
		// ROXML_S_DESC_O_SELF
		node->axes = ROXML_ID_DESC_O_SELF;
		node->name = axes+1;
		tmp_node = (xpath_node_t*)calloc(1, sizeof(xpath_node_t));
		tmp_node->axes = ROXML_ID_CHILD;
		node->next = tmp_node;
		if(strlen(node->name) > 0) {
			tmp_node = (xpath_node_t*)calloc(1, sizeof(xpath_node_t));
			node->next->next = tmp_node;
			node = roxml_set_axes(tmp_node, axes+1, offset);
		}
	} else if(strncmp(ROXML_L_DESC_O_SELF, axes, strlen(ROXML_L_DESC_O_SELF))==0) {
		// ROXML_L_DESC_O_SELF
		node->axes = ROXML_ID_DESC_O_SELF;
		node->name = axes+strlen(ROXML_L_DESC_O_SELF);
		*offset += strlen(ROXML_L_DESC_O_SELF);
		tmp_node = (xpath_node_t*)calloc(1, sizeof(xpath_node_t));
		tmp_node->axes = ROXML_ID_CHILD;
		node->next = tmp_node;
		node = roxml_set_axes(tmp_node, axes+strlen(ROXML_L_DESC_O_SELF), offset);
	} else if(strncmp(ROXML_L_DESC, axes, strlen(ROXML_L_DESC))==0) {
		// ROXML_L_DESC
		node->axes = ROXML_ID_DESC;
		node->name = axes+strlen(ROXML_L_DESC);
		*offset += strlen(ROXML_L_DESC);
		tmp_node = (xpath_node_t*)calloc(1, sizeof(xpath_node_t));
		tmp_node->axes = ROXML_ID_CHILD;
		node->next = tmp_node;
		node = roxml_set_axes(tmp_node, axes+strlen(ROXML_L_DESC), offset);
	} else if(strncmp(ROXML_L_SELF, axes, strlen(ROXML_L_SELF))==0) {
		// ROXML_L_SELF
		node->axes = ROXML_ID_SELF;
		node->name = axes+strlen(ROXML_L_SELF);
	} else if(strncmp(ROXML_S_SELF, axes, strlen(ROXML_S_SELF))==0) {
		// ROXML_S_SELF
		node->axes = ROXML_ID_SELF;
		node->name = axes+strlen(ROXML_S_SELF);
	} else if(strncmp(ROXML_L_PARENT, axes, strlen(ROXML_L_PARENT))==0) {
		// ROXML_L_PARENT
		node->axes = ROXML_ID_PARENT;
		node->name = axes+strlen(ROXML_L_PARENT);
	} else if(strncmp(ROXML_S_PARENT, axes, strlen(ROXML_S_PARENT))==0) {
		// ROXML_S_PARENT
		node->axes = ROXML_ID_PARENT;
		node->name = axes+strlen(ROXML_S_PARENT);
	} else if(strncmp(ROXML_L_ATTR, axes, strlen(ROXML_L_ATTR))==0) {
		// ROXML_L_ATTR
		node->axes = ROXML_ID_ATTR;
		node->name = axes+strlen(ROXML_L_ATTR);
	} else if(strncmp(ROXML_S_ATTR, axes, strlen(ROXML_S_ATTR))==0) {
		// ROXML_S_ATTR
		node->axes = ROXML_ID_ATTR;
		node->name = axes+strlen(ROXML_S_ATTR);
	} else if(strncmp(ROXML_L_ANC, axes, strlen(ROXML_L_ANC))==0) {
		// ROXML_L_ANC
		node->axes = ROXML_ID_ANC;
		node->name = axes+strlen(ROXML_L_ANC);
	} else if(strncmp(ROXML_L_ANC_O_SELF, axes, strlen(ROXML_L_ANC_O_SELF))==0) {
		// ROXML_L_ANC_O_SELF
		node->axes = ROXML_ID_ANC_O_SELF;
		node->name = axes+strlen(ROXML_L_ANC_O_SELF);
	} else if(strncmp(ROXML_L_NEXT_SIBL, axes, strlen(ROXML_L_NEXT_SIBL))==0) {
		// ROXML_L_NEXT_SIBL
		node->axes = ROXML_ID_NEXT_SIBL;
		node->name = axes+strlen(ROXML_L_NEXT_SIBL);
	} else if(strncmp(ROXML_L_PREV_SIBL, axes, strlen(ROXML_L_PREV_SIBL))==0) {
		// ROXML_L_PREV_SIBL
		node->axes = ROXML_ID_PREV_SIBL;
		node->name = axes+strlen(ROXML_L_PREV_SIBL);
	} else if(strncmp(ROXML_L_NEXT, axes, strlen(ROXML_L_NEXT))==0) {
		// ROXML_L_NEXT
		node->axes = ROXML_ID_NEXT;
		node->name = axes+strlen(ROXML_L_NEXT);
	} else if(strncmp(ROXML_L_PREV, axes, strlen(ROXML_L_PREV))==0) {
		// ROXML_L_PREV
		node->axes = ROXML_ID_PREV;
		node->name = axes+strlen(ROXML_L_PREV);
	} else if(strncmp(ROXML_L_NS, axes, strlen(ROXML_L_NS))==0) {
		// ROXML_L_NS is not handled
		node->axes = ROXML_ID_NS;
		node->name = axes+strlen(ROXML_L_NS);
	} else if(strncmp(ROXML_L_CHILD, axes, strlen(ROXML_L_CHILD))==0) {
		// ROXML_L_CHILD
		node->axes = ROXML_ID_CHILD;
		node->name = axes+strlen(ROXML_L_CHILD);
	} else {
		// ROXML_S_CHILD
		node->axes = ROXML_ID_CHILD;
		node->name = axes;
	}
	return node;
}

/** \comment xpath is something like "/node[first() or last()]/item/title/@version || /node/item/title[@version < 3]" */
int ROXML_INT roxml_parse_xpath(char *path, xpath_node_t ** xpath)
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
				new_node = roxml_set_axes(new_node, path+cur, &offset);
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
				} else if((path[cur] == '!')&&(path[cur+1] == '=')) {
					path[cur] = '\0';
					path[cur+1] = '\0';
					new_cond->op = ROXML_OPERATOR_DIFF;
					if(ROXML_WHITE(path[cur-1])) {
						path[cur-1] = '\0';
					}
					cur++;
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

void ROXML_INT roxml_free_xcond(xpath_cond_t *xcond)
{
	if(xcond->next) {
		roxml_free_xcond(xcond->next);
	}
	free(xcond);
}

void ROXML_INT roxml_free_xpath(xpath_node_t *xpath, int nb)
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

int ROXML_INT roxml_int_cmp(int a, int b, int op)
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

int ROXML_INT roxml_validate_predicat(xpath_node_t *xn, node_t *candidat)
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

int ROXML_INT roxml_request_id(node_t *root)
{
	int i = 0;
	xpath_tok_table_t * table = NULL;
	if(root->priv == NULL) {
		table = (xpath_tok_table_t*)calloc(1, sizeof(xpath_tok_table_t));
		table->id = ROXML_REQTABLE_ID;
		table->ids[ROXML_REQTABLE_ID] = 1;
		pthread_mutex_init(&table->mut, NULL);
		root->priv = (void*)table;
	}
	table = (xpath_tok_table_t*)root->priv;
	pthread_mutex_lock(&table->mut);
	for(i = 1; i < 255; i++) {
		if(table->ids[i] == 0) {
			table->ids[i]++;
			pthread_mutex_unlock(&table->mut);
			return i;
		}
	}
	pthread_mutex_unlock(&table->mut);
	return -1;
}

void ROXML_INT roxml_release_id(node_t *root, node_t **pool, int pool_len, int req_id)
{
	int i = 0;
	xpath_tok_table_t * table = (xpath_tok_table_t*)root->priv;
	for(i = 0; i < pool_len; i++) {
		node_t *n = pool[i];
		if(n->priv) {
			pthread_mutex_lock(&table->mut);
			xpath_tok_t * prev = (xpath_tok_t*)n->priv;
			xpath_tok_t * tok = (xpath_tok_t*)n->priv;
			if(tok->id == req_id) {
				n->priv = (void*)tok->next;
				free(tok);
			} else {
				while(tok) {
					if(tok->id == req_id) {
						prev->next = tok->next;
						free(tok);
						break;
					}
					prev = tok;
					tok = tok->next;
				}
			}
			pthread_mutex_unlock(&table->mut);
		}
	}
	table->ids[req_id] = 0;
}

int ROXML_INT roxml_add_to_pool(node_t *root, node_t *n, int req_id)
{
	xpath_tok_table_t * table = (xpath_tok_table_t*)root->priv;

	pthread_mutex_lock(&table->mut);
	xpath_tok_t * tok = (xpath_tok_t*)n->priv;
	xpath_tok_t * last_tok = NULL;

	while(tok) {
		if(tok->id == req_id) {
			pthread_mutex_unlock(&table->mut);
			return 0;
		}
		last_tok = tok;
		tok = tok->next;
	}
	if(last_tok == NULL) {
		n->priv = calloc(1, sizeof(xpath_tok_t));
		last_tok = (xpath_tok_t*)n->priv;
	} else {
		last_tok->next = (xpath_tok_t*)calloc(1, sizeof(xpath_tok_t));
		last_tok = last_tok->next;
	}
	last_tok->id = req_id;
	pthread_mutex_unlock(&table->mut);

	return 1;
}

int ROXML_INT roxml_validate_axes(node_t *root, node_t *candidat, node_t ***ans, int *nb, int *max, xpath_node_t *xn, int req_id)
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
			if(candidat->type & ROXML_ATTR_NODE) { valid = 1; }
		} else if(strcmp("text()", axes) == 0)  { 
			if(candidat->type & ROXML_TXT_NODE) { valid = 1; }
		}
		if(xn->next == NULL) { path_end = 1; }
		if((xn->axes == ROXML_ID_SELF)||(xn->axes == ROXML_ID_PARENT)) { valid = 1; }
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
		if(roxml_add_to_pool(root, candidat, req_id)) {
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
	}

	return valid;
}

void ROXML_INT roxml_check_node(xpath_node_t *xp, node_t *root, node_t *context, node_t ***ans, int *nb, int *max, int ignore, int req_id)
{
	int validate_node = 0;

	if(!xp)	{ return; }

	// if found a "all document" axes
	if(ignore == ROXML_DESC_ONLY)	{
		node_t *current = context->chld;
		while(current)	{
			roxml_check_node(xp, root, current, ans, nb, max, ignore, req_id);
			current = current->sibl;
		}
	}

	switch(xp->axes) {
		case ROXML_ID_CHILD: {
			node_t *current = context->chld;
			while(current)	{
				validate_node = roxml_validate_axes(root, current, ans, nb, max, xp, req_id);
				if(validate_node)	{
					roxml_check_node(xp->next, root, current, ans, nb, max, ROXML_DIRECT, req_id);
				}
				current = current->sibl;
			}
			if((xp->name == NULL)||(strcmp(xp->name, "text()") == 0)||(strcmp(xp->name, "node()") == 0)) {
				node_t *current = context->text;
				while(current)	{
					validate_node = roxml_validate_axes(root, current, ans, nb, max, xp, req_id);
					current = current->sibl;
				}
			} 
			if((xp->name == NULL)||(strcmp(xp->name, "node()") == 0)) {
				node_t *current = context->attr;
				while(current)	{
					validate_node = roxml_validate_axes(root, current, ans, nb, max, xp, req_id);
					current = current->sibl;
				}
			}
		} break;
		case ROXML_ID_DESC: {
			xp = xp->next;
			roxml_check_node(xp, root, context, ans, nb, max, ROXML_DESC_ONLY, req_id);
		} break;
		case ROXML_ID_DESC_O_SELF: {
			xp = xp->next;
			validate_node = roxml_validate_axes(root, context, ans, nb, max, xp, req_id);
			if(validate_node) {
				roxml_check_node(xp->next, root, context, ans, nb, max, ROXML_DIRECT, req_id);
			}
			roxml_check_node(xp, root, context, ans, nb, max, ROXML_DESC_ONLY, req_id);
		} break;
		case ROXML_ID_SELF: {
			validate_node = roxml_validate_axes(root, context, ans, nb, max, xp, req_id);
			roxml_check_node(xp->next, root, context, ans, nb, max, ROXML_DIRECT, req_id);
		} break;
		case ROXML_ID_PARENT: {
			validate_node = roxml_validate_axes(root, context->prnt, ans, nb, max, xp, req_id);
			roxml_check_node(xp->next, root, context->prnt, ans, nb, max, ROXML_DIRECT, req_id);
		} break;
		case ROXML_ID_ATTR: {
			node_t *attribute = context->attr;
			while(attribute)  {
				validate_node = roxml_validate_axes(root, attribute, ans, nb, max, xp, req_id);
				if(validate_node)	{
					roxml_check_node(xp->next, root, context, ans, nb, max, ROXML_DIRECT, req_id);
				}
				attribute = attribute->sibl;
			}
		} break;
		case ROXML_ID_ANC: {
			node_t *current = context->prnt;
			while(current)	{
				validate_node = roxml_validate_axes(root, current, ans, nb, max, xp, req_id);
				if(validate_node)	{
					roxml_check_node(xp->next, root, current, ans, nb, max, ROXML_DIRECT, req_id);
				}
				current = current->prnt;
			}
		} break;
		case ROXML_ID_NEXT_SIBL: {
			node_t *current = context->sibl;
			while(current)	{
				validate_node = roxml_validate_axes(root, current, ans, nb, max, xp, req_id);
				if(validate_node)	{
					roxml_check_node(xp->next, root, current, ans, nb, max, ROXML_DIRECT, req_id);
				}
				current = current->sibl;
			}
		} break;
		case ROXML_ID_PREV_SIBL: {
			node_t *current = context->prnt->chld;
			while(current != context)	{
				validate_node = roxml_validate_axes(root, current, ans, nb, max, xp, req_id);
				if(validate_node)	{
					roxml_check_node(xp->next, root, current, ans, nb, max, ROXML_DIRECT, req_id);
				}
				current = current->sibl;
			}
		} break;
		case ROXML_ID_NEXT: {
			node_t *current = context;
			while(current)  {
				node_t * following = current->sibl;
				while(following) {
					validate_node = roxml_validate_axes(root, following, ans, nb, max, xp, req_id);
					if(validate_node)	{
						roxml_check_node(xp->next, root, following, ans, nb, max, ROXML_DIRECT, req_id);
					} else {
						xp->axes = ROXML_ID_CHILD;
						roxml_check_node(xp, root, following, ans, nb, max, ROXML_DESC_ONLY, req_id);
						xp->axes = ROXML_ID_NEXT;
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
					validate_node = roxml_validate_axes(root, preceding, ans, nb, max, xp, req_id);
					if(validate_node)	{
						roxml_check_node(xp->next, root, preceding, ans, nb, max, ROXML_DIRECT, req_id);
					} else {
						xp->axes = ROXML_ID_CHILD;
						roxml_check_node(xp, root, preceding, ans, nb, max, ROXML_DESC_ONLY, req_id);
						xp->axes = ROXML_ID_PREV;
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
				validate_node = roxml_validate_axes(root, current, ans, nb, max, xp, req_id);
				if(validate_node)	{
					roxml_check_node(xp->next, root, current, ans, nb, max, ROXML_DIRECT, req_id);
				}
				current = current->prnt;
			}
		} break;
	}


	return;
}

void ROXML_INT roxml_print_space(FILE *f, char ** buf, int * offset, int * len, int lvl)
{
	int i = 0;
	if(*buf) {
		int pos = *offset + lvl;
		if(pos >= *len) { *buf = realloc(*buf, *len+ROXML_LONG_LEN); }
		for(; i < lvl; i++) {
			strcat(*buf, " ");
		}
		*offset = pos;
	}
	if(f) {
		for(; i < lvl; i++) {
			fwrite(" ", 1, 1, f);
		}
	}
}

void ROXML_INT roxml_write_string(char ** buf, FILE * f, char * str, int *offset, int * len)
{
	int pos = *offset + strlen(str);
	if((pos >= *len)&&(*buf)) { *buf = realloc(*buf, *len+ROXML_LONG_LEN); }
	if(f) { fprintf(f, "%s", str); }
	if(*buf) { strcat(*buf+(*offset), str); }
	*offset = pos;
}

void ROXML_INT roxml_write_node(node_t * n, FILE *f, char * buf, int human, int lvl, int *offset, int *len)
{
	char name[ROXML_BASE_LEN];
	roxml_get_name(n, name, ROXML_BASE_LEN);
	if(human) {
		roxml_print_space(f, &buf, offset, len, lvl);
	}
	roxml_write_string(&buf, f, "<", offset, len);
	roxml_write_string(&buf, f, name, offset, len);
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
			roxml_write_string(&buf, f, " ", offset, len);
			roxml_write_string(&buf, f, arg, offset, len);
			roxml_write_string(&buf, f, "=", offset, len);
			roxml_write_string(&buf, f, value, offset, len);
			attr = attr->sibl;
			roxml_release(value);
		}
		roxml_write_string(&buf, f, ">", offset, len);
		if(human) {
			roxml_write_string(&buf, f, "\n", offset, len);
		}
		while(chld || text)	{
			char val[ROXML_LONG_LEN];
			if(text) {
				char *value;
				int status;
				if(human) {
					roxml_print_space(f, &buf, offset, len, lvl+1);
				}
				value = roxml_get_content(text, val, ROXML_LONG_LEN, &status);
				if(status >= ROXML_LONG_LEN) {
					value = roxml_get_content(text, NULL, 0, &status);
				}
				roxml_write_string(&buf, f, value, offset, len);
				if(human) {
					roxml_write_string(&buf, f, "\n", offset, len);
				}
				text = text->sibl;
				roxml_release(value);
			}
			if(chld) {
				roxml_write_node(chld, f, buf, human, lvl+1, offset, len);
				chld = chld->sibl;
			}
		}
		if(human) {
			roxml_print_space(f, &buf, offset, len, lvl);
		}
		roxml_write_string(&buf, f, "</", offset, len);
		roxml_write_string(&buf, f, name, offset, len);
		roxml_write_string(&buf, f, ">", offset, len);
		if(human) {
			roxml_write_string(&buf, f, "\n", offset, len);
		}
	} else {
		if(human) {
			roxml_print_space(f, &buf, offset, len, lvl);
		}
		roxml_write_string(&buf, f, "/>", offset, len);
		if(human) {
			roxml_write_string(&buf, f, "\n", offset, len);
		}
	}
}

void ROXML_INT roxml_del_arg_node(node_t * n) 
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

void ROXML_INT roxml_del_txt_node(node_t * n)
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

void ROXML_INT roxml_del_std_node(node_t * n)
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

