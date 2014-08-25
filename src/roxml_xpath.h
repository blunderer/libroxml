#ifndef ROXML_XPATH_H
#define ROXML_XPATH_H

#include <roxml_internal.h>

/** \brief number tester
 *
 * \fn int roxml_is_number(char *input);
 * This function tells if a string is a number
 * \param input string to test
 * \return 1 if the string was a number else 0
 */
ROXML_INT int roxml_is_number(char *input);

/** \brief axes setter function
 *
 * \fn roxml_set_axes(xpath_node_t *node, char *axes, int *offset);
 * this function set the axe to a xpath node from xpath string
 * \param node the xpath node to fill
 * \param axes the string where axe is extracted from
 * \param offset the detected offset in axe string
 * \return the filled xpath_node
 */
ROXML_INT xpath_node_t *roxml_set_axes(xpath_node_t *node, char *axes, int *offset);

/** \brief node absolute position get
 *
 * \fn roxml_get_node_internal_position(node_t *n);
 * this function returns the absolute position of the node between siblings
 * \param n the node
 * \return the absolute position starting at 1
 */
ROXML_INT int roxml_get_node_internal_position(node_t *n);

/** \brief xpath parsing function
 *
 * \fn roxml_parse_xpath(char *path, xpath_node_t **xpath, int context);
 * this function convert an xpath string to a table of list of xpath_node_t
 * \param path the xpath string
 * \param xpath the parsed xpath
 * \param context 0 for a real xpath, 1 for a xpath in predicat
 * \return the number of xpath list in the table
 */
ROXML_INT int roxml_parse_xpath(char *path, xpath_node_t **xpath, int context);

/** \brief xpath condition free function
 *
 * \fn roxml_free_xcond(xpath_cond_t *xcond);
 * this function frees an xpath_cond_t cell
 * \param xcond the xcond to free
 * \return
 */
ROXML_INT void roxml_free_xcond(xpath_cond_t * xcond);

/** \brief xpath free function
 *
 * \fn roxml_free_xpath(xpath_node_t *xpath, int nb);
 * this function frees the parsed xpath structure
 * \param xpath the xpath to free
 * \param nb the number of xpath structures in the table
 * \return
 */
ROXML_INT void roxml_free_xpath(xpath_node_t *xpath, int nb);

/** \brief double operation function
 *
 * \fn roxml_double_oper(double a, double b, int op);
 * this function  compare two doubles using one defined operator
 * \param a first operand
 * \param b second operand
 * \param op the operator to use
 * \return 1 if comparison is ok, esle 0
 */
ROXML_INT double roxml_double_oper(double a, double b, int op);

/** \brief  double comparison function
 *
 * \fn roxml_double_cmp(double a, double b, int op);
 * this function  compare two doubles using one defined operator
 * \param a first operand
 * \param b second operand
 * \param op the operator to use
 * \return 1 if comparison is ok, esle 0
 */
ROXML_INT int roxml_double_cmp(double a, double b, int op);

/** \brief  string comparison function
 *
 * \fn roxml_string_cmp(char *sa, char *sb, int op);
 * this function compare two strings using one defined operator
 * \param sa first operand
 * \param sb second operand
 * \param op the operator to use
 * \return 1 if comparison is ok, else 0
 */
ROXML_INT int roxml_string_cmp(char *sa, char *sb, int op);

/** \brief predicat validation function
 *
 * \fn roxml_validate_predicat(xpath_node_t *xn, node_t *candidat);
 * this function check for predicat validity. predicat is part between brackets
 * \param xn the xpath node containing the predicat to test
 * \param candidat the node to test
 * \return 1 if predicat is validated, else 0
 */
ROXML_INT int roxml_validate_predicat(xpath_node_t *xn, node_t * candidat);

/** \brief id reservation function
 *
 * \fn roxml_request_id(node_t *root);
 * this function request an available id for a new xpath search on the tree
 * \param root the root of the tree where id table is stored
 * \return the id between 1 - 255 or -1 if failed
 */
ROXML_INT int roxml_request_id(node_t *root);

/** \brief node pool presence tester function
 *
 * \fn roxml_in_pool(node_t *root, node_t *n, int req_id);
 * this function test is a node is in a pool
 * \param root the root of the tree
 * \param n the node to test
 * \param req_id the pool id
 * \return
 */
ROXML_INT int roxml_in_pool(node_t *root, node_t * n, int req_id);

/** \brief release id function
 *
 * \fn roxml_release_id(node_t *root, node_t **pool, int pool_len, int req_id);
 * this function release a previously required id and remove all id token from the pool
 * \param root the root of the tree that was used for id request
 * \param pool the result from xpath search using this id
 * \param pool_len the number of node in pool
 * \param req_id the id to release
 * \return
 */
ROXML_INT void roxml_release_id(node_t *root, node_t **pool, int pool_len, int req_id);

/** \brief pool node delete function
 *
 * \fn roxml_del_from_pool(node_t *root, node_t *n, int req_id);
 * this function remove one node from a pool
 * \param root the root of the tree
 * \param n the node to remove
 * \param req_id the pool id
 * \return
 */
ROXML_INT void roxml_del_from_pool(node_t *root, node_t * n, int req_id);

/** \brief add a token top node function
 *
 * \fn roxml_add_to_pool(node_t *root, node_t *n, int req_id);
 * this function add a token to target node. This token is used to garanty
 * unicity in xpath results
 * \param root the root node
 * \param n the node to mark
 * \param req_id the id to use
 * \return 0 if already in the pool, else 1
 */
ROXML_INT int roxml_add_to_pool(node_t *root, node_t * n, int req_id);

/** \brief axe validation function
 *
 * \fn roxml_validate_axes(node_t *root, node_t *candidat, node_t ***ans, int *nb, int *max, xpath_node_t *xn, int req_id);
 * this function validate if an axe is matching the current node
 * \param root the root node
 * \param candidat the node to test
 * \param ans the pointer to answers pool
 * \param nb the number of answers in pool
 * \param max the current size of the pool
 * \param xn the xpath node to test
 * \param req_id the pool id
 * \return 1 if axe is validated, else 0
 */
ROXML_INT int roxml_validate_axes(node_t *root, node_t * candidat, node_t *** ans, int *nb, int *max, xpath_node_t *xn, int req_id);

/** \brief real xpath validation function
 *
 * \fn roxml_check_node(xpath_node_t *xp, node_t *root, node_t *context, node_t ***ans, int *nb, int *max, int ignore, int req_id);
 * this function perform the xpath test on a tree
 * \param xp the xpath nodes to test
 * \param root the root node
 * \param context the current context node
 * \param ans the pointer to answers pool
 * \param nb the number of answers in pool
 * \param max the current size of the pool
 * \param ignore a flag for some axes that are going thru all document
 * \param req_id the pool id
 * \return
 */
ROXML_INT void roxml_check_node(xpath_node_t *xp, node_t * root, node_t * context, node_t *** ans, int *nb, int *max, int ignore, int req_id);

/** \brief node set and function
 *
 * \fn roxml_compute_and(node_t *root, node_t **node_set, int *count, int cur_req_id, int prev_req_id);
 * this function computes the AND of two node pools. The resulting pool will have the same ID as cur_req_id.
 * \param root the root of the tree
 * \param node_set the node set containing the 2 pools
 * \param count number of node in the node set
 * \param cur_req_id the id of the first group
 * \param prev_req_id the id of the second group
 */
ROXML_INT void roxml_compute_and(node_t *root, node_t **node_set, int *count, int cur_req_id, int prev_req_id);

/** \brief node set or function
 *
 * \fn roxml_compute_or(node_t *root, node_t **node_set, int *count, int req_id, int glob_id);
 * this function computes the OR of two node pools. The resulting pool will have the same ID as glob_id.
 * \param root the root of the tree
 * \param node_set the node set containing the 2 pools
 * \param count number of node in the node set
 * \param req_id the id of the first group
 * \param glob_id the id of the second group
 * \return
 */
ROXML_INT void roxml_compute_or(node_t *root, node_t **node_set, int *count, int req_id, int glob_id);

/** \brief real xpath execution
 *
 * \fn roxml_exec_xpath(node_t *root, node_t *n, xpath_node_t *xpath, int index, int * count);
 * this function exec a decoded xpath strcuture
 * \param root the root of the tree
 * \param n the context node
 * \param xpath the xpath structure
 * \param index the number of xpath condition in string
 * \param count the pointer to a variable that is filled with the resulting node number
 * \return  the resulting node set that have to be freed with roxml_release
 */
ROXML_INT node_t **roxml_exec_xpath(node_t *root, node_t * n, xpath_node_t * xpath, int index, int *count);

ROXML_INT int _func_xpath_ignore(char *chunk, void *data);
ROXML_INT int _func_xpath_new_node(char *chunk, void *data);
ROXML_INT int _func_xpath_quote(char *chunk, void *data);
ROXML_INT int _func_xpath_dquote(char *chunk, void *data);
ROXML_INT int _func_xpath_open_parenthesys(char *chunk, void *data);
ROXML_INT int _func_xpath_close_parenthesys(char *chunk, void *data);
ROXML_INT int _func_xpath_open_brackets(char *chunk, void *data);
ROXML_INT int _func_xpath_close_brackets(char *chunk, void *data);
ROXML_INT int _func_xpath_condition_or(char *chunk, void *data);
ROXML_INT int _func_xpath_condition_and(char *chunk, void *data);
ROXML_INT int _func_xpath_path_or(char *chunk, void *data);
ROXML_INT int _func_xpath_operators(char *chunk, void *data, int operator, int operator_bis);
ROXML_INT int _func_xpath_operator_equal(char *chunk, void *data);
ROXML_INT int _func_xpath_operator_sup(char *chunk, void *data);
ROXML_INT int _func_xpath_operator_inf(char *chunk, void *data);
ROXML_INT int _func_xpath_operator_diff(char *chunk, void *data);
ROXML_INT int _func_xpath_number(char *chunk, void *data);
ROXML_INT int _func_xpath_funcs(char *chunk, void *data, int func, char *name);
ROXML_INT int _func_xpath_position(char *chunk, void *data);
ROXML_INT int _func_xpath_first(char *chunk, void *data);
ROXML_INT int _func_xpath_last(char *chunk, void *data);
ROXML_INT int _func_xpath_nsuri(char *chunk, void *data);
ROXML_INT int _func_xpath_lname(char *chunk, void *data);
ROXML_INT int _func_xpath_operator_add(char *chunk, void *data);
ROXML_INT int _func_xpath_operator_subs(char *chunk, void *data);
ROXML_INT int _func_xpath_default(char *chunk, void *data);

#endif /* ROXML_XPATH_H */
