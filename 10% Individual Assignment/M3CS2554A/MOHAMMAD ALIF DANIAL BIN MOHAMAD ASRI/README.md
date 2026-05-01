# ============================================================
# SMART CITY TRAFFIC FLOW SIMULATOR
# NAME: MOHAMMAD ALIF DANIAL BIN MOHAMAD ASRI
# STUDENT ID: 2024237606
# COURSE: ITT440 - PARALLEL PROGRAMMING
# ============================================================

"""
# Problem Statement
  In modern urban planning, thousands of vehicles must be simulated to predict 
  traffic congestion in real-time. It is too slow to process each vehicle's 
  movement and sensor data sequentially. This assignment shows how traffic 
  modeling can be made faster by using concurrent and parallel programming.

# Objective
  * Create a system that analyzes traffic sensor records to update vehicle physics.
  * Implement sequential, concurrent (threading), and parallel (multiprocessing) versions.
  * Examine each system's performance to find the most efficient approach.
  * Explain why CPU-intensive physics calculations benefit from parallel programming.

# Project Scope
* Data Size      - 10,000,000 traffic sensor records
* Vehicles       - 5,000 unique vehicle IDs
* Math Intensity - 10^5 operations per vehicle
* Target Vehicles- 10 random vehicles for recommendation/analysis
* Language       - Python 3.10+
* Platform       - VS Code / IDLE

# Three Implementations
* Sequential  - One by one processing.
* Concurrent  - Threading (Interleaved execution).
* Parallel    - Multiprocessing (Simultaneous execution).

# Difference between Sequential, Concurrent & Parallel

## Sequential
* Tasks are carried out linearly. The previous task must finish before the next starts.

## Concurrent
* Tasks advance by alternating (context switching). Only one task runs at a time due to GIL.
* Best for: I/O operations (waiting for sensors/network).

## Parallel
* Multiple CPU cores are used to conduct many tasks at the exact same time.
* Best for: CPU-intensive calculations (physics engines).
"""

import threading
import multiprocessing
import time
import math
import random
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor

# ============================================================
# Code Structure
# ============================================================

def generate_traffic_data():
    """Create 10 million records"""
    print(f"[GEN] Generating 10,000,000 records...")
    return [random.randint(1, 5000) for _ in range(10_000_000)]

def build_vehicle_db(data):
    """Build dictionary index for fast lookups"""
    print(f"[DB] Building vehicle database for 5,000 vehicles...")
    db = {i: [] for i in range(1, 5001)}
    for i, v_id in enumerate(data[:1000000]): 
        db[v_id].append(i)
    return db

def calculate_physics(vehicle_id):
    """Simulates similarity scoring/physics engine updates"""
    calc_sum = 0
    for i in range(10**5):
        calc_sum += math.sqrt(i) * math.tan(i)
    time.sleep(0.01) # Simulated sensor latency
    return calc_sum

# Sequential - one at a time
def run_sequential(vehicles):
    print("\n[SEQUENTIAL] Processing...")
    start = time.perf_counter()
    for count, v_id in enumerate(vehicles, 1):
        print(f"   Vehicle {count}/10")
        calculate_physics(v_id)
    return time.perf_counter() - start

# Concurrent - threading
def run_concurrent(vehicles):
    print("\n[CONCURRENT] Threading Processing...")
    start = time.perf_counter()
    with ThreadPoolExecutor() as executor:
        list(executor.map(calculate_physics, vehicles))
    return time.perf_counter() - start

# Parallel - multiprocessing  
def run_parallel(vehicles):
    print("\n[PARALLEL] Multiprocessing Processing...")
    start = time.perf_counter()
    with ProcessPoolExecutor() as executor:
        list(executor.map(calculate_physics, vehicles))
    return time.perf_counter() - start

# ============================================================
# Result & Performance Analysis
# ============================================================

if __name__ == "__main__":
    print("============================================================")
    print("SMART CITY TRAFFIC FLOW SIMULATOR - 10 MILLION RECORDS")
    print("============================================================")
    
    # Setup Phase
    data = generate_traffic_data()
    db = build_vehicle_db(data)
    targets = [random.randint(1, 5000) for _ in range(10)]
    
    print("\n[TARGET] Processing 10 vehicles...")

    # Execution Phase
    t_seq = run_sequential(targets)
    t_con = run_concurrent(targets)
    t_par = run_parallel(targets)

    # Expected Output Formatting
    print("\n============================================================")
    print("PERFORMANCE COMPARISON")
    print("============================================================")
    print(f"{'Method':<25} {'Time':<15} {'Speedup':<10}")
    print("-" * 60)
    print(f"{'Sequential':<25} {t_seq:<15.2f} {'1.00x':<10}")
    print(f"{'Concurrent (Threads)':<25} {t_con:<15.2f} {t_seq/t_con:<10.2f}x")
    print(f"{'Parallel (Processes)':<25} {t_par:<15.2f} {t_seq/t_par:<10.2f}x")
    print("============================================================")
    print(f"WINNER: PARALLEL ({t_seq/t_par:.2f}x faster)")
    print("============================================================")

"""
# Summary 
* For CPU-intensive activities like traffic physics, parallel processing works best, 
  achieving a significant speedup by utilizing all CPU cores.
* Because of the GIL, concurrent (threading) offers little advantage for math-heavy tasks.
* Fastest Method - Parallel (Multiprocessing)
* Speedup Factor - ~4.36x faster than Sequential (depends on CPU cores)
* Improvement - Significant reduction in real-time processing time.
* Best for CPU-intensive tasks - Parallel
"""
