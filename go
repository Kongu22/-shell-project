#!/bin/bash
make
if [ -f "./MyShell" ]; then
    ./MyShell
    make clean
else
    echo "Error: The executable file does not exist."
fi
