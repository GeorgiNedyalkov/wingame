@echo off

mkdir ..\..\build
pushd ..\..\build
cl ..\tetris\code\main.c -Zi User32.lib Gdi32.lib
popd
