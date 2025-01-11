import subprocess
import time
import sys
import statistics

def run_command_with_timing(command):
    subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)  # Warm-up
    start_time = time.perf_counter()
    result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return result, time.perf_counter() - start_time

def run_program(program, t, d, runs=1):
    command = f"echo '{t} {d} 1 0 1' | prlimit --as=$((9*128*1024*1024)) time -f'%e %M %x' timeout --foreground 60s ./{program}"
    times = []
    total_mem = 0
    success_count = 0

    for _ in range(runs):
        try:
            result, execution_time = run_command_with_timing(command)
            stderr_lines = result.stderr.decode('utf-8').strip().split("\n")[-1].split()
            memory_usage, exit_code = int(stderr_lines[1]), int(stderr_lines[2])
            if exit_code == 0:
                times.append(execution_time)
                total_mem += memory_usage
                success_count += 1
        except Exception as e:
            print(f"Error: {e}")
            continue

    if success_count == 0:
        return -1, -1, -1, False
    avg_time = sum(times) / success_count
    avg_mem = total_mem / success_count
    std_dev = statistics.stdev(times) if len(times) > 1 else 0
    return avg_time, avg_mem, std_dev, True, times

def measure_scalability(runs):
    ds = [5, 10, 15, 20, 25, 30, 32, 34]
    threads = [2**i for i in range(5)]
    results = []

    for d in ds:
        # Run reference program for t=1 only
        t = 1
        ref_time, ref_mem, ref_std_dev, ref_success, ref_times = run_program("build/reference/reference", t, d, runs)
        if not ref_success:
            print(f"Reference program failed for d={d}, t={t}")
            continue

        for t in threads:
            par_time, par_mem, par_std_dev, par_success, par_times = run_program("build/parallel/parallel", t, d, runs)
            if not par_success:
                scalability = -1
            else:
                scalability = ref_time / par_time if par_time else 1

            results.append({
                "d": d, "threads": t, "ref_time": ref_time, "ref_mem": ref_mem, "ref_std_dev": ref_std_dev,
                "par_time": par_time, "par_mem": par_mem, "par_std_dev": par_std_dev, "scalability": scalability,
                "ref_times": ref_times, "par_times": par_times
            })

            print(f"d={d}, threads={t}: ref_time={ref_time:.2f}s, par_time={par_time:.2f}s, "
                  f"scalability={scalability:.2f}, ref_mem={int(ref_mem)}KB, par_mem={int(par_mem)}KB, "
                  f"ref_std_dev={ref_std_dev:.2f}, par_std_dev={par_std_dev:.2f}")
            print(f"ref_times={ref_times}")
            print(f"par_times={par_times}")
    return results

def main():
    runs = int(sys.argv[1]) if len(sys.argv) > 1 else 1
    results = measure_scalability(runs)
    print("Measurement complete. Results:")
    for result in results:
        print(result)

if __name__ == "__main__":
    main()

