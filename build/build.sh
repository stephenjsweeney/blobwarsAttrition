#!/bin/sh

cd ..

rm -rf dist
make clean
make
make dist
make src-dist
