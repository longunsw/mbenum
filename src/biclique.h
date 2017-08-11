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

	static void biclique_enumerate(std::string dir);

private:
	static void biclique_find_basic(FILE *fp, BiGraph &bg, vid_t *clique,
			int nc, vid_t *left, int nl, vid_t *right, int ne, int ce);
	static void biclique_find_improve(FILE *fp, BiGraph &bg, vid_t *clique,
			int nc, vid_t *left, int nl, vid_t *right, int ne, int ce);
	static void biclique_out(FILE *fp, vid_t *right, int nr, vid_t *left,
			int nl);
	static void biclique_find_pivot(BiGraph &bg, vid_t *clique, int nc,
			vid_t *left, int nl, vid_t *right, int ne, int ce);

	static BiCliqueStat bs;
};

#endif

