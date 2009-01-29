#!/bin/bash

cd  ../apps/addonsExamples/
for example in $( ls )
do
echo "-----------------------------------------------------------------"
echo "building " $example
cd $example/
xcodebuild -configuration Debug -target "openFrameworks" -project "openFrameworks.xcodeproj"
echo "example " $example " built example"
cd ../
echo "-----------------------------------------------------------------"
echo ""
done
