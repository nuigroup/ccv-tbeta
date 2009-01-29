#!/bin/sh

cd ../apps/examples

rm -rf `find .  -name openFrameworks.app`
rm -rf `find .  -name openFrameworksDebug.app`
rm -rf `find .  -name build`
rm -rf `find .  -name *.pbxuser` 
rm -rf `find .  -name *.mode1v3`

cd ../addonsExamples

rm -rf `find .  -name openFrameworks.app`
rm -rf `find .  -name openFrameworksDebug.app`
rm -rf `find .  -name build`
rm -rf `find .  -name *.pbxuser` 
rm -rf `find .  -name *.mode1v3`