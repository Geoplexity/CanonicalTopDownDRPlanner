The p2dot script reads in a `.p` file and outputs a `.dot` file.


Steps for obtaining the `.p` file (using subdivision programs by William Floyd. Go [here](http://www.math.vt.edu/people/floyd/research/software/subdiv.html).):

1. Prepare a subdivision rule file (ex. pent.r) and an initial tiling file (ex. pent.l1)
2. Use subdivide.c to subdivide the tiling and write to a new tiling file (ex. pent.l2)
Perform step 2 as many times as necessary.
3. Use tilepack.c to read in the generated tiling file and write a CirclePack (by Ken Stephenson, available from  http://www.math.utk.edu/~kens) script file (ex. pentl2.xmd).
4. Use CirclePack to load and execute the script file. Save the packing (Menu: Save -> Save Packing) as a .p file (ex. pentl2.xmd.p), with "Complex" and "Centers" selected in the dialog.


Usage of p2dot.py:

`python p2dot.py .p_filename start_idx`

Here `start_idx` is the starting index of vertices actually in the tiling (the `.xmd` and `.p` file adds additional barycenter vertices for generating the circle packing). The index can be found in the CirclePack script file.
Ex. for pentl2.xmd.p start_idx = 37, as in line 11 of pentl2.cmd there is the command
`mark -cw -c a(37,101);`
