#!/bin/bash
mkdir build 2> /dev/null
rm -r build/* > /dev/null
cd build
cmake ..
make

