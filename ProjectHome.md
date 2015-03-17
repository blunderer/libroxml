# This library is minimum, easy-to-use, C implementation for xml file parsing. #

contact: tristan.lelong@libroxml.net

Official webpage and documentation is available at http://www.libroxml.net

Official downloads are available at http://download.libroxml.net

Debian user can go to http://debian.libroxml.net for debian mirror and packages

## Why libroxml? ##
Because XML parsing is always hard to reinvent

Because very often, xml libraries are too big to fit with little application

To be sure libroxml will stay easy to use, I'll limit the public API to 30 functions.
This is quite enought for basic needs, and most of the time, we don't need more.

Currently 22 functions are part of the public API and library binary is about 50Ko.

see [libroxml performances](RoxmlPerfs.md)

## What's happening Now ##

  * **2014-09-29** git HEAD is now pointing to 3.0 beta libroxml version. This version doesn't change behavior, but includes a massive reorganization of the source code, as well a refactoring of some functions. This result in a faster and smaller libroxml:
    * 48KB for the full libary
    * 24KB for the minimal version

  * 2014-09-02 Server up again: I switched to a backup server and only lost download statistics for the last 2 month.
  * Since 2014-09-02 server down: libroxml download is not reachable because of the HDD being dead. Recovery of the server is in progress, but this might take a few days before the server is up again.

  * Since 2013-12-16 autotools are now the default build system for libroxml.

  * Since 2013-12-04 it is possible to configure the features of libroxml by defining a site.mk file. This allow to make libroxml smaller and an even better fit for deeply embedded systems: with only core parsing libroxml.so is smaller than 30kB.

  * Since 2013-07-15 an exception to LGPL v2.1 for static linking was added. This should makes it easier for people wanting to integrate the library into iOS / android packages. Read the [License.txt](RoxmlLicense.md) file for more informations.

  * googlecode is deprecating its download section, and starting January 2014, it will not be possible to post any new downloads. To address this, Libroxml downloads are now officially hosted at http://download.libroxml.net. The download page on googlecode will stay available, but all newer versions will be exclusively available on the new server after version 2.2.2. All previous downloads are also ported to the new server.

  * Since 2012-03-09 Libroxml source code is using GIT

  * Since 2011-09-22 Libroxml is packaged in openembedded.

  * Since 2011-07-13 Libroxml is packaged in buildroot.

  * libroxml is now quite stabilized. SVN version should not break everything. It is of course always more carefull to use packages from download section, but SVN checkout gives you the benefits of lastest bug fixes.

## What can do libroxml version 1.x ? ##
It allow you to easily:
  * load / unload document from files
  * load / unload document from buffer
  * navigate thru the xml tree using libroxml [C/C++ API](PublicAPI.md)
  * read attributes and attributes' values for nodes
  * get content of nodes
  * use basic XPath expressions (more to come)

## What do libroxml version 2.x add ? ##
It allow you to easily:
  * use more XPath expressions, [see xpath list](XpathList.md) (and still more to come)
  * modifications on the loaded XML tree
  * applying modification from the loaded XML tree to a file or buffer

## What more do libroxml provide? ##
It also includes:
  * a mini XPath resolver **roxml** to parse xml files from shell scripts directly.
  * unit tests for all of the API functions
  * binary for performance measure

## MORE ##
libroxml now provide a fuse module that allow you to attach an XML file to a mount point and navigate inside as if you were on a real file system.
  * each node is a directory
  * each attribute is a file
  * text content of a node is a special file named content.data

This allow you to navigate using XPath syntax such as:

`cd /rss[@version=2.0]/channel/item["last()"]/title`

I'm not sure it's very usefull yet, but at least it's funny. If anyone find something to do with it, please contact me...