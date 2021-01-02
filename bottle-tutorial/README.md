# How I got opencascade working on my machine
You can install on ubuntu by adding the freecad ppa and using apt (ppa is here https://launchpad.net/~freecad-maintainers/+archive/ubuntu/freecad-stable/+packages)

However you can just download the source by going to the opencascade website (https://old.opencascade.com/content/download-center). There you can get a tar.gz containing a CMake project. The readme inside gives a good explanation of how to build but I basically did the usual:

´´´
mkdir build && cd build:
cmake ..;
make;
make install;
´´´

Note that there might be a few dependencies. I had build errors which I had to fix by installing libxi-dev via apt.

