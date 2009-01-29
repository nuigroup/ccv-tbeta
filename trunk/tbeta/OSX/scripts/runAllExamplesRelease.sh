#!/bin/bash

cd  ../apps/examples/
for example in $( ls )
do
echo "-----------------------------------------------------------------"
echo "building " $example
cd $example/
./openFrameworks.app/Contents/MacOS/openFrameworks
cd ../
echo "-----------------------------------------------------------------"
echo ""
done
