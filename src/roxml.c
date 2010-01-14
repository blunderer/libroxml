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

void ROXML_API roxml_release(void * data)
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

char * ROXML_API roxml_get_content(node_t *n, char * name, int bufsize, int *size)
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

char * ROXML_API roxml_get_name(node_t *n, char * name, int size)
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

node_t * ROXML_API roxml_get_text(node_t *n, int nb)
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

node_t * ROXML_API roxml_get_attr(node_t *n, char * name, int nb)
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

void ROXML_API roxml_close(node_t *n)
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

int ROXML_API roxml_get_chld_nb(node_t *n)
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

node_t * ROXML_API roxml_get_chld(node_t *n, char * name, int nb)
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

node_t * ROXML_API roxml_get_parent(node_t *n)
{
	if(n->prnt == NULL)	{
		return n;
	} else	{
		return n->prnt;
	}
	return NULL;
}

int ROXML_API roxml_get_type(node_t *n)
{
	return (n->type & (ROXML_ATTR_NODE | ROXML_STD_NODE | ROXML_TXT_NODE));
}

int ROXML_API roxml_get_node_index(node_t *n, int * last)
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

node_t * ROXML_API roxml_load_doc(char *filename)
{
	node_t *current_node = NULL;
	FILE* file = fopen(filename, "r");
	if(file == NULL)	{
		return NULL;
	}
	current_node = roxml_create_node(0, file, ROXML_STD_NODE | ROXML_FILE);
	current_node = roxml_parent_node(NULL, current_node);
	return roxml_load(current_node,  file, NULL);
}

node_t * ROXML_API roxml_load_buf(char *buffer)
{
	if(buffer == NULL)	{ return NULL; }
	node_t *current_node = NULL;
	current_node = roxml_create_node(0, buffer, ROXML_STD_NODE | ROXML_BUFF);
	current_node = roxml_parent_node(NULL, current_node);
	return roxml_load(current_node, NULL, buffer);
}

node_t ** ROXML_API roxml_xpath(node_t *n, char * path, int *nb_ans)
{
	int path_id;
	int index = 0;
	int ansnb = 0;
	int ansmax = 1;
	xpath_node_t *xpath = NULL;
	node_t *root = n;

	if(n == NULL)	{ 
		if(nb_ans) { *nb_ans = 0; }
		return NULL; 
	}

	while(root->prnt) { root = root->prnt; }

	node_t **ans = roxml_malloc(sizeof(node_t*), ansmax, PTR_NODE_RESULT);
	char * full_path_to_find = strdup(path);
	char * path_to_find = full_path_to_find;

	index = roxml_parse_xpath(path_to_find, &xpath);

	for(path_id = 0; path_id < index; path_id++)	{
		int req_id;
		node_t *orig = n;
		xpath_node_t *cur_xpath = &xpath[path_id];
		if(cur_xpath->abs)	{
			// context node is root
			orig = root;
		}
		// assign a new request ID
		req_id = roxml_request_id(root);
		roxml_check_node(cur_xpath, root, orig, &ans, &ansnb, &ansmax, ROXML_DIRECT, req_id);
		roxml_release_id(root, ans, ansnb, req_id);
	}

	roxml_free_xpath(xpath, index);

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

void ROXML_API roxml_del_node(node_t * n) 
{
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

void ROXML_API roxml_commit_changes(node_t *n, char * dest, char ** buffer, int human)
{
	int size = 0;
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

	node_t *new_node = roxml_create_node(0, buffer, type | ROXML_PENDING | ROXML_BUFF);
	new_node->end = end_node;

	if(content_l && name_l) {
		node_t *new_txt = roxml_create_node(content_pos, buffer, ROXML_TXT_NODE | ROXML_PENDING | ROXML_BUFF);
		roxml_parent_node(new_node, new_txt);
		new_txt->end = end_content;
	}

	roxml_parent_node(parent, new_node);

	return new_node;
}

