#!/bin/bash

CC=g++
LDFLAGS=$(pkg-config --libs glew glfw3)

$CC CoordinateSystems.cpp "$LDFLAGS"
