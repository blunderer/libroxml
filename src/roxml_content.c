#include <string.h>
#include <stdlib.h>
#include <roxml_mem.h>
#include <roxml_file.h>
#include <roxml_buff.h>
#include <roxml_content.h>

ROXML_INT inline int roxml_read(int pos, int size, char *buffer, node_t *node)
{
	int ret_len = 0;

	if (size > 0 && buffer) {
		if (node->type & ROXML_FILE)
			ret_len = roxml_read_file(pos, size, buffer, node);
		else
			ret_len = roxml_read_buff(pos, size, buffer, node);
	}

	return ret_len;
}

ROXML_API char *roxml_get_content(node_t *n, char *buffer, int bufsize, int *size)
{
	int total = 0;
	char *content = buffer;

	if (n == NULL) {
		if (size)
			*size = 0;
		if (buffer)
			strcpy(buffer, "");
		return buffer;
	} else if (n->type & ROXML_ELM_NODE) {
		node_t *ptr = n->chld;
		while (ptr) {
			if ((ptr->type & ROXML_NODE_TYPES) == ROXML_TXT_NODE)
				total += ptr->end - ptr->pos;
			ptr = ptr->sibl;
		}

		if (content == NULL) {
			content = roxml_malloc(sizeof(char), total + 1, PTR_CHAR);
			bufsize = total + 1;
		}
		if (content == NULL)
			return NULL;

		total = 0;
		ptr = n->chld;

		while (ptr) {
			if ((ptr->type & ROXML_NODE_TYPES) == ROXML_TXT_NODE) {
				int ret_len = 0;
				int read_size = ptr->end - ptr->pos;

				if (total + read_size > bufsize - 1)
					read_size = bufsize - total - 1;
				ret_len += roxml_read(ptr->pos, read_size, content + total, ptr);

				total += ret_len;
			}
			ptr = ptr->sibl;
		}
	} else {
		node_t *target = n;
		int read_size = 0;
		int name_len = 0;
		int spec_offset = 0;

		ROXML_GET_BASE_BUFFER(name);

		roxml_get_name(n, name, ROXML_BASE_LEN);
		name_len = strlen(name);

		ROXML_PUT_BASE_BUFFER(name);

		if (n->type & ROXML_DOCTYPE_NODE) {
			total = target->end - target->pos - name_len - 2;
			spec_offset = target->pos + name_len + 2;
		} else if (n->type & ROXML_TXT_NODE) {
			total = target->end - target->pos;
			spec_offset = target->pos;
		} else if (n->type & ROXML_CMT_NODE) {
			total = target->end - target->pos - 4;
			spec_offset = target->pos + 4;
		} else if (n->type & ROXML_PI_NODE) {
			total = target->end - target->pos - name_len - 3;
			spec_offset = target->pos + name_len + 3;
		} else if (n->type & ROXML_ATTR_NODE) {
			target = n->chld;
			if (target) {
				spec_offset = target->pos;
				total = target->end - target->pos;
			} else {
				spec_offset = 0;
				total = 0;
			}
		}

		if (content == NULL) {
			content = roxml_malloc(sizeof(char), total + 1, PTR_CHAR);
			bufsize = total + 1;
		}
		if (content == NULL)
			return NULL;

		read_size = total;
		if (read_size > bufsize - 1)
			read_size = bufsize - 1;
		total = roxml_read(spec_offset, read_size, content, target);
	}

	content[total] = '\0';
	if (size)
		*size = total + 1;
	return content;
}

ROXML_API char *roxml_get_name(node_t *n, char *buffer, int size)
{
	int offset = 0;

	ROXML_GET_BASE_BUFFER(tmp_name);

	memset(tmp_name, 0, ROXML_BASE_LEN * sizeof(char));

	if (buffer)
		memset(buffer, 0, size * sizeof(char));

	if (n == NULL) {
		ROXML_PUT_BASE_BUFFER(tmp_name);
		return buffer;
	}

	if (n->prnt == NULL) {
		strcpy(tmp_name, "documentRoot");
	} else if (n->type & ROXML_NS_NODE) {
		roxml_ns_t *ns = (roxml_ns_t *) n->priv;
		if (ns)
			strncpy(tmp_name, ns->alias, ROXML_BASE_LEN-1);
		else
			tmp_name[0] = '\0';
	} else if ((n->type & ROXML_TXT_NODE) || (n->type & ROXML_CMT_NODE)) {
		ROXML_PUT_BASE_BUFFER(tmp_name);
		if (buffer)
			strcpy(buffer, "");
		return NULL;
	} else {
		int count = 0;
		int total = 0;
		int spec_offset = 0;

		if (n->type & ROXML_PI_NODE)
			spec_offset = 2;
		else if (n->type & ROXML_DOCTYPE_NODE)
			spec_offset = 1;

		if ((total = roxml_read(n->pos + spec_offset, ROXML_BASE_LEN, tmp_name, n)) > 0) {
			while (ROXML_WHITE(tmp_name[offset]) || tmp_name[offset] == '<')
				offset++;
			count = offset;
		} else {
			count = 0;
		}

		if (n->type & ROXML_PI_NODE) {
			for (; count < total; count++) {
				if (ROXML_WHITE(tmp_name[count]))
					break;
				else if ((tmp_name[count] == '?') && (tmp_name[count + 1] == '>'))
					break;
			}
		} else if (n->type & ROXML_ELM_NODE) {
			for (; count < total; count++) {
				if (ROXML_WHITE(tmp_name[count]))
					break;
				else if ((tmp_name[count] == '/') && (tmp_name[count + 1] == '>'))
					break;
				else if (tmp_name[count] == '>')
					break;
			}
		} else if (n->type & ROXML_ATTR_NODE) {
			for (; count < total; count++) {
				if (ROXML_WHITE(tmp_name[count]))
					break;
				else if (tmp_name[count] == '=')
				      break;
				else if (tmp_name[count] == '>')
				      break;
				else if ((tmp_name[count] == '/') && (tmp_name[count + 1] == '>'))
				      break;
			}
		} else if (n->type & ROXML_DOCTYPE_NODE) {
			for (; count < total; count++) {
				if (ROXML_WHITE(tmp_name[count]))
					break;
				else if (tmp_name[count] == '>')
					break;
			}
		}
		tmp_name[count] = '\0';
	}

	if (buffer == NULL) {
		buffer = (char *)roxml_malloc(sizeof(char), strlen(tmp_name) - offset + 1, PTR_CHAR);
		strcpy(buffer, tmp_name + offset);
	} else {
		if (strlen(tmp_name) - offset < (unsigned int)size)
			size = strlen(tmp_name) - offset;
		strncpy(buffer, tmp_name + offset, size);
	}

	ROXML_PUT_BASE_BUFFER(tmp_name);

	return buffer;
}

ROXML_API int roxml_get_nodes_nb(node_t *n, int type)
{
	node_t *ptr = n;
	int nb = -1;
	if (n) {
		nb = 0;
		if (ptr->chld) {
			ptr = ptr->chld;
			do {
				if (ptr->type & type)
					nb++;
				ptr = ptr->sibl;
			} while (ptr);
		}

		if (type & ROXML_ATTR_NODE) {
			ptr = n->attr;
			while (ptr) {
				nb++;
				ptr = ptr->sibl;
			}
		}
	}
	return nb;
}

ROXML_API node_t *roxml_get_nodes(node_t *n, int type, char *name, int nth)
{
	node_t *ptr = NULL;

	if (n == NULL)
		return NULL;

	if (name == NULL) {
		int count = 0;
		if (n->ns && (type & ROXML_NS_NODE)) {
			ptr = n->ns;
			if (nth == 0)
				return ptr;
		} else if (n->attr && (type & ROXML_ATTR_NODE)) {
			ptr = n->attr;
			if (nth == 0)
				return ptr;
			while ((ptr->sibl) && (nth > count)) {
				ptr = ptr->sibl;
				count++;
			}
		} else {
			ptr = n->chld;
			while (ptr && !((ptr->type & ROXML_NODE_TYPES) & type))
				ptr = ptr->sibl;
		}
		if (nth > count) {
			ptr = n->chld;
			while (ptr && !((ptr->type & ROXML_NODE_TYPES) & type))
				ptr = ptr->sibl;
			while (ptr && (ptr->sibl) && (nth > count)) {
				ptr = ptr->sibl;
				if ((ptr->type & ROXML_NODE_TYPES) & type)
					count++;
			}
		}
		if (nth > count)
			return NULL;
	} else {
		if (n->attr && (type & ROXML_ATTR_NODE)) {
			ptr = n->attr;
			while (ptr) {
				int ans = strcmp(roxml_get_name(ptr, NULL, 0), name);
				roxml_release(RELEASE_LAST);
				if (ans == 0)
					return ptr;
				ptr = ptr->sibl;
			}
		}
		ptr = n->chld;
		while (ptr) {
			if ((ptr->type & ROXML_NODE_TYPES) & type) {
				int ans = strcmp(roxml_get_name(ptr, NULL, 0), name);
				roxml_release(RELEASE_LAST);
				if (ans == 0)
					return ptr;
			}
			ptr = ptr->sibl;
		}
	}
	return ptr;
}

ROXML_API inline node_t *roxml_get_ns(node_t *n)
{
        return roxml_get_nodes(n, ROXML_NS_NODE, NULL, 0);
}

ROXML_API inline int roxml_get_pi_nb(node_t *n)
{
	return roxml_get_nodes_nb(n, ROXML_PI_NODE);
}

ROXML_API inline node_t *roxml_get_pi(node_t *n, int nth)
{
	return roxml_get_nodes(n, ROXML_PI_NODE, NULL, nth);
}

ROXML_API inline int roxml_get_cmt_nb(node_t *n)
{
	return roxml_get_nodes_nb(n, ROXML_CMT_NODE);
}

ROXML_API inline node_t *roxml_get_cmt(node_t *n, int nth)
{
	return roxml_get_nodes(n, ROXML_CMT_NODE, NULL, nth);
}

ROXML_API inline int roxml_get_txt_nb(node_t *n)
{
	return roxml_get_nodes_nb(n, ROXML_TXT_NODE);
}

ROXML_API inline node_t *roxml_get_txt(node_t *n, int nth)
{
	return roxml_get_nodes(n, ROXML_TXT_NODE, NULL, nth);
}

ROXML_API inline int roxml_get_attr_nb(node_t *n)
{
	return roxml_get_nodes_nb(n, ROXML_ATTR_NODE);
}

ROXML_API inline node_t *roxml_get_attr(node_t *n, char *name, int nth)
{
	return roxml_get_nodes(n, ROXML_ATTR_NODE, name, nth);
}

ROXML_API inline int roxml_get_chld_nb(node_t *n)
{
	return roxml_get_nodes_nb(n, ROXML_ELM_NODE);
}

ROXML_API inline node_t *roxml_get_chld(node_t *n, char *name, int nth)
{
	return roxml_get_nodes(n, ROXML_ELM_NODE, name, nth);
}

ROXML_API inline int roxml_get_type(node_t *n)
{
	if (n)
		return (n->type & ROXML_NODE_TYPES);
	return 0;
}

ROXML_API int roxml_get_node_position(node_t *n)
{
	int idx = 1;
	char name[256];
	node_t *prnt;
	node_t *first;

	if (n == NULL)
		return 0;

	roxml_get_name(n, name, 256);

	prnt = n->prnt;
	if (!prnt)
		return 1;
	first = prnt->chld;

	while ((first) && (first != n)) {
		char twin[256];

		roxml_get_name(first, twin, 256);
		if (strcmp(name, twin) == 0)
			idx++;
		first = first->sibl;
	}

	return idx;
}
