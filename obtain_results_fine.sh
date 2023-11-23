#!/bin/bash

# Compile the program
gcc Grano_fino.c -o granofino -pthread

# Create or clear the csv file
echo "threads"$(printf ",time%d" {1..100}) > times_fine_grain.csv

# Run the program with different number of threads
for threads in {1..32}; do
  # Print the number of threads
  echo -n "$threads" >> times_fine_grain.csv
  for i in {1..100}; do
    # Run the granofino and get the time
    result=$(./granofino matrices_large.dat $threads)
    # Extract the timing information and append to the CSV line
    echo -n ",$result" >> times_fine_grain.csv
  done
  # End of line for CSV
  echo "" >> times_fine_grain.csv
done