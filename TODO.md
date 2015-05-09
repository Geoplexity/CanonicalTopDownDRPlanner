GUI
============
- Zoom is weird, zooms in on (0,0)
- Put a line over my thick line, just to be safe

Build
============
- Circle.hpp should be broken up
- windows in main should be pulled out into separate files
- should make cpp files for some header only things (like circle, easyfont, etc)

Pebble game
============
- Doesn't check if the same edge is entered twice (same direction or reversed)

Series-parallel testing
============
- Can there be degree zero vertices?
- Can there be 3+ degree one verts?
- MinPriorityQueue needs to handle errors more gracefully
- Isostatic_Graph_Realizer should check that the graph is at least three vertices

Known bugs
============
- Run square edges triangle and press space... usually crashes

Features to add
============
- Color code clicking
- Decomposition as explained in the paper
- Recombination

Software
========
1) Make a mechanism for doing edge contractions and deletions
2) Detect if a graph is a partial 2-tree by looking for K4 minor
3) Automate the process of transforming a graph into a partial 2-tree via edge deletions, either randomized or some heuristic... will require some thought, it will be random at first
4) Figure out how to pick the non-edges needed to get a 2-tree
5) Determine the realization of a graph given lengths of non-edges
6) Figure out the intervals of these non-edges and search the space for a realization that has the desired length of dropped edges
7) Compare results to old solver from Ruijin/Peters
8) Make video




Paper
============
- See doc/TODO.md
