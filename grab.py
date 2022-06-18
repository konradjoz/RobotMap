from pathlib import Path

from rich.console import Console
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

example_module = Pybind11Extension(
    'pybind_11_example',
    [str(fname) for fname in Path('src').glob('*.cpp')],
    include_dirs=['include'],
    extra_compile_args=['-O3']
)

setup(
    name='pybind_11_example',
    version=0.1,
    author='Konrad Jozefczyk',
    author_email='support@example.com',
    description='Barebones pybind11+setup.py example project',
    ext_modules=[example_module],
    cmdclass={"build_ext": build_ext},
)



# Read the the 
f = open("data.txt", "r")
print(f.read(5))

Lines = f.readlines()

count = 0

t = [ [0]*3 for i in range(3)]

i, k, j = 0, 0, 0

for line in Lines:
    var1, var2, var3 = "", "", ""
    barCount = 0
    for char in line:
        if char == '@':
            barCount += 1
        elif ((barCount == 0) & (char != "@")):
            print(char)
            var1 += char
        elif ((barCount == 1) & (char != "@")):
            print(char)
            var2 += char
        elif ((barCount == 2) & (char != "@")):
            print(char)
            var3 += char
    
    t[count][0] = float(var1)
    t[count][1] = float(var2)
    t[count][2] = float(var3)

    count += 1

z = '34.224.2.2423'

print(z)

string = ""
for z in line:
     string += z

print(t)