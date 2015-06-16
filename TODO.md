GUI
============
- Zoom is weird, zooms in on (0,0)
- Put a line over my thick line, just to be safe
- Animated_graph_realization_window: can't move the graph once it's done, but you can zoom

Build
============
- should make cpp files for some header only things (like gl_obj, easyfont, etc)

Pebble game
============
- Doesn't check if the same edge is entered twice (same direction or reversed)

Isostatic Graph Realizer
============
- Min_Priority_Queue: needs to handle errors more gracefully
- should check that the graph is at least three vertices
- IS make partial 2 tree actually working correctly? I just kind of made up an algorithm

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
6) Figure out the intervals of these non-edges and search the space for a realization that has the desired length of dropped edges
7) Compare results to old solver from Ruijin/Peters
8) Make video

Next steps
==========
- Recombination: Binary search approach
    - 3 steps is safe, for h through h-1
    - See "Elsevier Paper - Discussion"
    - Test with 1, then 2, then 3, etc. dropped edges
        - Make a cycle with n-edges and some diagonals that make k4s. Have to drop until k4 edges until they're gone and add back in safe diagonals.
- New video (with narration) to replace uniform search.
- Recombination at higher points in the graph. Treat rigid body child as two tree. Figure out pattern based on what happens for n incident edges, such that plugging it in yields new 2-tree.

- DR-Plan: Don't display edges, maybe just show a number?



Paper
============
- See doc/TODO.md
