#!/bin/bash

CC=g++
LDFLAGS=`pkg-config --libs glew glfw3`

$CC -g *.cpp $LDFLAGS
