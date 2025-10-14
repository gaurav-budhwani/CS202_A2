#!/bin/bash
# Array of programs to analyze
programs=("c_programs/program1.c" "c_programs/program2.c" "c_programs/program3.c")

# Loop and execute analyzer
for prog in "${programs[@]}"; do
    python analyzer.py "$prog"
done

