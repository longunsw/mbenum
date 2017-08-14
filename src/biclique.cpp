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

/* Enumerate biclique in bipartite graphs
 * Author: Yun Zhang
 * Created: September 2006
 * Last Update: August 2007
 */

#include "bigraph.h"
#include "biclique.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <algorithm>
#include "utility.h"

using namespace std;

#define LMIN 2

/* Global Variables */
int LLEAST, RLEAST;
int VERSION;
int PRINT;

//typedef unsigned long num_t;

#ifdef PERFORMANCE
long long node_num;
double time_check, time_expand, time_out, time_sort;
#endif

void BCE::biclique_enumerate()
{

	//string output = dir + "bg.txt";
	//FILE *fp1 = fopen(output.c_str(), "w");
	FILE *fp1 = stdout;

	num_t n1 = bg.getV1Num();
	num_t n2 = bg.getV2Num();
	vid_t left[n1], right[n2], clique[n2];
	vid_t u, v;

	memset(clique, -1, n2 * sizeof(vid_t));  // initially Clique is empty
	for (u = 0; u < n1; u++)
		left[u] = u;  // every left vertex is candidate
	for (v = 0; v < n2; v++)
		right[v] = v; // every right vertex is candidate

	cerr << "here" << endl;
	/* Call the recursive function to find maximal bicliques */
	if (VERSION == 1)
	{
		cerr << "version 1" << endl;
		biclique_find_basic(fp1, bg, clique, 0, left, n1, right, 0, n2);
	}
	else if (VERSION == 2)
	{
		/* Sort the candidate right vertices */
		cerr << "version 2" << endl;
		sort(right, right + n2, [this](int a, int b)
		{	return bg.getV2Degree(a) < bg.getV2Degree(b);});
		cerr << "begin find..." << endl;
		biclique_find_improve(fp1, bg, clique, 0, left, n1, right, 0, n2);
	}
	else if (VERSION == 3)
	{
		cerr << "version 3" << endl;
		sort(right, right + n2, [this](int a, int b)
		{	return this->bg.getV2Degree(a) < this->bg.getV2Degree(b);});
		cerr << "begin find..." << endl;
		int level = 0;
		biclique_find_improve(fp1, bg, clique, 0, left, n1, right, 0, n2);

	}

	cout << "Number of bicliques         : " << bs.sum << endl;
	cout << "Size of edge max biclique   : (" << bs.emaxl << ", " << bs.emaxr
			<< ")\n";
	cout << "Size of vertex max biclique   : (" << bs.vmaxl << ", " << bs.vmaxr
			<< ")\n";
	//fclose(fp1);
	return;

}

/* ------------------------------------------------------------- *
 * Function: biclique_out()                                      *
 *   Print out a biclique: left and right vertices on two lines  *
 * ------------------------------------------------------------- */
void BCE::biclique_out(FILE *fp, vid_t *left, int nl, vid_t *right, int nr)
{
	bs.sum++;

	if (nl * nr > bs.emaxl * bs.emaxr)
	{
		bs.emaxl = nl;
		bs.emaxr = nr;
	}
	if (nl + nr > bs.vmaxl + bs.vmaxr)
	{
		bs.vmaxl = nl;
		bs.vmaxr = nr;
	}
	if (bs.sum % 1000 == 0)
	{
		cout << "Number of bicliques: " << bs.sum << endl;
	}
	/*if (bs.sum % 1000 == 0)
	 {
	 cout << "Number of bicliques         : " << bs.sum << endl;
	 cout << "Size of edge max biclique   : (" << bs.emaxl << ", "
	 << bs.emaxr << ")\n";
	 cout << "Size of vertex max biclique   : (" << bs.vmaxl << ", "
	 << bs.vmaxr << ")\n";
	 }*/

	/*int i;

	for (i = 0; i < nl - 1; i++)
	{
		fprintf(fp, "%d\t", left[i]);
	}
	fprintf(fp, "%d\n", left[i]);

	for (i = 0; i < nr - 1; i++)
	{
		fprintf(fp, "%d\t", right[i]);
	}
	fprintf(fp, "%d\n", right[i]);
	fprintf(fp, "\n");*/
}

/* ------------------------------------------------------------- *
 * Function: searchtreenode_out()                                *
 *   Print out the elements in biclique, candidates, and former  *
 *   candidates, in both left and right.                         *
 * ------------------------------------------------------------- */
void searchtreenode_out(vid_t *clique, vid_t *right, vid_t *left, int nc,
		int ne, int ce, int nl)
{
	int i;
	for (i = 0; i < nc; i++)
		printf(" %d", clique[i]);
	printf("\t|");
	for (i = 0; i < ne; i++)
		printf(" %d", right[i]);
	printf("\t|");
	for (i = ne; i < ce; i++)
		printf(" %d", right[i]);
	printf("\t|");
	for (i = 0; i < nl; i++)
		printf(" %d", left[i]);
	printf("\n");
}

void searchtreenode_out2(vid_t *clique, vid_t *right, vid_t *left,
		vid_t *old_right, vid_t w, int nc, int ne, int ce, int nl, int old_ne)
{
	int i;
	for (i = 0; i < nc; i++)
		printf(" %d", clique[i]);
	printf("\t|");
	for (i = 0; i < ne; i++)
		printf(" %d", right[i]);
	printf(" %d", w);
	printf("\t|");
	for (i = old_ne + 1; i < ce; i++)
		printf(" %d", old_right[i]);
	printf("\t|");
	for (i = 0; i < nl; i++)
		printf(" %d", left[i]);
	printf("\n");
}

/* --------------------------------------------------- *
 *  Function: biclique_find_basic()
 *    The basic version of biclique_find().
 * --------------------------------------------------- */
void BCE::biclique_find_basic(FILE *fp, BiGraph &bg, vid_t *clique, int nc,
		vid_t *left, int nl, vid_t *right, int ne, int ce)
{
	num_t n1 = bg.getV1Num();
	vid_t new_left[nl];
	vid_t new_right[ce];
	vid_t u, v, w, j, k;
	int new_nc, new_nl, new_ne, new_ce;
	int count, is_maximal = 1;
#ifdef PERFORMANCE
	double utime;
#endif

	while (ne < ce)
	{

		/* Choose one vertex from candidate set */
		v = right[ne];

#ifdef DEBUG
		searchtreenode_out(G, clique, right, left, nc, ne, ce, nl);
#endif

#ifdef PERFORMANCE
		node_num++;
		utime = get_cur_time();
#endif

		/* Set right vertices in clique */
		new_nc = nc;
		clique[new_nc++] = v;

		/* Set neighbors on left */
		new_nl = 0;
		for (j = 0; j < nl; j++)
		{
			u = left[j];
			if (bg.isEdge(u, v))
				new_left[new_nl++] = u;
		}

#ifdef PERFORMANCE
		time_expand += get_cur_time() - utime;
		utime = get_cur_time();
#endif

		/* Set right vertices in not */
		new_ne = 0;
		is_maximal = 1;
		for (j = 0; j < ne; j++)
		{
			w = right[j];
			count = 0;
			for (k = 0; k < new_nl; k++)
			{
				u = new_left[k];
				if (bg.isEdge(u, w))
					count++;
			}
			if (count == new_nl)
			{
				is_maximal = 0;
				break;
			}
			else if (count > 0)
				new_right[new_ne++] = w;
		}

		/* Stop this branch if it is not maximal */
		if (!is_maximal)
		{
#ifdef DEBUG
			searchtreenode_out2(G, clique, new_right, new_left, right, w,
					new_nc, new_ne, new_ce, new_nl, ne);
#endif
			ne++;
			continue;
		}

#ifdef PERFORMANCE
		time_check += get_cur_time() - utime;
		utime = get_cur_time();
#endif

		/* Set right vertices in cand */
		new_ce = new_ne;
		for (j = ne + 1; j < ce; j++)
		{
			w = right[j];
			count = 0;
			for (k = 0; k < new_nl; k++)
			{
				u = new_left[k];
				if (bg.isEdge(u, w))
					count++;
			}
			if (count == new_nl)
			{
				clique[new_nc++] = w;
			}
			else if (count > 0)
			{
				new_right[new_ce++] = w;
			}
		}

#ifdef PERFORMANCE
		time_expand += get_cur_time() - utime;
#endif

		/* Print out the found maximal biclique */
		if (new_nc >= RLEAST && new_nl >= LLEAST)
		{
			biclique_out(fp, clique, new_nc, new_left, new_nl);
		}

		/* Recursively find bicliques */
		if ((new_ne < new_ce) && (new_nc + (new_ce - new_ne) >= RLEAST))
		{
			biclique_find_basic(fp, bg, clique, new_nc, new_left, new_nl,
					new_right, new_ne, new_ce);
		}
#ifdef DEBUG
		else
		{
			searchtreenode_out(G, clique, new_right, new_left,
					new_nc, new_ne, new_ce, new_nl);
		}
#endif

		/* Move v to former candidate set */
		ne++;

	}

	return;
}

/* --------------------------------------------------- *
 *  Function: biclique_find_improve()
 *    The Improved Version of biclique_find().
 * --------------------------------------------------- */
void BCE::biclique_find_improve(FILE *fp, BiGraph &bg, vid_t *clique, int nc,
		vid_t *left, int nl, vid_t *right, int ne, int ce)
{
	//unsigned int n1 = bg.getV1Num();
	vid_t new_left[nl];
	vid_t new_right[ce];
	vid_t u, v, w, j, k;
	int new_nc, new_nl, new_ne, new_ce;
	int count, is_maximal = 1;
	int x, noc[ce - ne];
#ifdef PERFORMANCE
	double utime;
#endif
	// Improvement II
	// divide new_left to two parts: in L' and not in L'
	// ----------------------------
	// |  L'          |   not L'  |
	// ----------------------------
	// ^              ^           ^
	// new_nl ->          <- not_nl
	// L'=new_left[0..new_nl-1], ~L'=new_left[new_nl,nl-1]
	int not_nl;  // position of not_left
	int nn;      // number of vertices directly go to not
	// End

	/* Same operations as v2 on each candidate in order */
	while (ne < ce)
	{

		/* Choose the next candidate in P */
		v = right[ne];

#ifdef DEBUG
		searchtreenode_out(G, clique, right, left, nc, ne, ce, nl);
#endif

#ifdef PERFORMANCE
		node_num++;
		utime = get_cur_time();
#endif

		/* Choose one vertex from candidate set */
		new_nc = nc;
		clique[new_nc++] = v;

		/* Set right vertices in clique */
		new_nl = 0;
		not_nl = nl;
		for (j = 0; j < nl; j++)
		{
			u = left[j];
			if (bg.isEdge(u, v))
				new_left[new_nl++] = u;
			else
				new_left[not_nl--] = u;
		}

#ifdef PERFORMANCE
		time_expand += get_cur_time() - utime;
		utime = get_cur_time();
#endif

		/* Set right vertices in not */
		new_ne = 0;
		is_maximal = 1;
		for (j = 0; j < ne; j++)
		{
			w = right[j];
			count = 0;
			for (k = 0; k < new_nl; k++)
			{
				u = new_left[k];
				if (bg.isEdge(u, w))
					count++;
			}
			if (count == new_nl)
			{
				is_maximal = 0;
				break;
			}
			else if (count > 0)
				new_right[new_ne++] = w;
		}

		/* Stop this branch if it is not maximal */
		if (!is_maximal)
		{
#ifdef DEBUG
			searchtreenode_out2(G, clique, new_right, new_left, right, w,
					new_nc, new_ne, new_ce, new_nl, ne);
#endif
			ne++;
			continue;
		}

#ifdef PERFORMANCE
		time_check += get_cur_time() - utime;
		utime = get_cur_time();
#endif

		/* Set right vertices in cand */
		new_ce = new_ne;
		nn = 1; // number of vertice will be put in not when backtracking
		for (j = ne + 1; j < ce; j++)
		{
			w = right[j];
			/* count the connections to L */
			count = 0;
			for (k = 0; k < new_nl; k++)
			{
				u = new_left[k];
				if (bg.isEdge(u, w))
					count++;
			}
			if (count == new_nl)
			{
				clique[new_nc++] = w;
				// Improvement II
				for (k = nl; k > not_nl; k--)
				{
					u = new_left[k];
					if (bg.isEdge(u, w))
						count++;
				}
				// switch such vertex with the one next to
				// the last picked vertex to biclique
				if (count == new_nl)
				{
					right[j] = right[ne + nn];
					right[ne + nn] = w;
					nn++;
				}
			}
			else if (count > 0)
			{
				/* Improvement I: Sorting candidates in new_right */
				x = new_ce - 1;
				while (x >= new_ne && noc[x - new_ne] > count)
				{
					noc[x + 1 - new_ne] = noc[x - new_ne];
					new_right[x + 1] = new_right[x];
					x--;
				}
				noc[x + 1 - new_ne] = count;
				new_right[x + 1] = w;
				new_ce++;
			}
		}

#ifdef PERFORMANCE
		time_expand += get_cur_time() - utime;
#endif

		/* Print out the found maximal biclique */
		if (new_nc >= RLEAST && new_nl >= LLEAST)
		{
			biclique_out(fp, clique, new_nc, new_left, new_nl);
		}

		/* Recursively find bicliques */
		if ((new_ne < new_ce) && (new_nc + (new_ce - new_ne) >= RLEAST))
		{
			biclique_find_improve(fp, bg, clique, new_nc, new_left, new_nl,
					new_right, new_ne, new_ce);
		}
		else
		{
#ifdef DEBUG
			searchtreenode_out(G, clique, new_right, new_left,
					new_nc, new_ne, new_ce, new_nl);
#endif
		}

		/* Move v and other qualified vertics to former candidate set */
		ne += nn;

	}

	return;
}

void BCE::biclique_find_improve2(FILE *fp, BiGraph &bg, vid_t *clique, int nc,
		vid_t *left, int nl, vid_t *right, int ne, int ce)
{
	vid_t new_left[nl];
	vid_t new_right[ce];
	vid_t u, v, w, j, k;
	int new_nc, new_nl, new_ne, new_ce;
	int count, is_maximal = 1;

	int not_nl;  // position of not_left
	int nn;      // number of vertices directly go to not
	// End

	/* Same operations as v2 on each candidate in order */
	while (ne < ce)
	{

		/* Choose the next candidate in P */
		v = right[ne];

		/* Choose one vertex from candidate set */
		new_nc = nc;
		clique[new_nc++] = v;

		/* Set right vertices in clique */
		new_nl = 0;
		not_nl = nl;

		for (j = 0; j < nl; ++j)
		{
			leftBit[left[j]] = 0;
		}

		auto vNeighbors = bg.getV2Neighbors(v);
		for (int i = 0; i < vNeighbors.size(); ++i)
		{
			int u = vNeighbors[i];
			if (leftBit[u] == 0)
				new_left[new_nl++] = u;
			else
				new_left[not_nl--] = u;
		}

		/* Set right vertices in not */
		new_ne = 0;
		is_maximal = 1;

		for (int i = 0; i < ne; ++i)
		{
			dupBit[right[i]] = 0;
		}

		for (k = 0; k < new_nl; ++k)
		{
			v = new_left[k];
			auto vNeighbors = bg.getV1Neighbors(v);
			for (int i = 0; i < vNeighbors.size(); ++i)
			{
				u = vNeighbors[i];
				if (dupBit[u] > -1)
					dupBit[u]++;
			}
		}

		for (j = 0; j < ne; ++j)
		{
			u = right[j];
			if (dupBit[u] == new_nl)
			{
				is_maximal = 0;
				break;
			}
			else if (dupBit[u] > 0)
			{
				new_right[new_ne++] = u;
			}
		}

		/* Stop this branch if it is not maximal */
		if (!is_maximal)
		{
			ne++;
			continue;
		}

		/* Set right vertices in cand */
		new_ce = new_ne;
		nn = 1; // number of vertice will be put in not when backtracking

		for (j = ne + 1; j < ce; ++j)
		{
			candBit[right[j]] = 0;
		}

		for (k = 0; k < new_nl; ++k)
		{
			v = new_left[k];
			auto vNeighbors = bg.getV1Neighbors(v);
			for (int i = 0; i < vNeighbors.size(); ++i)
			{
				u = vNeighbors[i];
				if (candBit[u] > -1)
					candBit[u]++;
			}
		}

		for (j = ne + 1; j < ce; ++j)
		{
			w = right[j];
			if (candBit[w] == new_nl)
			{
				clique[new_nc++] = w;
				for (k = nl; k > not_nl; k--)
				{
					u = new_left[k];
					if (bg.isEdge(u, w))
						count++;
				}
				// switch such vertex with the one next to
				// the last picked vertex to biclique
				if (count == new_nl)
				{
					right[j] = right[ne + nn];
					right[ne + nn] = w;
					nn++;
				}

			}
			else if (candBit[w] > 0)
			{
				/*x = new_ce - 1;
				while (x >= new_ne && noc[x - new_ne] > count)
				{
					noc[x + 1 - new_ne] = noc[x - new_ne];
					new_right[x + 1] = new_right[x];
					x--;
				}
				noc[x + 1 - new_ne] = count;
				new_right[x + 1] = w;*/
				new_right[new_ce++] = w;
			}

		}

		sort(new_right, new_right+new_ce, [this](int a, int b){return this->candBit[a] < this->candBit[b];});

		for (j = 0; j < nl; ++j)
		{
			leftBit[left[j]] = -1;
		}

		for (j = ne + 1; j < ce; ++j)
		{
			candBit[right[j]] = -1;
		}

		for (int i = 0; i < ne; ++i)
		{
			dupBit[right[i]] = -1;
		}

		/* Print out the found maximal biclique */
		if (new_nc >= RLEAST && new_nl >= LLEAST)
		{
			biclique_out(fp, clique, new_nc, new_left, new_nl);
		}

		/* Recursively find bicliques */
		if (new_nl >= LLEAST && (new_ne < new_ce) && (new_nc + (new_ce - new_ne) >= RLEAST))
		{
			biclique_find_improve2(fp, bg, clique, new_nc, new_left, new_nl,
					new_right, new_ne, new_ce);
		}


		for(int i = 0; i < nn; ++i)
		{
			v = right[ne+i];
			auto vNeighbors = bg.getV2Neighbors(v);
			for(j = 0; j < vNeighbors.size(); ++j)
			{
				u = vNeighbors[j];
				auto uNeighbors = bg.getV1Neighbors(u);
				for(k = 0; k < uNeighbors.size(); ++k)
				{
					int vv = uNeighbors[u];
					adjPrunNodes.insert(vv);
				}
			}
		}

		adjPrunNodes.insert(right[ne]);

		nn = 0;
		for(int i = ne; i < ce; ++i)
		{
			adjPrunList.push_back(right[ne]);
		}

		for(auto it = adjPrunList.begin(); it != adjPrunList.end();)
		{
			if(adjPrunNodes.find(*it) != adjPrunNodes.end())
			{
				it = adjPrunList.erase(it);
				right[ne+nn] = *it;
				nn++;

			}
			else
				++it;
		}

		ne += nn;

		int i = 0;
		for(auto it = adjPrunList.begin(); it != adjPrunList.end(); ++it)
		{
			right[ne+i] = *it;
			i++;
		}

		adjPrunList.clear();
		adjPrunNodes.clear();

	}

	return;
}

/*void BCE::biclique_find(BiGraph &bg, vid_t *left, int nl, vid_t *par, int np,
 vid_t *cand, int nc, int nd, int &level)
 {

 cout << "level: " << level << endl;
 cout << "left, " << nl << ": ";
 for (int i = 0; i < nl; ++i)
 cout << left[i] << " ";
 cout << endl;
 cout << "par, " << np << ": ";
 for (int i = 0; i < np; ++i)
 cout << par[i] << " ";
 cout << endl;
 cout << "cand, " << nc << ": ";
 for (int i = 0; i < nc; ++i)
 cout << cand[i] << " ";
 cout << endl;
 cout << "dup, " << nd - nc << ": ";
 for (int i = nc; i < nd; ++i)
 cout << cand[i] << " ";
 cout << endl;
 cout << "leftBit: ";
 for (int i = 0; i < bg.getV1Num(); ++i)
 if (leftBit[i] == 0)
 cout << "0";
 else
 cout << "1";
 cout << endl;
 cout << "candBit: ";
 for (int i = 0; i < bg.getV2Num(); ++i)
 if (candBit[i] == 0)
 cout << "0";
 else
 cout << "1";
 cout << endl;
 cout << "dupBit: ";
 for (int i = 0; i < bg.getV2Num(); ++i)
 if (dupBit[i] == 0)
 cout << "0";
 else
 cout << "1";
 cout << endl << endl;

 if (nc == 0 && nc == nd)
 {
 biclique_out(stdout, left, nl, par, np);
 return;
 }

 if (nc == 0)
 {
 return;
 }

 vid_t new_left[nl];
 vid_t new_cand[nd];
 vid_t u, v, vv;
 int new_nc, new_nl, new_np, new_nd;

 while (nc > 0)
 {
 v = cand[0];

 new_np = np;
 par[new_np++] = v;

 new_nl = 0;

 candBit[v] = 0;
 auto vNeighbors = bg.getV2Neighbors(v);
 for (int i = 0; i < vNeighbors.size(); ++i)
 {
 u = vNeighbors[i];
 if (leftBit[u] == 1)
 new_left[new_nl++] = u;
 }

 for (int i = 0; i < nl; ++i)
 {
 leftBit[left[i]] = 0;
 }

 for (int i = 0; i < new_nl; ++i)
 {
 leftBit[new_left[i]] = 1;
 }

 new_nc = 0;
 for (int i = 0; i < new_nl; ++i)
 {
 u = new_left[i];
 auto uNeighbors = bg.getV1Neighbors(u);
 for (int j = 0; j < uNeighbors.size(); ++j)
 {
 vv = uNeighbors[j];
 if (candBit[vv] == 1)
 {
 new_cand[new_nc++] = vv;
 candBit[vv] = 0;
 }
 }
 }

 new_nd = new_nc;
 for (int i = 0; i < new_nl; ++i)
 {
 u = new_left[i];
 auto uNeighbors = bg.getV1Neighbors(u);
 for (int j = 0; j < uNeighbors.size(); ++j)
 {
 vv = uNeighbors[j];
 if (level == 2)
 {
 cout << "vv: " << vv << " ";
 cout << "dupBit: ";
 for (int i = 0; i < bg.getV2Num(); ++i)
 if (dupBit[i] == 0)
 cout << "0";
 else
 cout << "1";
 cout << endl;
 }
 if (dupBit[vv] == 1)
 {
 new_cand[new_nd++] = vv;
 dupBit[vv] = 0;
 }
 }
 }

 for (int i = 0; i < nc; ++i)
 {
 candBit[cand[i]] = 0;
 }

 for (int i = nc; i < nd; ++i)
 {
 dupBit[cand[i]] = 0;
 }

 for (int i = 0; i < new_nc; ++i)
 {
 candBit[new_cand[i]] = 1;
 }

 for (int i = new_nc; i < new_nd; ++i)
 {
 dupBit[new_cand[i]] = 1;
 }

 level++;
 cout << "new left:";
 for (int i = 0; i < new_nl; ++i)
 cout << new_left[i] << " ";
 cout << endl;
 cout << "new par:";
 for (int i = 0; i < new_np; ++i)
 cout << par[i] << " ";
 cout << endl;
 cout << "new cand:";
 for (int i = 0; i < new_nc; ++i)
 cout << new_cand[i] << " ";
 cout << endl;
 cout << "new dup:";
 for (int i = nc; i < new_nd; ++i)
 cout << new_cand[i] << " ";
 cout << endl;
 cout << "leftBit: ";
 for (int i = 0; i < bg.getV1Num(); ++i)
 if (leftBit[i] == 0)
 cout << "0";
 else
 cout << "1";
 cout << endl;
 cout << "candBit: ";
 for (int i = 0; i < bg.getV2Num(); ++i)
 if (candBit[i] == 0)
 cout << "0";
 else
 cout << "1";
 cout << endl;
 cout << "dupBit: ";
 for (int i = 0; i < bg.getV2Num(); ++i)
 if (dupBit[i] == 0)
 cout << "0";
 else
 cout << "1";
 cout << endl << endl;

 biclique_find(bg, new_left, new_nl, par, new_np, new_cand, new_nc,
 new_nd, level);

 level--;
 for (int i = 0; i < nl; ++i)
 {
 leftBit[left[i]] = 1;
 }

 swap(cand[0], cand[nc - 1]);
 nc--;

 for (int i = 0; i < nc; ++i)
 {
 candBit[cand[i]] = 1;
 }

 if (nc == 0)
 {
 for (int i = nc; i < nd; ++i)
 {
 dupBit[cand[i]] = 0;
 }
 }
 else
 {
 for (int i = nc; i < nd; ++i)
 {
 dupBit[cand[i]] = 1;
 }
 }

 cout << "after level: " << level << endl;
 cout << "left, " << nl << ": ";
 for (int i = 0; i < nl; ++i)
 cout << left[i] << " ";
 cout << endl;
 cout << "par, " << np << ": ";
 for (int i = 0; i < np; ++i)
 cout << par[i] << " ";
 cout << endl;
 cout << "cand, " << nc << ": ";
 for (int i = 0; i < nc; ++i)
 cout << cand[i] << " ";
 cout << endl;
 cout << "dup, " << nd - nc << ": ";
 for (int i = nc; i < nd; ++i)
 cout << cand[i] << " ";
 cout << endl;
 cout << "leftBit: ";
 for (int i = 0; i < bg.getV1Num(); ++i)
 if (leftBit[i] == 0)
 cout << "0";
 else
 cout << "1";
 cout << endl;
 cout << "candBit: ";
 for (int i = 0; i < bg.getV2Num(); ++i)
 if (candBit[i] == 0)
 cout << "0";
 else
 cout << "1";
 cout << endl;
 cout << "dupBit: ";
 for (int i = 0; i < bg.getV2Num(); ++i)
 if (dupBit[i] == 0)
 cout << "0";
 else
 cout << "1";
 cout << endl << endl;
 }
 }*/

void BCE::biclique_find_pivot(BiGraph &bg, vid_t *clique, int nc, vid_t *left,
		int nl, vid_t *right, int ne, int ce)
{

}

//BiCliqueStat bsInit()
//{
//	BiCliqueStat bs;
//	return bs;
//}
//
//BiCliqueStat BCE::bs = bsInit();
