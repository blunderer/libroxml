#ifndef ROXML_NAV_H
#define ROXML_NAV_H

#include <roxml_internal.h>

/** \brief get rela sibling
 *
 * \fn node_t *roxml_get_real_prev_sibling(node_t *n);
 * This function returns the real previous sibling of a node
 * no matter what its type is.
 * \param n is one node of the tree
 * \return the previous sibling
 */
ROXML_INT node_t *roxml_get_real_prev_sibling(node_t *n);

#endif /* ROXML_NAV_H */
