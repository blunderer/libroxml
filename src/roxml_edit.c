#include <string.h>
#include <stdlib.h>
#include <roxml_nav.h>
#include <roxml_edit.h>
#include <roxml_core.h>

ROXML_INT node_t *roxml_parent_node(node_t *parent, node_t * n, int position)
{
        int nb;

        if (n == NULL)
                return NULL;
        if (parent == NULL)
                return n;
        if (position == 0)
                return roxml_append_node(parent, n);

        if (n->type & ROXML_ATTR_NODE)
                nb = roxml_get_attr_nb(parent);
        else
                nb = roxml_get_nodes_nb(parent, ROXML_PI_NODE | ROXML_CMT_NODE | ROXML_TXT_NODE | ROXML_ELM_NODE | ROXML_DOCTYPE_NODE);

        if (position > nb)
                return roxml_append_node(parent, n);
 
        n->prnt = parent;

        if (parent->ns && ((parent->ns->type & ROXML_INVALID) != ROXML_INVALID)
            && ((roxml_ns_t *) parent->ns->priv)->alias[0] == '\0') {
                if (n->ns == NULL)
                        n->ns = parent->ns;
        }

        if (position == 1) {
                if (n->type & ROXML_ATTR_NODE) {
                        n->sibl = parent->attr;
                        parent->attr = n;
                } else {
                        n->sibl = parent->chld;
                        parent->chld = n;
                }
        } else {
                int i;
                node_t *prev = parent->chld;
                node_t *next = parent->chld;

                if (n->type & ROXML_ATTR_NODE)
                        next = parent->attr;
                for (i = 1; i < position; i++) {
                        prev = next;
                        next = next->sibl;
                }
                n->sibl = next;
                prev->sibl = n;
        }
        return n;
}

ROXML_API void roxml_del_node(node_t *n)
{
	if (n == NULL)
		return;

	if ((n->type & ROXML_ELM_NODE) ||
	    (n->type & ROXML_DOCTYPE_NODE) || (n->type & ROXML_PI_NODE) || (n->type & ROXML_CMT_NODE)) {
		roxml_del_std_node(n);
	} else if (n->type & ROXML_ATTR_NODE) {
		roxml_del_arg_node(n);
	} else if (n->type & ROXML_TXT_NODE) {
		roxml_del_txt_node(n);
	}
	roxml_free_node(n);
}

ROXML_API node_t *roxml_add_node(node_t *parent, int position, int type, char *name, char *value)
{
	node_t *new_node = NULL;
	int name_l = 0;
	int end_node = 0;
	int content_l = 0;
	int content_pos = 0;
	int end_content = 0;
	char *buffer = NULL;

	if (parent) {
		if (parent->type & ROXML_ATTR_NODE) {
			if (((type & ROXML_TXT_NODE) == 0) || (parent->chld))
				return NULL;
		} else if ((parent->type & ROXML_ELM_NODE) == 0) {
			if (parent->prnt && (parent->prnt->type & ROXML_ELM_NODE))
				parent = parent->prnt;
			else
				return NULL;
		}
	}

	if (value)
		content_l = strlen(value);
	if (name)
		name_l = strlen(name);

	if (type & ROXML_ATTR_NODE) {
		int xmlns_l = 0;
		if (!name || !value)
			return NULL;
		if (type & ROXML_NS_NODE) {
			if (name_l > 0)
				xmlns_l = 6;
			else
				xmlns_l = 5;
			buffer = (char *)malloc(sizeof(char) * (name_l + content_l + xmlns_l + 4));
			sprintf(buffer, "xmlns%s%s=\"%s\"", name_l ? ":" : "", name, value);
		} else {
			buffer = (char *)malloc(sizeof(char) * (name_l + content_l + 4));
			sprintf(buffer, "%s=\"%s\"", name, value);
		}
		content_pos = name_l + 2 + xmlns_l;
		end_node = name_l + 1 + xmlns_l;
		end_content = name_l + content_l + 2 + xmlns_l;
	} else if (type & ROXML_CMT_NODE) {
		if (!value)
			return NULL;
		buffer = (char *)malloc(sizeof(char) * (content_l + 8));
		sprintf(buffer, "<!--%s-->", value);
		content_pos = 0;
		end_node = content_l + 4;
		end_content = content_l + 4;
	} else if (type & ROXML_PI_NODE) {
		if (!name)
			return NULL;
		if (content_l) {
			buffer = (char *)malloc(sizeof(char) * (name_l + content_l + 8));
			sprintf(buffer, "<?%s %s?>", name, value);
			end_node = name_l + content_l + 3;
			end_content = name_l + content_l + 5;
		} else {
			buffer = (char *)malloc(sizeof(char) * (name_l + 7));
			sprintf(buffer, "<?%s?>", name);
			end_node = name_l + 2;
			end_content = name_l + 4;
		}
		content_pos = 0;
	} else if (type & ROXML_TXT_NODE) {
		if (!value)
			return NULL;
		buffer = (char *)malloc(sizeof(char) * (content_l + 1));
		sprintf(buffer, "%s", value);
		content_pos = 0;
		end_node = content_l + 1;
		end_content = content_l + 1;
	} else if (type & ROXML_ELM_NODE) {
		if (!name)
			return NULL;
		if (value) {
			buffer = (char *)malloc(sizeof(char) * (name_l * 2 + content_l + 6));
			sprintf(buffer, "<%s>%s</%s>", name, value, name);
			content_pos = name_l + 2;
			end_node = name_l + content_l + 2;
			end_content = end_node;
		} else {
			buffer = (char *)malloc(sizeof(char) * (name_l + 5));
			sprintf(buffer, "<%s />", name);
		}
	} else {
		return NULL;
	}

	new_node = roxml_create_node(0, buffer, type | ROXML_PENDING | ROXML_BUFF);
	new_node->end = end_node;

	if (type & ROXML_NS_NODE) {
		roxml_ns_t *ns = calloc(1, sizeof(roxml_ns_t) + name_l + 1);
		ns->id = ROXML_NS_ID;
		ns->alias = (char *)ns + sizeof(roxml_ns_t);
		if (name)
			strcpy(ns->alias, name);
		new_node->priv = ns;
	}

	if (((type & ROXML_ELM_NODE) && content_l) || (type & ROXML_ATTR_NODE)) {
		node_t *new_txt = roxml_create_node(content_pos, buffer, ROXML_TXT_NODE | ROXML_PENDING | ROXML_BUFF);
		roxml_append_node(new_node, new_txt);
		new_txt->end = end_content;
	}

	if (parent == NULL) {
		xpath_tok_table_t *table = (xpath_tok_table_t *) calloc(1, sizeof(xpath_tok_table_t));
		parent = roxml_create_node(0, NULL, ROXML_ELM_NODE | ROXML_PENDING | ROXML_BUFF);
		parent->end = 1;

		table->id = ROXML_REQTABLE_ID;
		table->ids[ROXML_REQTABLE_ID] = 1;

		pthread_mutex_init(&table->mut, NULL);
		parent->priv = (void *)table;

		roxml_append_node(parent, new_node);
	} else {
		roxml_parent_node(parent, new_node, position);
	}

	return new_node;
}

ROXML_INT void roxml_reset_ns(node_t *n, node_t * ns)
{
	node_t *attr = NULL;
	node_t *chld = NULL;

	if (!n)
		return;

	if (n->ns == ns) {
		if (n->prnt)
			n->ns = n->prnt->ns;
		else
			n->ns = NULL;
	}

	chld = n->chld;
	while (chld) {
		roxml_reset_ns(chld, ns);
		chld = chld->sibl;
	}

	attr = n->attr;
	while (attr) {
		if ((attr->type & ROXML_NS_NODE) == 0)
			if (attr->ns == ns)
				attr->ns = attr->prnt->ns;
		attr = attr->sibl;
	}
}

ROXML_INT void roxml_del_txt_node(node_t *n)
{
        node_t *current = n->prnt->chld;

        while (current && (current->type & ROXML_TXT_NODE) == 0)
                current = current->sibl;

        if (n->prnt && n->prnt->next == n)
                n->prnt->next = roxml_get_real_prev_sibling(n);

        if (current == n) {
                n->prnt->chld = n->sibl;
        } else if (current) {
                while (current->sibl && current->sibl != n)
                        current = current->sibl;
                current->sibl = n->sibl;
        }
}

ROXML_INT void roxml_del_std_node(node_t *n)
{
        node_t *current = n->prnt->chld;
        if (n->prnt && n->prnt->next == n)
                n->prnt->next = roxml_get_real_prev_sibling(n);

        if (current == n) {
                n->prnt->chld = n->sibl;
        } else if (current) {
                while (current->sibl && current->sibl != n)
                        current = current->sibl;
                current->sibl = n->sibl;
        }
        roxml_del_tree(n->chld);
        roxml_del_tree(n->attr);
}

ROXML_INT void roxml_del_arg_node(node_t *n)
{
	node_t *current = n->prnt->attr;

	if (n->type & ROXML_NS_NODE)
		roxml_reset_ns(n->prnt, n);

	if (current == n) {
		n->prnt->attr = n->sibl;
	} else if (current) {
		while (current->sibl && current->sibl != n)
			current = current->sibl;
		current->sibl = n->sibl;
	}
	roxml_del_tree(n->chld);
}

ROXML_API node_t *roxml_set_ns(node_t *n, node_t * ns)
{
	node_t *attr = NULL;
	node_t *chld = NULL;

	if (!n) {
		return NULL;
	}

	if (ns) {
		node_t *common_parent = n;
		while (common_parent && common_parent != ns->prnt) {
			common_parent = common_parent->prnt;
		}
		if (common_parent != ns->prnt) {
			return NULL;
		}
	}

	n->ns = ns;
	chld = n->chld;
	while (chld) {
		roxml_set_ns(chld, ns);
		chld = chld->sibl;
	}

	attr = n->attr;
	while (attr) {
		if ((attr->type & ROXML_NS_NODE) == 0) {
			attr->ns = ns;
		}
		attr = attr->sibl;
	}

	return n;
}
