import time
from grab import pybind_11_example as pbe

console = Console()

robomap[1000][3]

print('C++:')
start_time = time.perf_counter_ns()
print('Answer:', pbe.map(robomap))
print('Time:', (time.perf_counter_ns() - start_time) / 1e9, 's')