#!/bin/bash

# Compile the program
gcc coarse_coarse.c -o coarse -pthread

# Create or clear the csv file
echo "threads"$(printf ",time%d" {1..100}) > times_coarse_grain.csv

# Run the program with different number of threads
for threads in {1..32}; do
  # Print the number of threads
  echo -n "$threads" >> times_coarse_grain.csv
  for i in {1..100}; do
    # Run the granofino and get the time
    result=$(./coarse matrices_large.dat $threads)
    # Extract the timing information and append to the CSV line
    echo -n ",$result" >> times_coarse_grain.csv
  done
  # End of line for CSV
  echo "" >> times_coarse_grain.csv
done