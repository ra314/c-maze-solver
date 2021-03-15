# c-maze-solver
Input Format:
First provide the dimensions of the maze.

The next line is the location of the agent.

The next line is the location of the goal.

The next lines upto a "$" provide locations for blocks to be placed in the maze.

These blocks cannot be traversed through.

Then provide a sample path. This path will be verified, or fixed if not valid.

Eg:
```
Input:
3x3
[0,0]
[2,2]
[1,1]
[1,0]
$
[0,0]->[0,1]->[0,2]->[1,2]->[2,2]

Output:
==STAGE 0=======================================
The grid has 3 rows and 3 columns.
The grid has 2 block(s).
The initial cell in the grid is [0,0].
The goal cell in the grid is [2,2].
The proposed route in the grid is:
[0,0]->[0,1]->[1,1]->[1,2]->[2,2].
There is a block on this route!
==STAGE 1=======================================
 012
0I* 
1##*
2  G
------------------------------------------------
 012
0I**
1##*
2  G
------------------------------------------------
[0,0]->[0,1]->[0,2]->[1,2]->[2,2].
The route is valid!
================================================
```
