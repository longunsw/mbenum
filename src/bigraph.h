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

/* API for Bit-Based Adjacency Matrix for undirected bipartite graphs */
/* Graph Theory Team, Computer Science Department */ 
/* University of Tennessee, Knoxville */
/* Yun Zhang, yzhang@cs.utk.edu, September 2006 */
/* Updated: 12/17/2013 */

#ifndef __BIGRAPH_H
#define __BIGRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "utility.h"

class Edge
{
public:
	Edge(int u_, int v_){u = u_; v = v_;}
	bool operator<(const Edge &other) const
	{
		if(u == other.u)
			return v < other.v;
		return u < other.u;
	}

	int u;
	int v;
};


class DegreeNode
{
public:
	int id;
	int degree;
};

class BiGraph
{

public:

	BiGraph(std::string dir);

	~BiGraph(){}

	void addEdge(vid_t u, vid_t v);
	void deleteEdge(vid_t u, vid_t v);
	bool isEdge(vid_t u, vid_t v);
	num_t getV1Num() {return num_v1;}
	num_t getV2Num() {return num_v2;}
	num_t getV1Degree(vid_t u) {return degree_v1[u];}
	num_t getV2Degree(vid_t u) {return degree_v2[u];}
	void print();
	void printSum();
	void printCout();


	void pruneCore(num_t v1_min, num_t v2_min);
	void pruneSquare(num_t v1_min, num_t v2_min);
	void pruneSquareEdge(num_t v1_min, num_t v2_min);
	void pruneSquareDyn(num_t v1_min, num_t v2_min);
	void pruneSquareNode(num_t v1_min, num_t v2_min);

private:

	void init(unsigned int num_v1, unsigned int num_v2);
	void loadGraph(std::string dir);
	void compressGraph(std::vector<int> &prunedV1, std::vector<int> &prunedV2);

	std::string dir;
	num_t num_v1;
	num_t num_v2;
	num_t num_edges;

	std::vector<std::vector<vid_t>> neighbor_v1;
	std::vector<std::vector<vid_t>> neighbor_v2;

	std::vector<int> degree_v1;
	std::vector<int> degree_v2;

	std::vector<num_t> core_v1;
	std::vector<num_t> core_v2;
};

#endif  /* __BIGRAPH_H */

