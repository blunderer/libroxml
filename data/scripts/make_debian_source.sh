#!/bin/bash

if [ $# -lt 1 ]; then
	echo "usage: $0 <rev>"
	echo "error: must specify a version number"
	exit 1
fi

VERSION=$1
TOP_DIR=$(cd $(dirname $0)/../../ && pwd)
TMP_DIR=/tmp/build-libroxml-$(date +"%y%m%d%H%M%S")

mkdir -p $TMP_DIR

echo "Exporting from GIT @$VERSION"
(cd $TOP_DIR && git archive --format=tar --prefix=libroxml-$VERSION/ $VERSION . | gzip > $TMP_DIR/libroxml-$VERSION.tar.gz)
tar zxf $TMP_DIR/libroxml-$VERSION.tar.gz -C $TMP_DIR/

echo "Cleaning repository"
rm -fr $TMP_DIR/libroxml-$VERSION/unittest
rm -fr $TMP_DIR/libroxml-$VERSION/data/scripts
rm -fr $TMP_DIR/libroxml-$VERSION/libroxml.spec
rm -fr $TMP_DIR/libroxml-$VERSION/TODO

mv $TMP_DIR/libroxml-$VERSION/debian $TMP_DIR/

echo "Generate original package"
cd $TMP_DIR && tar zcf libroxml_$VERSION.orig.tar.gz libroxml-$VERSION> /dev/null

mv $TMP_DIR/debian $TMP_DIR/libroxml-$VERSION/

echo "Build package"
cd $TMP_DIR/libroxml-$VERSION/ && dpkg-buildpackage

echo "********************************************************************"
echo "Your package is built in '$TMP_DIR'"
echo "********************************************************************"

