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

/* API for Bit-Based Adjacency Matrix for bipartite graphs */
/* Implement functions defined in bigraph.h */
/* Graph Theory Team, Computer Science Department */
/* University of Tennessee, Knoxville */
/* Yun Zhang, yzhang@eecs.utk.edu, December 12, 2004 */
/* Last Updated: 12/18/2014 */

#include "bigraph.h"
#include <string>
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <list>
#include <vector>
#include <unordered_set>
#include <map>
#include <set>
#include "utility.h"
using namespace std;

#define LINE_LENGTH 1000

BiGraph::BiGraph(string dir)
{
	this->dir = dir;
	loadGraph(dir);
}

void BiGraph::print()
{
	string bigraphE = dir + "compressed.e";
	string bigraphMeta = dir + "compressed.meta";

	FILE *graphEF = fopen(bigraphE.c_str(), "w");
	FILE *graphMetaF = fopen(bigraphMeta.c_str(), "w");

	fprintf(graphMetaF, "%d\n%d\n%d\n", num_v1, num_v2, num_edges);
	fclose(graphMetaF);
	for (int i = 0; i < num_v1; ++i)
	{
		for (int j = 0; j < neighbor_v1[i].size(); ++j)
		{
			fprintf(graphEF, "%d %d\n", i, neighbor_v1[i][j]);
		}
	}
	fclose(graphEF);

	/*cout << "\nBiGraph: " << endl;
	 for (int i = 0; i < num_v1; ++i)
	 {
	 cout << i << ": ";
	 for (int j = 0; j < neighbor_v1[i].size(); ++j)
	 {
	 cout << neighbor_v1[i][j] << ", ";
	 }
	 cout << endl;
	 }
	 cout << endl;*/
}

void BiGraph::printSum()
{
	cout << "\nBiGraph Sum: " << endl;
	cout << "num_v1: " << num_v1 << endl;
	cout << "num_v2: " << num_v2 << endl;
	cout << "edge: " << num_edges << endl;

}

void BiGraph::init(unsigned int num1, unsigned int num2)
{
	num_v1 = num1;
	num_v2 = num2;
	num_edges = 0;

	neighbor_v1.resize(num_v1);
	neighbor_v2.resize(num_v2);

	degree_v1.resize(num_v1);
	degree_v2.resize(num_v2);

	fill_n(degree_v1.begin(), num_v1, 0);
	fill_n(degree_v2.begin(), num_v2, 0);
}

void BiGraph::loadGraph(string dir)
{
	unsigned int n1, n2;
	unsigned int edges = 0;
	int u, v;
	int r;

	string metaFile = dir + "graph.meta";
	string edgeFile = dir + "graph.e";

	FILE * metaGraph = fopen(metaFile.c_str(), "r");
	FILE * edgeGraph = fopen(edgeFile.c_str(), "r");

	if (fscanf(metaGraph, "%d\n%d", &n1, &n2) != 2)
	{
		fprintf(stderr, "Bad file format: n1 n2 incorrect\n");
		exit(1);
	}

	fprintf(stdout, "n1: %d, n2: %d\n", n1, n2);

	init(n1, n2);

	while ((r = fscanf(edgeGraph, "%d %d", &u, &v)) != EOF)
	{
		//fprintf(stderr, "%d, %d\n", u, v);
		if (r != 2)
		{
			fprintf(stderr, "Bad file format: u v incorrect\n");
			exit(1);
		}

		addEdge(u, v);
		num_edges++;
	}

	fclose(metaGraph);
	fclose(edgeGraph);

	for (int i = 0; i < num_v1; ++i)
	{
		neighbor_v1[i].shrink_to_fit();
		sort(neighbor_v1[i].begin(), neighbor_v1[i].end());

	}
	for (int i = 0; i < num_v2; ++i)
	{
		neighbor_v2[i].shrink_to_fit();
		sort(neighbor_v2[i].begin(), neighbor_v2[i].end());
	}

}

void BiGraph::addEdge(vid_t u, vid_t v)
{

	neighbor_v1[u].push_back(v);
	++degree_v1[u];
	neighbor_v2[v].push_back(u);
	++degree_v2[v];
}

void BiGraph::deleteEdge(vid_t u, vid_t v)
{
	for (int i = 0; i < degree_v1[u]; ++i)
	{
		int vv = neighbor_v1[u][i];
		if (vv == v)
		{
			swap(neighbor_v1[u][i], neighbor_v1[u][degree_v1[u] - 1]);
			--degree_v1[u];
		}
	}

	for (int i = 0; i < degree_v2[v]; ++i)
	{
		int uu = neighbor_v2[v][i];
		if (uu == u)
		{
			swap(neighbor_v2[v][i], neighbor_v2[v][degree_v2[v] - 1]);
			--degree_v2[v];
		}
	}

}

bool BiGraph::isEdge(vid_t u, vid_t v)
{
	if (binary_search(neighbor_v1[u].begin(),
			neighbor_v1[u].begin() + degree_v1[u], v))
		return true;
	else
		return false;
}

void BiGraph::pruneCore(num_t v1_min, num_t v2_min)
{

	cout << "v1_min: " << v1_min << endl;
	cout << "v2_min: " << v2_min << endl;
	int v1MaxDegree = *max_element(degree_v1.begin(), degree_v1.end());
	int v2MaxDegree = *max_element(degree_v2.begin(), degree_v2.end());

	cout << "v1 max degree: " << v1MaxDegree << endl;
	cout << "v1 max degree: " << v2MaxDegree << endl;

	/*core_v1.resize(num_v1);
	 core_v2.resize(num_v2);
	 fill_n(core_v1.begin(), num_v1, 0);
	 fill_n(core_v2.begin(), num_v2, 0);*/

	vector<list<vid_t>> v1pool;
	vector<list<vid_t>> v2pool;

	v1pool.resize(v1MaxDegree + 1);
	v2pool.resize(v2MaxDegree + 1);

	vector<int> tmpV1Degree;
	vector<int> tmpV2Degree;
	tmpV1Degree.resize(num_v1);
	tmpV2Degree.resize(num_v2);

	vector<list<vid_t>::iterator> v1Loc;
	vector<list<vid_t>::iterator> v2Loc;
	v1Loc.resize(num_v1);
	v2Loc.resize(num_v2);

	vector<int> prunedV1;
	vector<int> prunedV2;
	prunedV1.resize(num_v1);
	prunedV2.resize(num_v2);
	fill_n(prunedV1.begin(), num_v1, 0);
	fill_n(prunedV2.begin(), num_v2, 0);

	for (int i = 0; i < num_v1; ++i)
	{
		int degree = degree_v1[i];
		tmpV1Degree[i] = degree;
		v1Loc[i] = v1pool[degree].insert(v1pool[degree].begin(), i);
	}

	for (int i = 0; i < num_v2; ++i)
	{
		int degree = degree_v2[i];
		tmpV2Degree[i] = degree;
		v2Loc[i] = v2pool[degree].insert(v2pool[degree].begin(), i);
	}

	int curV1Degree = 0;
	int curV2Degree = 0;
	while (curV1Degree < v1_min || curV2Degree < v2_min)
	{
		while (curV1Degree < v1_min)
		{
			while (!v1pool[curV1Degree].empty())
			{
				int u = v1pool[curV1Degree].front();
				//cout << "v1: " << u << endl;
				v1pool[curV1Degree].pop_front();
				prunedV1[u] = -1;
				tmpV1Degree[u] = -1;
				for (int i = 0; i < neighbor_v1[u].size(); ++i)
				{
					int v = neighbor_v1[u][i];
					if (tmpV2Degree[v] > -1)
					{
						v2pool[tmpV2Degree[v]].erase(v2Loc[v]);
						tmpV2Degree[v]--;
						if (tmpV2Degree[v] > -1)
						{
							curV2Degree = min(curV2Degree,
									(int) tmpV2Degree[v]);
							v2Loc[v] = v2pool[tmpV2Degree[v]].insert(
									v2pool[tmpV2Degree[v]].begin(), v);
						}

					}
				}
			}
			curV1Degree++;
		}

		//cout<<"curV1Degree: "<<curV1Degree<<", curV2Degree: "<<curV2Degree<<endl;

		while (curV2Degree < v2_min)
		{
			while (!v2pool[curV2Degree].empty())
			{
				int u = v2pool[curV2Degree].front();
				v2pool[curV2Degree].pop_front();
				prunedV2[u] = -1;
				tmpV2Degree[u] = -1;
				for (int i = 0; i < neighbor_v2[u].size(); ++i)
				{
					int v = neighbor_v2[u][i];
					if (tmpV1Degree[v] > -1)
					{
						v1pool[tmpV1Degree[v]].erase(v1Loc[v]);
						tmpV1Degree[v]--;
						if (tmpV1Degree[v] > -1)
						{
							curV1Degree = min(curV1Degree,
									(int) tmpV1Degree[v]);
							v1Loc[v] = v1pool[tmpV1Degree[v]].insert(
									v1pool[tmpV1Degree[v]].begin(), v);
						}

					}
				}
			}
			curV2Degree++;
		}

	}

	compressGraph(prunedV1, prunedV2);

}

void BiGraph::pruneSquare(num_t v1_min, num_t v2_min)
{
	map<Edge, int> support;
	map<Edge, set<int>> adjEdge;
	set<int> v2hop;

	vector<int> squareNum;
	squareNum.resize(num_v1);
	fill_n(squareNum.begin(), num_v1, 0);

	int minSupport = (v1_min - 1) * (v2_min - 1);
	int edges = 0;

	for (int u = 0; u < num_v1; ++u)
	{
		for (int j = 0; j < neighbor_v1[u].size(); ++j)
		{
			int v = neighbor_v1[u][j];
			support[Edge(u, v)] = 0;
		}
	}

	for (int u = 0; u < num_v1; ++u)
	{
		for (int j = 0; j < neighbor_v1[u].size(); ++j)
		{
			int v = neighbor_v1[u][j];
			for (int k = 0; k < neighbor_v2[v].size(); ++k)
			{
				int uu = neighbor_v2[v][k];
				if (uu != u)
				{
					adjEdge[Edge(u, uu)].insert(v);
					v2hop.insert(uu);
				}
			}
		}

		for (auto it = v2hop.begin(); it != v2hop.end(); ++it)
		{
			int uu = *it;
			Edge e(u, uu);
			squareNum[u] += (adjEdge[e].size() * (adjEdge[e].size() - 1));

			for (auto it = adjEdge[e].begin(); it != adjEdge[e].end(); ++it)
			{
				int v = *it;
				support[Edge(u, v)] += adjEdge[e].size() - 1;

			}
		}
		v2hop.clear();
		adjEdge.clear();
	}

//	for (auto it = support.begin(); it != support.end(); ++it)
//	{
//		cout << "(" << it->first.u << ", " << it->first.v << ") -> "
//				<< it->second << endl;
//
//	}

	for (auto it = support.begin(); it != support.end(); ++it)
	{
		if (it->second < minSupport)
		{
			deleteEdge(it->first.u, it->first.v);
			edges++;
		}

	}

	int totalSquare = 0;
	for (int i = 0; i < num_v1; ++i)
		totalSquare += squareNum[i];

	vector<int> prunedV1;
	vector<int> prunedV2;
	prunedV1.resize(num_v1);
	prunedV2.resize(num_v2);
	fill_n(prunedV1.begin(), num_v1, 0);
	fill_n(prunedV2.begin(), num_v2, 0);

	for (int i = 0; i < num_v1; ++i)
	{
		if (degree_v1[i] == 0)
			prunedV1[i] = -1;
	}

	for (int i = 0; i < num_v2; ++i)
	{
		if (degree_v2[i] == 0)
			prunedV2[i] = -1;
	}

	compressGraph(prunedV1, prunedV2);

	cout << "number of edges pruned: " << edges << endl;
	cout << "total Square: " << totalSquare << endl;
}

void BiGraph::pruneSquareDyn(num_t v1_min, num_t v2_min)
{
	map<Edge, int> support;
	map<Edge, set<int>> adjEdge;
	set<int> v2hop;

	vector<int> squareNum;
	squareNum.resize(num_v1);
	fill_n(squareNum.begin(), num_v1, 0);

	int minSupport = (v1_min - 1) * (v2_min - 1);
	int edges = 0;

	for (int u = 0; u < num_v1; ++u)
	{
		for (int j = 0; j < neighbor_v1[u].size(); ++j)
		{
			int v = neighbor_v1[u][j];
			support[Edge(u, v)] = 0;
		}
	}

	for (int u = 0; u < num_v1; ++u)
	{
		for (int j = 0; j < neighbor_v1[u].size(); ++j)
		{
			int v = neighbor_v1[u][j];
			for (int k = 0; k < neighbor_v2[v].size(); ++k)
			{
				int uu = neighbor_v2[v][k];
				if (uu != u)
				{
					adjEdge[Edge(u, uu)].insert(v);
					v2hop.insert(uu);
				}
			}
		}

		for (auto it = v2hop.begin(); it != v2hop.end(); ++it)
		{
			int uu = *it;
			Edge e(u, uu);
			squareNum[u] += (adjEdge[e].size() * (adjEdge[e].size() - 1));

			for (auto it = adjEdge[e].begin(); it != adjEdge[e].end(); ++it)
			{
				int v = *it;
				support[Edge(u, v)] += adjEdge[e].size() - 1;

			}
		}
		v2hop.clear();
		adjEdge.clear();
	}

}

void BiGraph::compressGraph(vector<int> &prunedV1, vector<int> &prunedV2)
{
	num_t order = 0;
	num_t n_num_v1 = 0;
	num_t n_num_v2 = 0;
	num_t n_edges = 0;

	for (int i = 0; i < num_v1; ++i)
	{
		if (prunedV1[i] != -1)
		{
			prunedV1[i] = order++;
		}
	}
	n_num_v1 = order;
	order = 0;
	for (int i = 0; i < num_v2; ++i)
	{
		if (prunedV2[i] != -1)
		{
			prunedV2[i] = order++;
		}
	}
	n_num_v2 = order;
	vector<vector<vid_t>> n_neighbor_v1;
	n_neighbor_v1.resize(n_num_v1);
	vector<vector<vid_t>> n_neighbor_v2;
	n_neighbor_v2.resize(n_num_v2);

	vector<int> n_degree_v1;
	n_degree_v1.resize(n_num_v1);
	vector<int> n_degree_v2;
	n_degree_v2.resize(n_num_v2);

	for (int i = 0; i < num_v1; ++i)
	{
		if (prunedV1[i] != -1)
		{
			int u = prunedV1[i];
			for (int j = 0; j < degree_v1[i]; ++j)
			{
				int v = neighbor_v1[i][j];
				if (prunedV2[v] != -1)
				{
					n_neighbor_v1[u].push_back(prunedV2[v]);
					n_edges++;
				}
			}
		}
	}

	for (int i = 0; i < n_num_v1; ++i)
	{
		for (int j = 0; j < n_neighbor_v1[i].size(); ++j)
		{
			n_neighbor_v2[n_neighbor_v1[i][j]].push_back(i);
		}
	}

	for (int i = 0; i < n_num_v1; ++i)
	{
		n_neighbor_v1[i].shrink_to_fit();
		sort(n_neighbor_v1[i].begin(), n_neighbor_v1[i].end());
		n_degree_v1[i] = n_neighbor_v1[i].size();

	}
	for (int i = 0; i < n_num_v2; ++i)
	{
		n_neighbor_v2[i].shrink_to_fit();
		sort(n_neighbor_v2[i].begin(), n_neighbor_v2[i].end());
		n_degree_v2[i] = n_neighbor_v2[i].size();
	}

	num_v1 = n_num_v1;
	num_v2 = n_num_v2;
	num_edges = n_edges;
	swap(neighbor_v1, n_neighbor_v1);
	swap(neighbor_v2, n_neighbor_v2);
	swap(degree_v1, n_degree_v1);
	swap(degree_v2, n_degree_v2);

	for (int i = 0; i < num_v1; ++i)
	{
		if (neighbor_v1[i].size() != degree_v1[i])
			cout << "degree error" << endl;
	}

	for (int i = 0; i < num_v2; ++i)
	{
		if (neighbor_v2[i].size() != degree_v2[i])
			cout << "degree error" << endl;
	}

	cout<<"degree correct"<<endl;

}
