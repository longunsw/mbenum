// Copyright 2006-2014 Yun Zhang 
// Department of Computer Science
// University of Tennessee
//
// This file is part of MBEA.                  
//
// MBEA is free software: you can redistribute it and/or modify      
// it under the terms of the GNU General Public License as published by   
// the Free Software Foundation, either version 3 of the License, or   
// (at your option) any later version.   
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of   
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   
// GNU General Public License for more details.   
//
// You should have received a copy of the GNU General Public License   
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/* Enumerate biclique in bipartite graph
 * Author: Yun Zhang
 * Date: September 2006
 */

#ifndef __BICLIQUE_H
#define __BICLIQUE_H

#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <set>

/* ---------------------------------------- *
 * Biclique Enumeration Functions           *
 * ---------------------------------------- */

struct BiCliqueStat
{
	BiCliqueStat()
	{
		sum = 0;
		emaxl = 0;
		emaxr = 0;
		vmaxl = 0;
		vmaxr = 0;
	}
	unsigned long sum;
	unsigned int emaxl;
	unsigned int emaxr;
	unsigned int vmaxl;
	unsigned int vmaxr;
};

class BCE
{
public:

	BCE(BiGraph &bg_, std::string dir_, int v1_min_, int v2_min_) :
			bg(bg_), dir(dir_), v1_min(v1_min_), v2_min(v2_min_), bs(
					BiCliqueStat())
	{
		leftBit.resize(bg.getV1Num());
		fill_n(leftBit.begin(), bg.getV1Num(), -1);
		candBit.resize(bg.getV2Num());
		fill_n(candBit.begin(), bg.getV2Num(), -1);
		dupBit.resize(bg.getV2Num());
		fill_n(dupBit.begin(), bg.getV2Num(), -1);
	}

	void biclique_enumerate();

private:
	void biclique_find_basic(FILE *fp, BiGraph &bg, vid_t *clique, int nc,
			vid_t *left, int nl, vid_t *right, int ne, int ce);
	void biclique_find_improve(FILE *fp, BiGraph &bg, vid_t *clique, int nc,
			vid_t *left, int nl, vid_t *right, int ne, int ce);
	void biclique_find_improve2(FILE *fp, BiGraph &bg, vid_t *clique, int nc,
				vid_t *left, int nl, vid_t *right, int ne, int ce);
	/*void biclique_find(BiGraph &bg, vid_t *left, int nl, vid_t *par, int np,
			vid_t *cand, int nd, int nc, int &level);*/
	void biclique_out(FILE *fp, vid_t *left, int nl, vid_t *right, int nr);
	void biclique_find_pivot(BiGraph &bg, vid_t *clique, int nc, vid_t *left,
			int nl, vid_t *right, int ne, int ce);

	BiGraph &bg;
	std::string dir;
	int v1_min;
	int v2_min;
	BiCliqueStat bs;
	std::vector<int> leftBit;
	std::vector<int> candBit;
	std::vector<int> dupBit;
	std::list<int> adjPrunList;
	std::set<int> adjPrunNodes;
};

#endif

