/** \file roxml-doxy.h
 *  \brief doxygen source documentation for libroxml.so
 *
 * This is the header file used to develop some
 * softwares using the libroxml.so library.
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

/** \mainpage libroxml homepage
 *
 * \section intro_sec <introduction>
 * This library is minimum, easy-to-use, C implementation for xml file parsing
 * It includes:
 * <ul>
 * <li>the library \b libroxml for xml parsing inside applications.</li>
 * <li>the binary \b roxml, an xpath resolver that can be used from shell.</li>
 * <li>the module \b fuse.xml that can be used to mount an XML file as a filesystem.</li>
 * <li>the full \ref public-api and \ref priv-api documentation.</li>
 * </ul>
 *
 * The stripped binary is about 50K. The public API defines less than 30 functions to do all you need, making it very easy to start with.
 * For bug reporting, you can check the google code dedicated project: http://libroxml.googlecode.com
 *
 * Libroxml is distributed under the terms of the <a href="http://www.gnu.org/licenses/lgpl.html">GNU Lesser General Public License</a>.
 * 
 * \section intro_end </introduction>
 * \section dl_sec <downloads>
 * The source code is available from google project SVN repository: 
 * \verbatim $> git clone https://code.google.com/p/libroxml \endverbatim
 *
 * You can download source packages:
 * <ul>
 * <li>Lastest version is 2.2.2: <a href="http://www.libroxml.com/downloads/libroxml-2.2.2.tar.gz">download now</a></li>
 * <li>Go to the download section if you need older libroxml revision: http://www.libroxml/downloads</li>
 * </ul>
 * 
 * \section dl_end </downloads>
 * \section what_sec <why libroxml>
 * Because XML parsing is always hard to reinvent, and because very often xml lib are 
 * too big to fit with very little application. Libroxml target mainly embedded software and environment, but you can use it whenever you need to deal with XML since libroxml is ligth and fast. 
 * libroxml is now provided in buildroot (http://buildroot.uclibc.org).
 *
 * libroxml allow you to easily:
 * <ul>
 * <li> load / unload XML document from buffers or files.</li>
 * <li> navigate thru an xml tree using simple getter API.</li>
 * <li> handle namespace.</li>
 * <li> use xpath syntax to access some nodes in the XML tree.</li>
 * <li> read nodes contents (text, attributes, comments ...)</li>
 * <li> create/modify xml trees and save them to a file or buffer.</li>
 *</ul>
 *
 * \note libroxml work with both strict XML documents but also with xml like formatted documents (without any <?xml?> definition...)
 * \warning libroxml may behave strangely when dealing with node names or attribute values bigger than 512 bytes. FOr those who really need it, they can increase this limit by modifying the ROXML_LONG_LEN define in roxml-defines.h.
 * \warning libroxml do not handle DOCTYPE nodes. However, it will nicely ignore them during parsing and will still return the xml tree.
 * \warning libroxml is not thread safe for all write operations (roxml_set_ns, roxml_add_node, roxml_del_node) therefore, those functions must be granted exclusiv access to the xml tree when called. On the other hand, all other functions can be safely called simultaneously.
 *
 * \section what_end </why libroxml>
 * \section how_sec <how does it work>
 * You can refer to \ref public-api for documentation on all functions
 *
 * there are several groups of functions in public API: 
 * <ul>
 * <li> Load and close XML data from different sources.</li>
 * <li> Get nodes and navigate into the loaded xml tree.</li>
 * <li> Get the xml content of nodes, attributes...</li>
 * <li> Create and delete nodes, export tree.</li>
 * <li> Some libroxml specific functions for memory handling</li>
 * </ul>
 * \section how_end </how does it work>
 */

/** \page public-api public API
 *
 * <div class="api">
 * <table>
 * <tr>
 * <td>
 * <h2>Manage xml source</h2>
 * <ul>
 * <li>\ref roxml_load_fd</li>
 * <li>\ref roxml_load_doc</li>
 * <li>\ref roxml_load_buf</li>
 * <li>\ref roxml_close</li>
 * </ul>
 * 
 * <h2>Access xml content</h2>
 * <ul>
 * <li>\ref roxml_get_type</li>
 * <li>\ref roxml_get_node_position</li>
 * <li>\ref roxml_get_name</li>
 * <li>\ref roxml_get_content</li>
 * </ul>
 * </td>
 * <td>
 * <h2>Navigate into xml tree</h2>
 * <ul>
 * <li>\ref roxml_get_root</li>
 * <li>\ref roxml_get_parent</li>
 * <li>\ref roxml_get_prev_sibling</li>
 * <li>\ref roxml_get_next_sibling</li>
 * <li>\ref roxml_get_ns</li>
 * <li>\ref roxml_get_chld</li>
 * <li>\ref roxml_get_chld_nb</li>
 * <li>\ref roxml_get_attr</li>
 * <li>\ref roxml_get_attr_nb</li>
 * <li>\ref roxml_get_txt</li>
 * <li>\ref roxml_get_txt_nb</li>
 * <li>\ref roxml_get_pi</li>
 * <li>\ref roxml_get_pi_nb</li>
 * <li>\ref roxml_get_cmt</li>
 * <li>\ref roxml_get_cmt_nb</li>
 * <li>\ref roxml_get_nodes</li>
 * <li>\ref roxml_get_nodes_nb</li>
 * <li>\ref roxml_xpath</li>
 * </ul>
 * </td>
 * <td>
 * <h2>Modify xml tree</h2>
 * <ul>
 * <li>\ref roxml_set_ns</li>
 * <li>\ref roxml_add_node</li>
 * <li>\ref roxml_del_node</li>
 * <li>\ref roxml_commit_changes</li>
 * </ul>
 *
 * <h2>Libroxml specifics</h2>
 * <ul>
 * <li>\ref roxml_release</li>
 * </ul>
 * </td>
 * </tr>
 * </table>
 * </div>
 */

/** \page priv-api private API
 * This is the documentation for private API. Users of libroxml should not need to go there. However, if you need to hack into library, understand how things are done, or correct a bug, feel free to dive in...
 * <div class="api">
 * <table>
 * <tr>
 * <td>
 * <h2>low level node handling</h2>
 * <ul>
 * <li>\ref roxml_free_node</li>
 * <li>\ref roxml_create_node</li>
 * <li>\ref roxml_del_tree</li>
 * <li>\ref roxml_close_node</li>
 * <li>\ref roxml_load</li>
 * <li>\ref roxml_parent_node</li>
 * <li>\ref roxml_process_begin_node</li>
 * </ul>
 *
 * <h2>misc functions</h2>
 * <ul>
 * <li>\ref roxml_malloc</li>
 * <li>\ref roxml_read</li>
 * <li>\ref roxml_is_separator</li>
 * </ul>
 * </td>
 * <td>
 * <h2>low level xpath functions</h2>
 * <ul>
 * <li>\ref roxml_set_axes</li>
 * <li>\ref roxml_parse_xpath</li>
 * <li>\ref roxml_free_xcond</li>
 * <li>\ref roxml_free_xpath</li>
 * <li>\ref roxml_double_cmp</li>
 * <li>\ref roxml_double_oper</li>
 * <li>\ref roxml_validate_predicat</li>
 * <li>\ref roxml_validate_axes</li>
 * <li>\ref roxml_check_node</li>
 * <li>\ref roxml_compute_and</li>
 * <li>\ref roxml_compute_or</li>
 * <li>\ref roxml_exec_xpath</li>
 * </ul>
 * </td>
 * <td>
 * <h2>node set handling</h2>
 * <ul>
 * <li>\ref roxml_request_id</li>
 * <li>\ref roxml_release_id</li>
 * <li>\ref roxml_add_to_pool</li>
 * <li>\ref roxml_del_from_pool</li>
 * <li>\ref roxml_in_pool</li>
 * </ul>
 *
 * <h2>modifications handling</h2>
 * <ul>
 * <li>\ref roxml_print_space</li>
 * <li>\ref roxml_write_string</li>
 * <li>\ref roxml_write_node</li>
 * <li>\ref roxml_del_arg_node</li>
 * <li>\ref roxml_del_txt_node</li>
 * <li>\ref roxml_del_std_node</li>
 * <li>\ref roxml_set_type</li>
 * <li>\ref roxml_get_node_internal_position</li>
 * </ul>
 * </td>
 * </tr>
 * </table>
 * </div>
 */

/** \page xpath xpath handling
 * <div class="xpath">
 * \section rel-xpath Relative xpath
 * <table border="0">
 * <tr><td> starting point from any given node</td><td><i>n1/n2/n3</i></td></tr>
 * </table border="0">
 *
 * \section abs-xpath Absolute xpath
 * <table border="0">
 * <tr><td> starting point form root even if current node is different</td><td><i>/n0/n1/n2/n3</i></td></tr>
 * </table border="0">
 *
 * \section spec-nodes Special Nodes
 * <table border="0">
 * <tr><td>access all nodes</td><td><i>*</i></td></tr>
 * <tr><td>access only element nodes</td><td><i>node()</i></td></tr>
 * <tr><td>access only text nodes</td><td>text()</td></tr>
 * <tr><td>access only comment nodes</td><td><i>comment()</i></td></tr>
 * <tr><td>access only processing-instruction nodes</td><td><i>processing-instruction()</i></td></tr>
 * </table border="0">
 *
 * \section cond Conditions
 * <table border="0">
 * <tr><td>Attribute string value</td><td><i>/n0[@a=value]</i></td></tr>
 * <tr><td>Attribute int value</td><td><i>/n0[\@a=value]</i></td></tr>
 * <tr><td>Attribute int value</td><td><i>/n0[\@a!=value]</i></td></tr>
 * <tr><td>Attribute int value</td><td><i>/n0[\@a<value]</i></td></tr>
 * <tr><td>Attribute int value</td><td><i>/n0[\@a>value]</i></td></tr>
 * <tr><td>Attribute int value</td><td><i>/n0[\@a<=value]</i></td></tr>
 * <tr><td>Attribute int value</td><td><i>/n0[\@a>=value]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[first()]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[first()+2]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[last()]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[last()-3]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[position() = 0]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[position() != 0]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[position() > 1]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[position() < 2]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[position() >= 1]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[position() <= 2]</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[position() <= 2]/n1</i></td></tr>
 * <tr><td>Node position</td><td><i>/n0[2]</i></td></tr>
 * <tr><td>Node with namespace</td><td><i>//*[namespace-uri() = 'http://test.com']</i></td></tr>
 * <tr><td>Other xpath</td><td><i>/n0[n1/\@attr]</i></td></tr>
 * </table border="0">
 *
 * \section short-xpath Shorten xpath
 * <table border="0">
 * <tr><td>Children</td><td><i>/n0/n1/n2</i></td></tr>
 * <tr><td>Attributes</td><td><i>/n0/n1/\@a</i></td></tr>
 * <tr><td>Descendent or self::<i>node()</td><td>/n0//n3</i></td></tr>
 * <tr><td>Parent</td><td><i>/n0/n1/../n1/n2</i></td></tr>
 * <tr><td>Self</td><td><i>/n0/n1/./n2</i></td></tr>
 * <tr><td>Node in namespaces</td><td><i>/test::n0</i></td></tr>
 * </table border="0">
 *
 * \section full-xpath Full xpath
 * <table border="0">
 * <tr><td>Nodes</td><td><i>/n0/n1/child::a</i></td></tr>
 * <tr><td>Attributes</td><td><i>/n0/n1/attribute::a</i></td></tr>
 * <tr><td>Descendent or self</td><td><i>/n0/descendant-or-self::n5</i></td></tr>
 * <tr><td>Parent</td><td><i>/child::n0/child::n1/parent::/n1</i></td></tr>
 * <tr><td>Self</td><td><i>/child::n0/child::n1/self::</i></td></tr>
 * <tr><td>Preceding</td><td><i>preceding::n1</i></td></tr>
 * <tr><td>Following</td><td><i>following::n1</i></td></tr>
 * <tr><td>Ancestor</td><td><i>ancestor::n2</i></td></tr>
 * <tr><td>Ancestor-or-self</td><td><i>ancestor-or-self::n2</i></td></tr>
 * <tr><td>Namespace</td><td><i>namespace::test</i></td></tr>
 * </table border="0">
 * </div>
 */
