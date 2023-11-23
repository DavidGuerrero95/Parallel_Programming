import pandas as pd
import matplotlib.pyplot as plt

# Assuming the CSV files are available in the current directory
# Read the CSV files into pandas DataFrames
sequential_df = pd.read_csv('times_sequential.csv')
coarse_grain_df = pd.read_csv('times_coarse_grain.csv')
fine_grain_df = pd.read_csv('times_fine_grain.csv')

# Calculate the average execution time for each number of threads
sequential_df['avg_time'] = sequential_df.iloc[:, 1:].mean(axis=1)
coarse_grain_df['avg_time'] = coarse_grain_df.iloc[:, 1:].mean(axis=1)
fine_grain_df['avg_time'] = fine_grain_df.iloc[:, 1:].mean(axis=1)

# Create a plot
plt.figure(figsize=(12, 8))

# Plot each dataset
plt.plot(sequential_df['threads'], sequential_df['avg_time'], label='Sequential', marker='*')
plt.plot(coarse_grain_df['threads'], coarse_grain_df['avg_time'], label='Coarse Grain', marker='x')
plt.plot(fine_grain_df['threads'], fine_grain_df['avg_time'], label='Fine Grain', marker='^')

# Labeling the plot
plt.title('Execution Time vs Number of Threads')
plt.xlabel('Number of Threads')
plt.ylabel('Average Execution Time (s)')
plt.legend()
plt.grid(True)

# Show the plot
plt.show()
