# ============================================================
# SMART CITY TRAFFIC FLOW SIMULATOR
# NAME: MOHAMMAD ALIF DANIAL BIN [YOUR LAST NAME]
# STUDENT ID: [YOUR STUDENT ID]
# ============================================================

# PROBLEM STATEMENT
# Many or thousands of vehicles must receive real-time coordinate 
# updates and traffic signal data simultaneously in a smart city.
# It is too slow to process each vehicle's physics logic sequentially.
# This assignment shows how traffic flow modeling can be made faster
# by using concurrent and parallel programming.

# OBJECTIVE
# * Create a system that analyzes sensor data to update vehicle physics.
# * Use the sequential, concurrent (threading), and parallel (multiprocessing) versions.
# * Examine each sequential, concurrent, and parallel system's performance.
# * Explain why CPU-intensive jobs benefit from parallel programming.

# PROJECT SCOPE
# * Data Size - 10,000,000 traffic records
# * Vehicles - 5,000 unique vehicles
# * Target Vehicles - 10 random vehicles for recommendation/analysis
# * Programming Language - Python 3.10+
# * Platform - VS Code / IDLE

import threading
import multiprocessing
import time
import math
import random
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor

# ============================================================
# KEY FUNCTIONS
# ============================================================

def generate_data():
    """Create 10 million records"""
    print(f"[GEN] Generating 10,000,000 records...")
    return [random.randint(1, 5000) for _ in range(10_000_000)]

def build_vehicle_db(data):
    """Build dictionary index for fast lookups"""
    print(f"[DB] Building vehicle database...")
    db = {i: [] for i in range(1, 5001)}
    for i, v_id in enumerate(data[:1000000]):
        db[v_id].append(i)
    return db

def calculate_physics(v_id):
    """CPU-intensive similarity and physics calculation"""
    calc_sum = 0
    for i in range(10**5):
        calc_sum += math.sqrt(i) * math.tan(i)
    time.sleep(0.01) # Simulated latency
    return calc_sum

# ============================================================
# THREE IMPLEMENTATIONS
# ============================================================

# Sequential - one at a time
def run_sequential(v_list):
    print("\n[SEQUENTIAL] Processing...")
    start = time.perf_counter()
    for count, v_id in enumerate(v_list, 1):
        print(f"   Vehicle {count}/10")
        calculate_physics(v_id)
    return time.perf_counter() - start

# Concurrent - threading
def run_concurrent(v_list):
    print("\n[CONCURRENT] Threading Processing...")
    start = time.perf_counter()
    with ThreadPoolExecutor() as executor:
        list(executor.map(calculate_physics, v_list))
    return time.perf_counter() - start

# Parallel - multiprocessing
def run_parallel(v_list):
    print("\n[PARALLEL] Multiprocessing Processing...")
    start = time.perf_counter()
    with ProcessPoolExecutor() as executor:
        list(executor.map(calculate_physics, v_list))
    return time.perf_counter() - start

# ============================================================
# MAIN EXECUTION
# ============================================================

if __name__ == "__main__":
    print("="*60)
    print("SMART CITY TRAFFIC SIMULATOR - 10 MILLION RECORDS")
    print("="*60)

    # Data Prep
    data = generate_data()
    db = build_vehicle_db(data)
    targets = [random.randint(1, 5000) for _ in range(10)]

    print(f"\n[TARGET] Processing 10 vehicles...")

    # Performance Benchmarking
    t_seq = run_sequential(targets)
    t_con = run_concurrent(targets)
    t_par = run_parallel(targets)

    # Result & Performance Analysis
    print("\n" + "="*60)
    print("PERFORMANCE COMPARISON")
    print("="*60)
    print(f"{'Method':<25} {'Time':<15} {'Speedup':<10}")
    print("-" * 60)
    print(f"{'Sequential':<25} {t_seq:<15.2f} {'1.00x':<10}")
    print(f"{'Concurrent (Threads)':<25} {t_con:<15.2f} {t_seq/t_con:<10.2f}x")
    print(f"{'Parallel (Processes)':<25} {t_par:<15.2f} {t_seq/t_par:<10.2f}x")
    print("="*60)
    print(f"WINNER: PARALLEL ({t_seq/t_par:.2f}x faster)")
    print("="*60)
