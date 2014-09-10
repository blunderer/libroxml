#include <string.h>
#include <stdlib.h>
#include <roxml_nav.h>
#include <roxml_edit.h>
#include <roxml_core.h>

ROXML_INT node_t *roxml_prepend_node(node_t *parent, node_t *n)
{
	if (n->type & ROXML_ATTR_NODE) {
		n->sibl = parent->attr;
		parent->attr = n;
	} else {
		n->sibl = parent->chld;
		parent->chld = n;
	}
	return n;
}

ROXML_INT node_t *roxml_parent_node(node_t *parent, node_t * n, int position)
{
        int nb;

        if (n == NULL)
                return NULL;
        if (parent == NULL)
                return n;

        if (n->type & ROXML_ATTR_NODE)
                nb = roxml_get_attr_nb(parent);
        else
                nb = roxml_get_nodes_nb(parent, ROXML_PI_NODE | ROXML_CMT_NODE | ROXML_TXT_NODE | ROXML_ELM_NODE | ROXML_DOCTYPE_NODE);

	roxml_set_parent(parent, n);

        if ((position == 0) || (position > nb)) { 
                return roxml_append_node(parent, n);
	} else if (position == 1) {
		return roxml_prepend_node(parent, n);
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

ROXML_INT void roxml_generate_cmt_node(node_t *n, char *content)
{
	int content_l = strlen(content);

	n->src.buf = malloc(sizeof(char) * (content_l + 8));
	sprintf(n->src.buf, "<!--%s-->", content);
	n->end = content_l + 4;
}

ROXML_INT void roxml_generate_txt_node(node_t *n, char *content)
{
	int content_l = strlen(content);

	n->src.buf = malloc(sizeof(char) * (content_l + 1));
	sprintf(n->src.buf, "%s", content);
	n->end = content_l + 1;
}

ROXML_INT void roxml_generate_elm_node(node_t *n, char *name, char *content)
{
	int content_l = 0;
	int name_l = strlen(name);

	if (content)
		content_l = strlen(content);

	if (content) {
		n->src.buf = malloc(sizeof(char) * (name_l * 2 + content_l + 6));
		sprintf(n->src.buf, "<%s>%s</%s>", name, content, name);
		n->end = name_l + content_l + 2;

		node_t *new_txt = roxml_create_node(name_l + 2, n->src.buf, ROXML_TXT_NODE | ROXML_PENDING | ROXML_BUFF);
		roxml_append_node(n, new_txt);
		new_txt->end = name_l + content_l + 2;
	} else {
		n->src.buf = malloc(sizeof(char) * (name_l + 5));
		sprintf(n->src.buf, "<%s />", name);
		n->end = 0;
	}
}

ROXML_INT void roxml_generate_pi_node(node_t *n, char *name, char *content)
{
	int content_l = 0;
	int name_l = strlen(name);

	if (content)
		content_l = strlen(content);

	if (content_l) {
		n->src.buf = malloc(sizeof(char) * (name_l + content_l + 8));
		sprintf(n->src.buf, "<?%s %s?>", name, content);
		n->end = name_l + content_l + 3;
	} else {
		n->src.buf = malloc(sizeof(char) * (name_l + 7));
		sprintf(n->src.buf, "<?%s?>", name);
		n->end = name_l + 2;
	}
}

ROXML_INT void roxml_generate_attr_node(node_t *n, int type, char *name, char *content)
{
	int xmlns_l = 0;
	int content_l = strlen(content);
	int name_l = strlen(name);

	if (type & ROXML_NS_NODE) {
		xmlns_l = 5;
		if (name_l > 0)
			xmlns_l++;

		n->src.buf = malloc(sizeof(char) * (name_l + content_l + xmlns_l + 4));
		sprintf(n->src.buf, "xmlns%s%s=\"%s\"", name_l ? ":" : "", name, content);

		roxml_ns_t *ns = calloc(1, sizeof(roxml_ns_t) + name_l + 1);
		ns->id = ROXML_NS_ID;
		ns->alias = (char *)ns + sizeof(roxml_ns_t);
		if (name)
			strcpy(ns->alias, name);
		n->priv = ns;
	} else {
		n->src.buf = malloc(sizeof(char) * (name_l + content_l + 4));
		sprintf(n->src.buf, "%s=\"%s\"", name, content);
	}

	node_t *new_txt = roxml_create_node(name_l + 2 + xmlns_l, n->src.buf, ROXML_TXT_NODE | ROXML_PENDING | ROXML_BUFF);
	new_txt->end = name_l + content_l + 2 + xmlns_l;
	n->end = name_l + 1 + xmlns_l;
	roxml_append_node(n, new_txt);
}

ROXML_API int roxml_add_node_check(node_t *parent, int type, char *name, char *content)
{
	int valid = 1;

	if (parent) {
		if (parent->type & ROXML_ATTR_NODE) {
			if (((type & ROXML_TXT_NODE) == 0) || (parent->chld))
				valid = 0;
		} else if ((parent->type & ROXML_ELM_NODE) == 0) {
			 if (parent->prnt && (parent->prnt->type & ROXML_ELM_NODE))
				 valid = 2;
			 else
				 valid = 0;
		}
	}

	switch (type) {
	case ROXML_ATTR_NODE | ROXML_NS_NODE:
	case ROXML_ATTR_NODE:
		if (!name || !content)
			valid = 0;
		break;
	case ROXML_TXT_NODE:
	case ROXML_CMT_NODE:
		if (!content)
			valid = 0;
		break;
	case ROXML_ELM_NODE:
	case ROXML_PI_NODE:
		if (!name)
			valid = 0;
		break;
	default:
		valid = 0;
	}

	return valid;
}

ROXML_API node_t *roxml_add_node(node_t *parent, int position, int type, char *name, char *content)
{
	int ret;
	node_t *new_node = NULL;

	ret = roxml_add_node_check(parent, type, name, content);
	if (ret == 0)
		return NULL;
	else if (ret == 2)
		parent = parent->prnt;

	if (parent == NULL) {
		node_t *n = roxml_create_node(0, NULL, ROXML_ELM_NODE | ROXML_PENDING | ROXML_BUFF);
		parent = roxml_create_root(n);
	}

	new_node = roxml_create_node(0, NULL, type | ROXML_PENDING | ROXML_BUFF);

	if (type & ROXML_ATTR_NODE)
		roxml_generate_attr_node(new_node, type, name, content);
	else if (type & ROXML_CMT_NODE)
		roxml_generate_cmt_node(new_node, content);
	else if (type & ROXML_PI_NODE)
		roxml_generate_pi_node(new_node, name, content);
	else if (type & ROXML_TXT_NODE)
		roxml_generate_txt_node(new_node, content);
	else if (type & ROXML_ELM_NODE)
		roxml_generate_elm_node(new_node, name, content);

	return roxml_parent_node(parent, new_node, position);
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

	roxml_del_std_node(current);
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

	if (!n)
		return NULL;

	if (ns) {
		node_t *common_parent = n;
		while (common_parent && common_parent != ns->prnt)
			common_parent = common_parent->prnt;
		if (common_parent != ns->prnt)
			return NULL;
	}

	n->ns = ns;
	chld = n->chld;
	while (chld) {
		roxml_set_ns(chld, ns);
		chld = chld->sibl;
	}

	attr = n->attr;
	while (attr) {
		if ((attr->type & ROXML_NS_NODE) == 0)
			attr->ns = ns;
		attr = attr->sibl;
	}

	return n;
}
