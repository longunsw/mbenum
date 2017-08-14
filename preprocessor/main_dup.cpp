/*
 * test.cpp
 *
 *  Created on: 29Jun.,2017
 *      Author: longyuan
 */

#include <algorithm>
#include <stdio.h>
#include <string>
#include <algorithm>
#include <ext/hash_map>
#include <ext/hash_set>
#include <vector>
#include <climits>
#define hash_map __gnu_cxx::hash_map
#define hash_set __gnu_cxx::hash_set

using namespace std;

#define MAX_NODE_ID 30000000

int main(int argc, char **argv)
{

	if (argc != 3)
	{
		fprintf(stderr,
				"program input factor\n\
                                 input file contains all the edges and stored as nodeid nodeid\n\
                                 output will add suffix .m and .e automatically\n\
                                 factor is used for hashing.\n");
		exit(1);
	}

	int line = 0;

	FILE *inFile = NULL;

	inFile = fopen(argv[1], "r");
	if (inFile == NULL)
	{
		fprintf(stderr, "cannot open the input file\n");
		exit(1);
	}

	string edgef = string(argv[1]);
	size_t pos = edgef.find_last_of(".");
	string edge_file = edgef.substr(0, pos);
	edge_file += ".e";
	FILE *edge = fopen(edge_file.c_str(), "w");
	string meta_file = edgef.substr(0, pos);
	meta_file += ".meta";
	FILE *meta = fopen(meta_file.c_str(), "w");

	char *lines = NULL;
	size_t n;
	ssize_t rz;

	unsigned int lNode;
	unsigned int rNode;
	unsigned int ll;
	unsigned int rr;

	long edge_id, edge_count = 0;

	int factor = atoi(argv[2]);

	hash_set<long> edge_lookup(10000000*factor);
	unsigned int *lIndex = new unsigned int[MAX_NODE_ID];
	unsigned int *rIndex = new unsigned int[MAX_NODE_ID];

	vector<vector<int> > graph;
	graph.resize(MAX_NODE_ID);
	fill_n(lIndex, MAX_NODE_ID, UINT_MAX);
	fill_n(rIndex, MAX_NODE_ID, UINT_MAX);
	fprintf(stderr, "the first element in lindex: %u\n", lIndex[0]);
	fprintf(stderr, "the first element in rindex: %u\n", rIndex[0]);

	unsigned lOrder = 0;
	unsigned rOrder = 0;
	size_t edges = 0;

	while ((rz = getline(&lines, &n, inFile)) != -1)
	{
		if (lines[0] == '#' || lines[0] == '%')
			continue;

		sscanf(lines, "%d %d", &lNode, &rNode);
		if (edge_count == 0)
			fprintf(stderr, "line 1: %s %d, %d\n", lines, lNode, rNode);

//		if (headNode == tailNode)
//		{
//			continue;
//		}


		ll = lNode;
		rr = rNode;

		if (lIndex[lNode] == UINT_MAX)
		{
			lIndex[lNode] = lOrder++;

		}

		if (rIndex[rNode] == UINT_MAX)
		{
			rIndex[rNode] = rOrder++;
		}

		lNode = lIndex[lNode];
		rNode = rIndex[rNode];

		edge_count++;

		if (edge_count % 50000000 == 0)
		{
			fprintf(stderr, "precessed %ld  M edges\n", edge_count / 1000000);
		}

//		if (headNode > tailNode)
//		{
//			unsigned int tmp = headNode;
//			headNode = tailNode;
//			tailNode = tmp;
//		}

		edge_id = ((long) lNode << (sizeof(unsigned int) * 8)) | rNode;

		if (edge_lookup.find(edge_id) != edge_lookup.end())
		{
	//		fprintf(stderr, "%d %d\n", ll, rr);
			continue;
		}

		edge_lookup.insert(edge_id);
		edges++;
		graph[lNode].push_back(rNode);

	}

	if(rOrder < lOrder)
	{
		fprintf(stderr, "%d %d %zu\n", lOrder, rOrder, edges);
		fprintf(meta, "%d\n", lOrder);
		fprintf(meta, "%d\n", rOrder);
		fprintf(meta, "%zu\n", edges);
		for (int i = 0; i < lOrder; ++i)
		{
			for (int j = 0; j < graph[i].size(); ++j)
			{
				fprintf(edge, "%d %d\n", i, graph[i][j]);
			}
		}
	}
	else
	{
		fprintf(stderr, "%d %d %zu\n", rOrder, lOrder, edges);
		fprintf(meta, "%d\n", rOrder);
		fprintf(meta, "%d\n", lOrder);
		fprintf(meta, "%zu\n", edges);
		for (int i = 0; i < lOrder; ++i)
		{
			for (int j = 0; j < graph[i].size(); ++j)
			{
				fprintf(edge, "%d %d\n", graph[i][j], i);
			}
		}


	}

	fclose(edge);
	fclose(meta);
	fclose(inFile);

	return 0;

}

