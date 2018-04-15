#!/bin/bash -e

VERSION=`egrep 'VERSION = ([0-9.+])' ../common.mk | awk '{print $3}'`

mkdir -p ../dist
rm -rf ../dist/*

linux/build.sh $VERSION

win32/build.sh $VERSION
