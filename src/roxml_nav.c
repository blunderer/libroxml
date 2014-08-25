#include <roxml_nav.h>

ROXML_API node_t *roxml_get_prev_sibling(node_t *n)
{
	node_t *prev = NULL;
	node_t *prev_elm = NULL;

	if (n && n->prnt) {
		prev = n->prnt->chld;
		while (prev && prev != n) {
			if ((prev->type & ROXML_NODE_TYPES) == ROXML_ELM_NODE) {
				prev_elm = prev;
			}
			prev = prev->sibl;
		}
	}
	return prev_elm;
}

ROXML_API node_t *roxml_get_next_sibling(node_t *n)
{
	if (n) {
		while (n->sibl && (n->sibl->type & ROXML_NODE_TYPES) != ROXML_ELM_NODE) {
			n = n->sibl;
		}
		return n->sibl;
	}
	return NULL;
}

ROXML_API node_t *roxml_get_parent(node_t *n)
{
	if (n) {
		if (n->prnt == NULL) {
			return n;
		} else {
			return n->prnt;
		}
	}
	return NULL;
}

ROXML_API node_t *roxml_get_root(node_t *n)
{
	node_t *root = NULL;
	if (n) {
		root = n;

		while (root->prnt)
			root = root->prnt;
	}
	return root;
}

/* do not call this function if n->prnt == NULL */
ROXML_INT node_t *roxml_get_real_prev_sibling(node_t *n)
{
	node_t *current = n->prnt->chld;
	node_t *prev = NULL;

	while (current != n) {
		prev = current;
		current = current->sibl;
	}

	return prev;
}
