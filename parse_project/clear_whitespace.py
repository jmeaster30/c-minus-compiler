import sys
import os

file = open(sys.argv[1], 'r')
lines = []

for line in file:
  line = line.replace(' ', '')
  line = line.replace('\t', '')
  line = line.replace('\n', '')
  line = line.replace('\r', '')
  lines.append(line)

file.close()
f2 = open(sys.argv[1], 'w')
for l in lines:
  f2.write(l)
f2.close()
