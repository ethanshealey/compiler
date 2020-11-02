import os, subprocess

i = []

for x in os.listdir():
    i.append(sum([int(y) for y in subprocess.getoutput('wc -l ' + x).split(' ') if y.isdigit()])) 

print(sum(i))
