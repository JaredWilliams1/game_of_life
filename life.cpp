#include <iostream>
#include <cstdlib>


class cellmap {
private:
   unsigned int width;
   unsigned int height;
   unsigned int length_in_bytes;
public:
    unsigned char *cells;
    unsigned char *temp_cells;
   cellmap(unsigned int h, unsigned int v);
   ~cellmap(void);
   void set_cell(unsigned int x, unsigned int y);
   void clear_cell(unsigned int x, unsigned int y);
   int cell_state(int x, int y);
   int count_neighbors(int x, int y);
   void next_generation(void);
   void init(void);
};

unsigned int cellmap_width = 6;
unsigned int cellmap_height = 6;

/* Width & height in pixels of each cell. */
unsigned int magnifier = 2;

/* Randomizing seed */
unsigned int seed;

int main()
{
    cellmap current_map(cellmap_height, cellmap_width);

    current_map.init();  // randomly initialize cell map

      while (true) {

        // Print matrix
        for (int i = 0; i < cellmap_height; i++) {
            for (int j = 0; j < cellmap_width; j++) {
                // std::cout << current_map.cells[i*cellmap_width+j] << " ";
                std::cout << current_map.cell_state(j, i) << " ";
            }
            std::cout << "\n";
        }

        std::string input;
        std::cout << "Continue or stop? ";
        std::cin >> input;

        if (input == "stop") {
            break;
        }
        else if (input == "continue") {
            current_map.next_generation();
        }
        else {
            std::cout << "Invalid input. Please enter 'continue' or 'stop'.\n";
        }

    }
   return 0;
}

/* cellmap constructor. */
cellmap::cellmap(unsigned int h, unsigned int w)
{
   width = w;
   height = h;
   length_in_bytes = w * h;
   cells = new unsigned char[length_in_bytes];  // cell storage
   temp_cells = new unsigned char[length_in_bytes]; // temp cell storage
   if ( (cells == NULL) || (temp_cells == NULL) ) {
      printf("Out of memory\n");
      exit(1);
   }
   memset(cells, 0, length_in_bytes);  // clear all cells, to start
}

/* cellmap destructor. */
cellmap::~cellmap(void)
{
   delete[] cells;
   delete[] temp_cells;
}

/* Turns an off-cell on, incrementing the on-neighbor count for the
   eight neighboring cells. */
void cellmap::set_cell(unsigned int x, unsigned int y)
{
   unsigned int w = width, h = height;
   int xoleft, xoright, yoabove, yobelow;
   unsigned char *cell_ptr = cells + (y * w) + x;

   // Calculate the offsets to the eight neighboring cells,
   // accounting for wrapping around at the edges of the cell map
   if (x == 0)
      xoleft = w - 1;
   else
      xoleft = -1;
   if (y == 0)
      yoabove = length_in_bytes - w;
   else
      yoabove = -w;
   if (x == (w - 1))
      xoright = -(w - 1);
   else
      xoright = 1;
   if (y == (h - 1))
      yobelow = -(length_in_bytes - w);
   else
      yobelow = w;

   *(cell_ptr) |= 0x01;
   *(cell_ptr + yoabove + xoleft) += 2;
   *(cell_ptr + yoabove) += 2;
   *(cell_ptr + yoabove + xoright) += 2;
   *(cell_ptr + xoleft) += 2;
   *(cell_ptr + xoright) += 2;
   *(cell_ptr + yobelow + xoleft) += 2;
   *(cell_ptr + yobelow) += 2;
   *(cell_ptr + yobelow + xoright) += 2;
}

/* Turns an on-cell off, decrementing the on-neighbor count for the
   eight neighboring cells. */
void cellmap::clear_cell(unsigned int x, unsigned int y)
{
   unsigned int w = width, h = height;
   int xoleft, xoright, yoabove, yobelow;
   unsigned char *cell_ptr = cells + (y * w) + x;

   // Calculate the offsets to the eight neighboring cells,
   // accounting for wrapping around at the edges of the cell map
   if (x == 0)
      xoleft = w - 1;
   else
      xoleft = -1;
   if (y == 0)
      yoabove = length_in_bytes - w;
   else
      yoabove = -w;
   if (x == (w - 1))
      xoright = -(w - 1);
   else
      xoright = 1;
   if (y == (h - 1))
      yobelow = -(length_in_bytes - w);
   else
      yobelow = w;

   *(cell_ptr) &= ~0x01;
   *(cell_ptr + yoabove + xoleft) -= 2;
   *(cell_ptr + yoabove ) -= 2;
   *(cell_ptr + yoabove + xoright) -= 2;
   *(cell_ptr + xoleft) -= 2;
   *(cell_ptr + xoright) -= 2;
   *(cell_ptr + yobelow + xoleft) -= 2;
   *(cell_ptr + yobelow) -= 2;
   *(cell_ptr + yobelow + xoright) -= 2;
}

/* Returns cell state (1=on or 0=off). */
int cellmap::cell_state(int x, int y)
{
   unsigned char *cell_ptr;

   cell_ptr = cells + (y * width) + x;
   return *cell_ptr & 0x01;
}

/* Calculates and displays the next generation of current_map */
void cellmap::next_generation()
{
   unsigned int x, y, count;
   unsigned int h = height, w = width;
   unsigned char *cell_ptr, *row_cell_ptr;

   // Copy to temp map, so we can have an unaltered version from
   // which to work
   memcpy(temp_cells, cells, length_in_bytes);

   // Process all cells in the current cell map
   cell_ptr = temp_cells;     // first cell in cell map
   for (y=0; y<h; y++) {      // repeat for each row of cells
   // Process all cells in the current row of the cell map
      x = 0;
      do {        // repeat for each cell in row
                  // Zip quickly through as many off-cells with no
                  // neighbors as possible
         while (*cell_ptr == 0) {
            cell_ptr++; // advance to the next cell
            if (++x >= w) goto RowDone;
         }
         // Found a cell that's either on or has on-neighbors,
         // so see if its state needs to be changed
         count = *cell_ptr >> 1; // # of neighboring on-cells
         if (*cell_ptr & 0x01) {
            // Cell is on; turn it off if it doesn't have
            // 2 or 3 neighbors
            if ((count != 2) && (count != 3)) {
               clear_cell(x, y);
            }
         } else {
            // Cell is off; turn it on if it has exactly 3 neighbors
            if (count == 3) {
               set_cell(x, y);
            }
         }
         // Advance to the next cell
         cell_ptr++; // advance to the next cell byte
      } while (++x < w);
RowDone:
;
   }
}

/* Randomly initializes the cellmap to about 50% on-pixels. */
void cellmap::init()
{
   unsigned int x, y, init_length;

   // Get the seed; seed randomly if 0 entered
   std::cout << "Seed (0 for random seed): ";
   std::cin >> seed;
   if (seed == 0) seed = (unsigned) time(NULL);

   // Randomly initialize the initial cell map to 50% on-pixels
   // (actually generally fewer, because some coordinates will be
   // randomly selected more than once)
   std::cout << "Initializing...";
   srand(seed);
   init_length = (height * width) / 2;
   do {
      x = std::rand() % width;
      y = std::rand() % height;
      if (cell_state(x, y) == 0) {
         set_cell(x, y);
      }
   } while (--init_length);
}