#!/bin/bash

while [ 0 -eq 0 ]; do
    ./build.sh;
    echo "waiting for changes to rebuild...";
    inotifywait -q -e close_write src/main.cpp;
done

