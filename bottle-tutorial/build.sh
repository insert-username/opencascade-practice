#!/bin/bash

# fail on error since all steps
# depend on each other.
set -e;

# remove the build directory if it exists.
rm -rf bld;
mkdir bld;
pushd bld;

cmake ..;
make;
echo "Build complete!"
./bin/*
