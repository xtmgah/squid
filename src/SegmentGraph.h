#ifndef __SEGMENTGRAPH_H__
#define __SEGMENTGRAPH_H__

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <limits>
#include <ctime>
#include <cmath>
#include <map>
#include <set>
#include <boost/algorithm/string.hpp>
#include <boost/icl/interval_set.hpp>
#include <boost/icl/interval_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/one_bit_color_map.hpp>
#include <boost/graph/stoer_wagner_min_cut.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/typeof/typeof.hpp>
#include "gurobi_c++.h"
#include "BPEdge.h"
#include "BPNode.h"
#include "ReadRec.h"
#include "SingleBamRec.h"

using namespace std;

extern int concorddis, concorddisl, concordidx, concordidxl;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_weight_t, int> > undirected_graph;
typedef boost::property_map<undirected_graph, boost::edge_weight_t>::type weight_map_type;
typedef boost::property_traits<weight_map_type>::value_type weight_type;

static std::map<char,char> Nucleotide={{'A','T'},{'C','G'},{'G','C'},{'T','A'},{'R','Y'},{'Y','R'},{'S','W'},{'W','S'},{'K','M'},{'M','K'},{'B','V'},{'V','B'},{'D','H'},{'H','D'}, {'N','N'}, {'.','.'},{'-','-'}};

void ReverseComplement(string::iterator itbegin, string::iterator itend);
pair<int,int> ExtremeValue(vector<int>::iterator itbegin, vector<int>::iterator itend);

struct MinCutEdge_t
{
  unsigned long first;
  unsigned long second;
};

class SegmentGraph_t{
public:
    vector<Node_t> vNodes;
    vector<Edge_t> vEdges;
    vector<int> Label;
public:
    SegmentGraph_t(){};
    SegmentGraph_t(const vector<int>& RefLength,SBamrecord_t& SBamrecord, vector< vector<int> >& Read_Node, int weightcutoff=5);
    SegmentGraph_t(string graphfile, int weightcutoff=5);
    void BuildNode(const vector<int>& RefLength, SBamrecord_t& SBamrecord);
    void BuildEdges(SBamrecord_t& SBamrecord, vector< vector<int> >& Read_Node);
    void FilterbyWeight(int weightcutoff);
    void FilterbyInterleaving();

    vector<int> LocateRead(int initialguess, ReadRec_t& ReadRec);
    vector<int> LocateRead(vector<int>& singleRead_Node, ReadRec_t& ReadRec);

    void RawEdges(SBamrecord_t& SBamrecord, vector< vector<int> >& Read_Node);
    void ChimericEdges(SBamrecord_t& SBamrecord, vector< vector<Edge_t> >& DiscordantEdges, vector< vector<int> >& Read_Node, vector<int>& BamIndex);
    
    void FilterEdges(int weightcutoff);
    void UpdateNodeLink();
    void CompressNode();
    void CompressNode(vector< vector<int> >& Read_Node);
    void OutputDegree(string outputfile);

    int DFS(int node, int curlabelid, vector<int>& Label);
    void ConnectedComponent(int & maxcomponentsize);
    void ConnectedComponent();
    void OutputConnectedComponent(string outputfile);

    vector< vector<int> > Ordering();
    vector<int> MincutRecursion(std::map<int,int> CompNodes, vector<Edge_t> CompEdges);
    void GenerateILP(std::map<int,int>& CompNodes, vector<Edge_t>& CompEdges, GRBEnv& env, GRBModel& model, vector<GRBVar>& vGRBVar);

    void SimplifyComponents(vector< vector<int> >& Components, map<int,int>& NewIndex, vector<Node_t>& NewNodeChr, vector< vector<int> >& LowSupportNode, vector<int>& ReferenceNode, vector<bool>& RelativePosition, int weightcutoff=3);
    void DesimplifyComponents(vector< vector<int> >& Components, map<int,int>& NewIndex, vector< vector<int> >& LowSupportNode, vector<int>& ReferenceNode, vector<bool>& RelativePosition);
    vector< vector<int> > ReadComponents(string file);
    vector< vector<int> > SortComponents(vector< vector<int> >& Components);
    vector< vector<int> > MergeSingleton(vector< vector<int> >& Components, const vector<int>& RefLength, vector<Node_t>& NewNodeChr, int LenCutOff=500000);
    /*bool MergeSingleton_Insert(int singleton, vector< vector<int> >& NewComponents, int LenCutOff); //return whether inserted or not
    bool MergeSingleton_Insert(vector<int> consecutive, vector< vector<int> >& NewComponents, int LenCutOff);*/
    bool MergeSingleton_Insert(vector<int> SingletonComponent, vector< vector<int> >& NewComponents, vector<Node_t>& NewNodeChr);
    bool MergeSingleton_Insert(vector< vector<int> > Consecutive, vector< vector<int> >& NewComponents, vector<Node_t>& NewNodeChr);
    vector< vector<int> > MergeComponents(vector< vector<int> >& Components, int cutoff=5);

    void OutputNewGenome(vector< vector<int> >& Components, const vector<string>& RefSequence, const vector<string>& RefName, string outputfile);
};

#endif
