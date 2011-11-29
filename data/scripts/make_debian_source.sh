#!/bin/bash

if [ $# -lt 1 ]; then
	echo "usage: $0 rev_num"
	echo "usage: $0 local rev_num"
	echo "error: must specify a version number"
	exit 1
fi

VERSION=$1
TOP_DIR=$(dirname $0)/../../
REV=$(LANG=C svn info $TOP_DIR | grep Revision | awk '{print $2}')
TMP_DIR=/tmp/build-libroxml-$(date +"%y%m%d%H%M%S")

mkdir -p $TMP_DIR

if [ "$VERSION" = "local" ]; then
	echo "Exporting from local"
	VERSION=$2
	cp -r $TOP_DIR $TMP_DIR/libroxml-$VERSION > /dev/null
	find $TMP_DIR/libroxml-$VERSION -name ".svn" -exec rm -fr {} \;
else
	echo "Exporting from SVN at rev $REV"
	svn export -r $REV $TOP_DIR $TMP_DIR/libroxml-$VERSION > /dev/null
fi

echo "Cleaning repository"
rm -fr $TMP_DIR/libroxml-$VERSION/unittest
rm -fr $TMP_DIR/libroxml-$VERSION/data/scripts
rm -fr $TMP_DIR/libroxml-$VERSION/libroxml.spec
rm -fr $TMP_DIR/libroxml-$VERSION/TODO
make -C $TMP_DIR/libroxml-$VERSION/ mrproper

mv $TMP_DIR/libroxml-$VERSION/debian $TMP_DIR/

echo "Generate original package"
cd $TMP_DIR && tar zcf libroxml_$VERSION.orig.tar.gz libroxml-$VERSION> /dev/null

mv $TMP_DIR/debian $TMP_DIR/libroxml-$VERSION/

echo "Build package"
cd $TMP_DIR/libroxml-$VERSION/ && dpkg-buildpackage

echo "********************************************************************"
echo "Your package is built in '$TMP_DIR'"
echo "********************************************************************"

