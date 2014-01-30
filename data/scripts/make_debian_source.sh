#!/bin/bash

if [ $# -lt 1 ]; then
	echo "usage: $0 <rev>"
	echo "error: must specify a version number"
	exit 1
fi

VERSION=$1
TOP_DIR=$(cd $(dirname $0)/../../ && pwd)
TMP_DIR=/tmp/build-libroxml-$(date +"%y%m%d%H%M%S")

LIBROXML_MAJOR=$(sed -n 's#.*MAJOR_VERSION.*, \(.*\))$#\1#p' $TOP_DIR/configure.ac)
LIBROXML_MINOR=$(sed -n 's#.*MINOR_VERSION.*, \(.*\))$#\1#p' $TOP_DIR/configure.ac)
LIBROXML_MICRO=$(sed -n 's#.*MICRO_VERSION.*, \(.*\))$#\1#p' $TOP_DIR/configure.ac)
LIBROXML_VERSION=$LIBROXML_MAJOR.$LIBROXML_MINOR.$LIBROXML_MICRO

mkdir -p $TMP_DIR
mkdir -p $TMP_DIR/logs

echo "== Exporting from GIT @$VERSION libroxml-$LIBROXML_VERSION =="
(cd $TOP_DIR && git archive --format=tar --prefix=libroxml-git-$VERSION/ $VERSION . > $TMP_DIR/libroxml-git-$VERSION.tar)
tar xf $TMP_DIR/libroxml-git-$VERSION.tar -C $TMP_DIR/ && rm -f $TMP_DIR/libroxml-git-$VERSION.tar

echo "== Generating upstream sources =="
(cd $TMP_DIR/libroxml-git-$VERSION/ && ./autogen.sh) &> $TMP_DIR/logs/upstream-source.txt
(cd $TMP_DIR/libroxml-git-$VERSION/ && ./configure) >> $TMP_DIR/logs/upstream-source.txt
(cd $TMP_DIR/libroxml-git-$VERSION/ && make dist-gzip && mv libroxml-$LIBROXML_VERSION.tar.gz ..) >> $TMP_DIR/logs/upstream-source.txt

echo "== Generating debian source package =="
(cd $TMP_DIR/ && tar zxf libroxml-$LIBROXML_VERSION.tar.gz)
cp -a $TMP_DIR/libroxml-$LIBROXML_VERSION.tar.gz $TMP_DIR/libroxml_$LIBROXML_VERSION.orig.tar.gz
cp -a $TMP_DIR/libroxml-git-$VERSION/debian $TMP_DIR/libroxml-$LIBROXML_VERSION

echo "== Build package =="
(cd $TMP_DIR/libroxml-$LIBROXML_VERSION/ && QUILT_PATCHES=debian/patches quilt push -a) > $TMP_DIR/logs/buildpackage.txt
(cd $TMP_DIR/libroxml-$LIBROXML_VERSION/ && dpkg-buildpackage -us -uc) &>> $TMP_DIR/logs/buildpackage.txt

echo "== Analyze package =="
(cd $TMP_DIR/ && lintian -vIiE --pedantic --color=auto *.changes *.deb *.dsc) > $TMP_DIR/logs/lintian.txt

echo "********************************************************************"
echo "Your package is built in '$TMP_DIR'"
echo "********************************************************************"

