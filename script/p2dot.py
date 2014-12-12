#!/usr/bin/python

#Convert a .p packing file to a .dot graph file 

#currently start vertex index given by command line arguments

import sys
import math

narg = len(sys.argv)
if narg < 3:
    print 'Usage: p2dot .p_filename start_vertex_idx'
    sys.exit(1)
p_filename = sys.argv[1]
start_idx = int(sys.argv[2])

f = open(p_filename, 'r')
line = f.readline()
end_idx = int(line.split()[1])

# return whether the given idx is actually a vertex in tiling (not a barycenter)
def is_vertex(idx): return idx >= start_idx and idx <= end_idx

coordinate_dict = dict() # coordinates of vertices
edge_dict = dict() # edge -- length

# read combinatorics
while not line.startswith('FLOWERS:'):  line = f.readline()
line = f.readline()
while not line.strip()=='':
    a = line.split() # a[0]: i, a[1]: degree, a[2]--a[d+2]: neighbors
    i=int(a[0])
    if is_vertex(i): 
        for j in range(2, len(a)):
            nidx = int(a[j])
            if nidx > i and  is_vertex(nidx): 
                edge_dict[(i, nidx)] = 0
    line = f.readline()


# read coordinates of vertices
while not line.startswith('CENTERS:'):  line = f.readline()
s = " ".join(line.strip() for line in f) # assume center is the last piece of info ... 
coords = s.split()
for i in range(start_idx, end_idx+1):
    xi = 2*(i-1); yi = xi + 1
    x = float(coords[xi]); y = float(coords[yi])
    coordinate_dict[i] = (x,y)

# compute edge lengths
for (i,j) in edge_dict.keys():
    pi = coordinate_dict[i]; pj = coordinate_dict[j]
    d = math.sqrt( (pi[0] - pj[0])**2 + (pi[1] - pj[1])**2 )
    edge_dict[(i,j)] = d

print '# vertices: %d, # edges: %d' % (len(coordinate_dict), len(edge_dict))

# write .dot file
fout = open(p_filename+".dot", 'w')
fout.write('graph G {\n');
# write vertices
for i in range(start_idx, end_idx+1): 
    fout.write(str(i) + ' ')
    fout.write('[x='+str(coordinate_dict[i][0])+' y='+str(coordinate_dict[i][1])+'];\n')
# write edges
for (i,j) in edge_dict.keys():
    fout.write(str(i)+'--'+str(j) + ' ')
    fout.write('[length='+str(edge_dict[(i,j)])+'];\n')
fout.write('}\n')
fout.flush()
fout.close()

