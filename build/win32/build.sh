#!/bin/bash -e

cd `dirname $0`

BUILDROOT="build/win32"

cd ../..

VERSION=$1
REVISION=$2
FOLDER="blobWarsAttrition-$1"
OUT="$BUILDROOT/$FOLDER"

make -f makefile.win32 clean
make -f makefile.win32

mkdir -p $OUT
rm -rf $OUT/*

cp blobWarsAttrition.exe $OUT

cp -rL data $OUT
cp -rL gfx $OUT
cp -rL music $OUT
cp -rL sound $OUT
cp -rL manual $OUT
cp -rL locale $OUT
cp LICENSE $OUT
cp README.md $OUT

cp /usr/x86_64-w64-mingw32/bin/*.dll $OUT

cd $BUILDROOT

zip -r blobWarsAttrition-${VERSION}-${REVISION}.win32.zip $FOLDER

mv *.zip ../../dist

rm -rf $FOLDER
