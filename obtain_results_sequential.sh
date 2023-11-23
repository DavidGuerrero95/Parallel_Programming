#!/bin/bash

# Compile the program
gcc matmulseq_file.c -o sequential

# Create or clear the csv file
echo "sequential"$(printf ",time%d" {1..100}) > times_sequential.csv

# Run the program with different number of threads
for sequential in {1..1}; do
  # Print the number of threads
  echo -n "sequential" >> times_sequential.csv
  for i in {1..100}; do
    # Run the sequential and get the time
    result=$(./sequential matrices_large.dat)
    # Extract the timing information and append to the CSV line
    echo -n ",$result" >> times_sequential.csv
  done
  # End of line for CSV
  echo "" >> times_sequential.csv
done