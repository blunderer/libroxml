#!/bin/bash

if [ $# -lt 1 ]; then
	echo "usage: $0 <rev>"
	echo "error: must specify a version number"
	exit 1
fi

VERSION=$1
TOP_DIR=$(cd $(dirname $0)/../../ && pwd)
TMP_DIR=/tmp/build-libroxml-$(date +"%y%m%d%H%M%S")

LIBROXML_VERSION=$(sed -n "s,\(Version: \)\(.*\),\2,p" $TOP_DIR/libroxml.pc)

mkdir -p $TMP_DIR

echo "Exporting from GIT @$VERSION"
(cd $TOP_DIR && git archive --format=tar --prefix=libroxml-$VERSION/ $VERSION . | gzip > $TMP_DIR/libroxml-$VERSION.tar.gz)
tar zxf $TMP_DIR/libroxml-$VERSION.tar.gz -C $TMP_DIR/ && rm -f $TMP_DIR/libroxml-$VERSION.tar.gz

echo "Cleaning repository"
rm -fr $TMP_DIR/libroxml-$VERSION/unittest
rm -fr $TMP_DIR/libroxml-$VERSION/data/scripts
rm -fr $TMP_DIR/libroxml-$VERSION/libroxml.spec
rm -fr $TMP_DIR/libroxml-$VERSION/TODO

mv $TMP_DIR/libroxml-$VERSION/debian $TMP_DIR/

echo "Export native source package"
cd $TMP_DIR
mv libroxml-$VERSION libroxml-$LIBROXML_VERSION 
tar zcf libroxml-$LIBROXML_VERSION.tar.gz libroxml-$LIBROXML_VERSION > /dev/null
mv libroxml-$LIBROXML_VERSION libroxml_$LIBROXML_VERSION 

echo "Generate original package"
tar zcf libroxml_$LIBROXML_VERSION.orig.tar.gz libroxml_$LIBROXML_VERSION > /dev/null

echo "Build package"
mv $TMP_DIR/debian $TMP_DIR/libroxml_$LIBROXML_VERSION/
cd $TMP_DIR/libroxml_$LIBROXML_VERSION/ && dpkg-buildpackage

echo "********************************************************************"
echo "Your package is built in '$TMP_DIR'"
echo "********************************************************************"

