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
- Color code clicking (stops working when something is highlighted)
- Decomposition as explained in the paper
- Recombination

Next steps
========
1. Figure out the intervals of these non-edges and search the space for a realization that has the desired length of dropped edges
2. Implement recombination at higher points in the graph.
    - Treat rigid body child as two tree. Figure out pattern based on what happens for n incident edges, such that plugging it in yields new 2-tree.
3. Compare results to old solver from Ruijin/Peters
4. New video (with narration) to replace uniform search.

OMD
========
- Recombination: Binary search approach
    - 3 steps is safe, for h through h-1
    - See "Elsevier Paper - Discussion"
    - Test with 1, then 2, then 3, etc. dropped edges
        - Make a cycle with n-edges and some diagonals that make k4s. Have to drop until k4 edges until they're gone and add back in safe diagonals.


DRP Software Plan
=========
- Layout
    - Menu bar
        - Load linkage, save framework, save all frameworks, etc.
    - Button bar? (frequent actions)
    - Three columns
        - DR-Plan
            - Toggle for seeing edges in the plan (otherwise, dashed line w/ number of edges)
            - Click to select node
            - Highlights selected node plus the whole subtree
            - Don't display edges as nodes, maybe just put the # of edges
        - Graph
            - Shows whole graph
            - Emphasize realized subgraphs with orientation
            - Emphasize selected DRP node
        - Picker (for realization orientation)
            - Shows only selected DRP node
            - De-emphasize realized subchildren
            - Buttons to toggle through orientations and to select one (shows warning if an ancestor is already selected)
- Other Features
    - "Groups" to select subgraphs that must be in the same node. Will require code improvement.
    - Click and drag to resize columns
    - Graph drawing
        - Import "motifs". Subgraphs that you can hook up.
    - Button to replace third column with a display of all the realized parts of the graph
    - Auto step through DRP nodes while user chooses realizations. Resumes upon repicking.
    - Auto realizing with backtracking


Paper
============
- See doc/TODO.md
