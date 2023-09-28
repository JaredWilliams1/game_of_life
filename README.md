This is an implementation of Conway's Game of Life that is more efficient than the brute force approach.

Conway's Game of Life isn't really a game it's a simulation. It starts with a grid with values that are either on or off

`0 0 1 1 0 0`  
`0 0 1 0 1 1`  
`1 0 0 0 0 0`  
`0 0 0 0 0 1`  
`0 1 0 1 1 0`  

This is the initial state. The values are called cells and they can either be alive or dead (1 or 0).

A cell's neighbors are all those directly next to it in 8 directions (a box)

`0 0 0 0 0 0`  
`0 X X X 0 0`  
`0 X Y X 0 0`  
`0 X X X 0 0`  
`0 0 0 0 0 0`  
The X's are the neighbors of Y

The game has 2 rules:
1. If the cell is alive, then it stays alive if it has either 2 or 3 live neighbors.
2. If the cell is dead, then it springs to life only in the case that it has 3 live neighbors.

From these two rules, a wide variety of unpredictable and self sustaining patterns can emerge. For more information, here's the [wikipedia](https://en.wikipedia.org/wiki/Conway's_Game_of_Life) page:

For all implementations the first step is to duplicate the grid, one for the previous cell values, and one for the new cell values.

The brute force approach is simple. For each individual cell, you check its neighbors and count the living ones to see if the cell will be living or dead in the next iteration. I think of this as the "pull" approach because you have to pull in the cells neighbor values to find the new value of the cell.

Now lets look at the more efficient implementation. For this implementation, you store cells not as a bit value but as a byte value. The Least Significant Bit (LSB) represents whether the cell is alive or dead, and the rest of the byte represents the alive neighbor count. This way we can store both pieces of information in a single number.

Lets say a cell is stored like this:  
`0 0 0 1 0 0 1`

It's technically storing the number 9 but it's actually storing 2 pieces of information:

`0 0 0 1 0 0    |    1`  
`4 neighbors      cell is on`

The term for the rightmost binary digit is the Least Significant Bit (LSB) and it stores the living/dead status.

Notice how the living cells have odd numbered bytes and the dead cells have even numbered bytes.

If I wanted to add a neighbor to a cell you'd add 2 to the byte:

`0 0 0 1 0 1    |    1`  
`5 neighbors      cell is on`

Likewise to remove a neighbor you'd subtract 2 from the byte.

If we want to change the living/dead status of the cell we just add/subtract 1 (modifying the LSB)

This allows for a "push" approach where:  

dead cells that come to life ----> increment the neighboring cells neighbor count (by adding 2 to the byte)  

living cells that die ----> decrement the neighboring cells neighbor count (by subtracting 2 from the byte)  

All of that is for this crucial speed-up:  
We can now safely skip cells that have byte values of 0. Since we're storing both the on/off value and the neighbor count in the same number, we know that cells with a value of 0 are dead and have no living neighbors (and therefore remaining dead in the next step).

For each update step, all non-zero cells are analyzed, we calculate if they're living or dead in the next step (using the info already in the byte), and then we update the neighbor count of the surrounding cells if the cell dies or is brought to life.


## Installation
There is no installation required other than the standard C++ library

## Usage

Compilation (executed in directory where the life.cpp file is located):

```
g++ life.cpp -o life
```

After compilation run the executible

```
./life
```
