#!/bin/bash

if [ $# -lt 1 ]; then
	echo "error: must specify a version number"
	exit 1
fi

VERSION=$1
TOP_DIR=$(dirname $0)/../../
REV=$(LANG=C svn info $TOP_DIR | grep Revision | awk '{print $2}')
TMP_DIR=/tmp/build-libroxml-$(date +"%y%m%d%H%M%S")

svn export -r $REV $TOP_DIR $TMP_DIR/libroxml-$VERSION > /dev/null

cp -a $TMP_DIR/libroxml-$VERSION $TMP_DIR/libroxml

tar zcf $TMP_DIR/libroxml-$VERSION.orig.tgz $TMP_DIR/libroxml-$VERSION 2> /dev/null

rm -fr $TMP_DIR/libroxml-$VERSION/unittest
rm -fr $TMP_DIR/libroxml-$VERSION/data/scripts
rm -fr $TMP_DIR/libroxml-$VERSION/libroxml.spec
rm -fr $TMP_DIR/libroxml-$VERSION/TODO

diff -Naur $TMP_DIR//libroxml $TMP_DIR/libroxml-$VERSION | gzip > $TMP_DIR/libroxml-$VERSION.diff.gz
rm -fr $TMP_DIR//libroxml


