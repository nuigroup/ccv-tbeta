#!/bin/bash

export LC_ALL=C
cd ../apps/examples

for example in $( ls . )
do
echo "-----------------------------------------------------------------"
echo "building " + $example
cd $example
cbp2makefile.py $example.cbp Makefile
cd ../
echo "-----------------------------------------------------------------"
echo ""
done
