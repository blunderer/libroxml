# Introduction #

You can find all this API in doxygen format using "make doxy" command

doxygen documentation is also availble at http://roxml.blunderer.org

# Availabe functions #

  * void _ROXML\_API_ roxml\_release (void `*`data)
  * char `*`_ROXML\_API_ roxml\_get\_content (node\_t `*`n, char `*`name, int bufsize, int `*`size)
  * char `*`_ROXML\_API_ roxml\_get\_name (node\_t `*`n, char `*`name, int size)
  * int _ROXML\_API_ roxml\_get\_text\_nb (node\_t `*`n)
  * node\_t `*`_ROXML\_API_ roxml\_get\_text (node\_t `*`n, int nth)
  * int _ROXML\_API_ roxml\_get\_attr\_nb (node\_t `*`n)
  * node\_t `*`_ROXML\_API_ roxml\_get\_attr (node\_t `*`n, char `*`name, int nth)
  * void _ROXML\_API_ roxml\_close (node\_t `*`n)
  * int _ROXML\_API_ roxml\_get\_chld\_nb (node\_t `*`n)
  * node\_t `*`_ROXML\_API_ roxml\_get\_chld (node\_t `*`n, char `*`name, int nth)
  * node\_t `*`_ROXML\_API_ roxml\_get\_parent (node\_t `*`n)
  * int _ROXML\_API_ roxml\_get\_type (node\_t `*`n)
  * int _ROXML\_API_ roxml\_get\_node\_position (node\_t `*`n, int `*`last)
  * node\_t `*`_ROXML\_API_ roxml\_load\_doc (char `*`filename)
  * node\_t `*`_ROXML\_API_ roxml\_load\_buf (char `*`buffer)
  * node\_t `*``*`_ROXML\_API_ roxml\_xpath (node\_t `*`n, char `*`path, int `*`nb\_ans)
  * void _ROXML\_API_ roxml\_del\_node (node\_t `*`n)
  * void _ROXML\_API_ roxml\_commit\_changes (node\_t `*`n, char `*`dest, char `*``*`buffer, int human)
  * node\_t `*` _ROXML\_API_ roxml\_add\_node (node\_t `*`parent, int position, int type, char `*`name, char `*`value)

# Details #

### void _ROXML\_API_ roxml\_release (void `*`data) ###

**memory cleanning function**

This function release the memory pointed by pointer just like free would but for memory allocated with roxml\_malloc. Freeing a NULL pointer won't do anything. roxml\_release also allow you to remove all previously allocated datas by using RELEASE\_ALL as argument. You can also safely use RELEASE\_LAST argument that will release the previously allocated var within the current thread (making this function thread safe). if using roxml\_release on a variable non roxml\_mallocated, nothing will happen

_Definition at line 27 of file roxml.c._

---

### char `*`_ROXML\_API_ roxml\_get\_content (node\_t `*`n, char `*`name, int bufsize, int `*`size) ###

**content getter function**

This function returns a pointer with text content of a node (chld are NOT included as text).; if the returned pointer is NULL then the node was empty. returned string should be roxml\_release when not used anymore

_Definition at line 104 of file roxml.c._

---

### char `*`_ROXML\_API_ roxml\_get\_name (node\_t `*`n, char `*`name, int size) ###

**name getter function**

This function return the name of the node or fill the current buffer with it if not NULL. if name is NULL, the function will allocated a buffer that user should roxml\_release when no longer needed. Be carreful as if your buffer is too short for the returned string, it will be truncated

_Definition at line 170 of file roxml.c._

---

### int _ROXML\_API_ roxml\_get\_text\_nb (node\_t `*`n) ###

**text node number getter function**

this function return the number of text nodes in a standard node

_Definition at line 232 of file roxml.c._

---

### node\_t `*`_ROXML\_API_ roxml\_get\_text (node\_t `*`n, int nth) ###

**text content getter function**

this function return the text content of a node as a TEXT node

_Definition at line 247 of file roxml.c._

---

### int _ROXML\_API_ roxml\_get\_attr\_nb (node\_t `*`n) ###

**number of attribute getter function**

This function returns the number of attributes for a given node

_Definition at line 268 of file roxml.c._

---

### node\_t `*`_ROXML\_API_ roxml\_get\_attr (node\_t `*`n, char `*`name, int nth) ###

**attribute getter function**

This function get the nth attribute of a node. User should roxml\_release the returned buffer when no longer needed.

_Definition at line 283 of file roxml.c._

---

### void _ROXML\_API_ roxml\_close (node\_t `*`n) ###

**unload function**

This function clear a document and all the corresponding nodes

_Definition at line 315 of file roxml.c._

---

### int _ROXML\_API_ roxml\_get\_chld\_nb (node\_t `*`n) ###

**chlds number getter function**

This function return the number of chlds for a given node

_Definition at line 335 of file roxml.c._

---

### node\_t `*`_ROXML\_API_ roxml\_get\_chld (node\_t `*`n, char `*`name, int nth) ###

**chld getter function**

This function returns a given chld of a node

_Definition at line 351 of file roxml.c._

---

### node\_t `*`_ROXML\_API_ roxml\_get\_parent (node\_t `*`n) ###

**parent getter function**

This function returns the parent of a given node the node parent

_Definition at line 378 of file roxml.c._

---

### int _ROXML\_API_ roxml\_get\_type (node\_t `*`n) ###

**node type function**

This function tells if a node is a arg or real node.

_Definition at line 388 of file roxml.c._

---

### int _ROXML\_API_ roxml\_get\_node\_position (node\_t `*`n) ###

**node get position function**

This function tells the index of a node between all its homonyns.

_Definition at line 393 of file roxml.c._

---

### node\_t `*`_ROXML\_API_ roxml\_load\_doc (char `*`filename) ###

**load function for files**

This function load a document and all the corresponding nodes

_Definition at line 417 of file roxml.c._

---

### node\_t `*`_ROXML\_API_ roxml\_load\_buf (char `*`buffer) ###

**load function for buffers**

This function load a document and all the corresponding nodes

_Definition at line 429 of file roxml.c._

---

### node\_t `*``*`_ROXML\_API_ roxml\_xpath (node\_t `*`n, char `*`path, int `*`nb\_ans) ###

**exec path function**

This function return a node corresponding to a given path. path syntax is : if path begin with a "/" it is an absolute path relative to root else it is a path relative to given node handled XPath are : "/", "..", "@<attr name>", "<node name>[idx](table.md)..." resulting node table should be roxml\_release when not used anymore

_Definition at line 438 of file roxml.c._

---

### void _ROXML\_API_ roxml\_del\_node (node\_t `*`n) ###

**node deletion function**

this function delete a node from the tree. The node is not really deleted from the file or buffer until the roxml\_commit\_changes is called.

_Definition at line 490 of file roxml.c._

---

### void _ROXML\_API_ roxml\_commit\_changes (node\_t `*`n, char `*`dest, char `*``*`buffer, int human) ###

**sync function**

this function sync changes to the given buffer or file in human or one-line format

_Definition at line 503 of file roxml.c._

---

### node\_t `*` _ROXML\_API_ roxml\_add\_node (node\_t `*`parent, int position, int type, char `*`name, char `*`value) ###

**add a node to the tree**

this function add a new node to the tree. This will not update de buffer or file, only the RAM loaded tree

_Definition at line 523 of file roxml.c._

---