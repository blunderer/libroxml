#ifndef ROXML_EDIT_H
#define ROXML_EDIT_H

#include <roxml_internal.h>

/** \brief node relocate function
 *
 * \fn roxml_parent_node(node_t *parent, node_t *n, int position);
 * this function change the position of a node in its parent list
 * \param parent the parent node
 * \param n the node to parent
 * \param position the position, 0 means or > nb children means at the end
 * \return
 */
ROXML_INT node_t *roxml_parent_node(node_t *parent, node_t * n, int position);

/** \brief internal function
 *
 * \fn void ROXML_INT roxml_reset_ns(node_t *n, node_t *ns);
 * This function remove the namespace of a node if it is the one
 * specified as argument, and replace it with its parent one
 * \param n is one node of the tree
 * \param ns is one nsdef of the tree
 * \return void
 */
ROXML_INT void roxml_reset_ns(node_t *n, node_t * ns);

/** \brief text node deletion function
 *
 * \fn roxml_del_txt_node(node_t *n);
 * this function delete a text node
 * \param n the node to delete
 * \return
 */
ROXML_INT void roxml_del_txt_node(node_t *n);

/** \brief node deletion function
 *
 * \fn roxml_del_std_node(node_t *n);
 * this function delete a standard node
 * \param n the node to delete
 * \return
 */
ROXML_INT void roxml_del_std_node(node_t *n);

/** \brief attribute node deletion function
 *
 * \fn roxml_del_arg_node(node_t *n);
 * this function delete an attribute node
 * \param n the node to delete
 * \return
 */
ROXML_INT void roxml_del_arg_node(node_t *n);

#endif /* ROXML_EDIT_H */
