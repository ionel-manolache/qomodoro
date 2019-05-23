#/bin/bash

# build the code
make clean
qmake -config release
make

# deploy needed libs
macdeployqt qomodoro.app

# Info.plist for hiding Dock icon
cp Info.plist qomodoro.app/Contents
