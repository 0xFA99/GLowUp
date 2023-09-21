#!/bin/bash

CC=g++
LDFLAGS=`pkg-config --libs glew glfw3`

$CC CreateWindow.cpp $LDFLAGS
