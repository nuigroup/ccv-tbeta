#!/bin/bash

export LC_ALL=C
cd ../apps/addonsExamples

for example in $( ls . )
do
echo "-----------------------------------------------------------------"
echo "building " + $example
cd $example
#codeblocks --build --target="Debug" "$example.cbp"
#codeblocks --build --target="Release" "$example.cbp"
#make Debug
#make Release
cbp2makefile.py $example.cbp Makefile
cd ../
echo "-----------------------------------------------------------------"
echo ""
done
