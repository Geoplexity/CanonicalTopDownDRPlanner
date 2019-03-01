# Canonical DR-Planner #

A software implementation of the canonical DR-planner and OMD solver for 2D, independent, bar-joint constraint systems (see publications).

## Installation
1. Install all dependencies.
    - OpenGL 3.3
    - GLEW
    - GLFW
    - Boost (specifically boost_graph)
2. Download the repository.
3. Run `make` from the repository root directory. You may need to alter `Makefile` to reflect the location of libraries on your system.
4. Test with `./drp -opt=0 -dot test_files/test.dot`.

## Usage
Command line flags (subject to change, check `main.cpp`):

- `-dot XX` where `XX` is the location of the desired input dot. Default is `test_files/test.dot`.
- `-opt=XX` where `XX` is some number. Default is `0`.
    - `0` creates the sequential canonical DR-plan of the input graph.
    - `1` displays a linkage with hints from the edge length.
    - `2` displays a framework using the input vertex positions.
    - `3` attempts the OMD problem for the input graph.

This software recognizes dot files with vertex properties `x` and `y` and edge property `length`. All graphs are interpreted as 2D bar-joint graphs.

Sample dot files are available in the `test_files` directory.

### License ###
Canonical Top Down DR Planner is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
The GNU GPL license can be found at [http://www.gnu.org/licenses/](http://www.gnu.org/licenses/).

## Publications
"Optimal Decomposition and Recombination of Isostatic Geometric Constraint Systems for Designing Layered Materials"

## Website
[The homepage for this software can be found here.](http://cise.ufl.edu/~tbaker/drp)
