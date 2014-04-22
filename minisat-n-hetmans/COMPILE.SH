#!/bin/sh
export MROOT="`pwd`"
cd nhetmans
make
mv minisat ../nhet
echo "Usage: `pwd`/nhet N"

