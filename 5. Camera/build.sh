#!/bin/bash

CC=g++
LDFLAGS=`pkg-config --libs glew glfw3`

$CC Camera.cpp $LDFLAGS
