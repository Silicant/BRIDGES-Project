
#include "Bridges.h"
#include "ColorGrid.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
//we added this v
#include <cmath>
#include <numeric>
#include <ctime>
#include <cstdlib>
#include <utility>
#include <algorithm>

#include "DataSource.h"
#include "data_src/ElevationData.h"
using namespace std;
using namespace bridges;

//` This program implements the mountain paths assignment in C++. See the README.md for a
// detailed description

// takes in the processed elevation data and returns a color grid for
// visualization using BRIDGES
double get_t(int a, int b, int v) { //a is min, b is max, v is current data value
	return (static_cast<double>(v - a) / static_cast<double>(b - a));
}
ColorGrid getImage(const ElevationData& elev_data) {
  //DONE
  //
  //return a Color Grid that represents the elevation as gray scale
  //
	//
	// use the elevation to print the level of color like a topography map
	//
	//
  // access elevation with elev_data.getVal(row, col)
  // compute the intensity by linear interpolation between elev_data.getMinVal(); and elev_data.getMaxVal();
  //
	int rows = elev_data.getRows();
	int cols = elev_data.getCols();
	double elevMin = elev_data.getMinVal();
	double elevMax = elev_data.getMaxVal();

  // create ColorGrid with ColorGrid(nbrows, nbcols)
  // fill ColorGrid with .set(row, col, color)
  ColorGrid cg (rows,cols);
	int v;
	double t;
	int color;
	for (int i = 0; i < rows; i++){
		for (int j = 0; j < cols; j++) {
			v = elev_data.getVal(i,j);
			t = get_t(elevMin, elevMax, v);
			color = lerp(0., 255., t);
			cg.set(i, j, Color(color,color,color));
		}
	}
	//cout << "I have " << cols << " columns\n";
  
  return cg;
}

struct Point {
	int cmp; //elevation differnce from curr
	int row;
	int col;
	Point(int new_elev, int new_row, int new_col) : cmp(abs(new_elev)), row(abs(new_row)), col(abs(new_col)) {}

};

//returns a pair of the location where the least path of resistance is
pair<int,int> compare(int currRow, int currCol, const ElevationData& elev_data) {
	int curr = elev_data.getVal(currRow, currCol);

	Point top(curr - elev_data.getVal(currRow - 1, currCol + 1), currRow - 1, currCol+1); 
	Point middle(curr - elev_data.getVal(currRow, currCol+1), currRow, currCol+1); 	//y is rows, x is cols
	Point bottom(curr - elev_data.getVal(currRow + 1 , currCol + 1), currRow + 1, currCol + 1); 
	

	pair<int,int> ret = {currRow, currCol};


	if (bottom.cmp < middle.cmp && bottom.cmp < top.cmp) { //case 1
		ret = {bottom.row, bottom.col};
		//cout << "case1: " <<  bottom.cmp << "\n";
	}
	else if (middle.cmp <= top.cmp && middle.cmp <= bottom.cmp) { //case 2, 3
		ret = {middle.row, middle.col};
		//cout << "case2/3: " << middle.cmp << "\n";
	}
	else if(top.cmp < middle.cmp && top.cmp < bottom.cmp) { //case for if top is least
		ret = {top.row, top.col};
		//cout << "Custom case: " << top.cmp << "\n";
	}
	else if (top.cmp - bottom.cmp == 0) { //case 4
		if (rand() % 2 == 0) {
			ret = {top.row,top.col};
			//cout << "case4: " << top.cmp << "\n";
		}
		else {
			ret = {bottom.row, bottom.col};
			//cout << "case4 bottom: " << bottom.cmp << "\n";
		}
	}
	//cout << ret.first << ", " << ret.second << "\n";

   return ret;

}


// determines the least effort path through the mountain starting a point on
// the left edge of the image
void findPath(const ElevationData&  elev_data, int startRow, ColorGrid& cg) {
  //        Run the greedy path from (0, startRow) to the right of the image
  // always move right, but select the right cell, the top right cell, or bottom right cell
  // by minimizing the difference of elevation.

  // Write path to the colorgrid

// FOR JACKSON // Iterations move right one, and each time the iteration ends it updates currRow 
	// up one, down one, or the same level depending on which path is best. To handle when the path reaches the edge,
	// create if statements for if the cells checked are within the bounds (> 0, < Rows). If the cell does not
	// exist within the bounds, don't check it (this will avoid the error). This means that it should still check
	// the middle and bottom rows or vice versa and can continue onwards.
	// After that is handled, the other four edge cases can be handled. I think randomly choosing between equal paths
	// should suffice in edge case 4.
	int currRow = startRow;
	int currCol = 0;
	cg.set(startRow, 0, Color(0,255,0));
	for (int i = 0; i < elev_data.getCols() - 1; i++) {
		auto [nextRow, nextCol] = compare(currRow, i, elev_data);
		cg.set(nextRow, nextCol, Color(255,0,0)); 
		currRow = nextRow;	
		//i serves as cols 

		// i (the iterator) handles which column we're in. There is never a case in which we remain in the same column
			// after an iteration, so i just serves the purpose of currCol at all times.

	}
}

int main(int argc, char **argv) {
	srand(time(0));

  // bridges object initialization


  // initialize Bridges
  Bridges bridges(4, "jDoug", "1217888151182");

  // defaults for row number and data file
  int startRow = 50;

  // set title
  bridges.setTitle("Mountain Paths - Greedy Algorithms Example");
  bridges.setDescription("Your goal is to find a path that takes you through the points with the lowest elevation changes, in an effort to minimize the overall effort in walking through the path.");

  // get elevation data

  DataSource ds;
  ElevationData elev_data = ds.getElevationData(6.02, 44.10, 9.70, 47.77, 0.02);

  // color grid for visualization
  ColorGrid cg = getImage(elev_data);

  // find path by applying a greedy algorithm
  startRow = elev_data.getRows()/2;
  findPath (elev_data, startRow, cg);

  // visualize
  bridges.setDataStructure(cg);
  bridges.visualize();

  return 0;
}
