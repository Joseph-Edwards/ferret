#ifndef _GRAPH_HPP_FDAJIOQ
#define _GRAPH_HPP_FDAJIOQ

#include "vec1.hpp"
#include <set>

// Store an edge for an edge-coloured graph.
// We require users only give non-negative colours,
// we use negative colours to represent reversed direction edges
class ColEdge { 
  int tar;
  int col;
public:
  ColEdge(int _target, int _colour)
  : tar(_target),
    col(_colour)
  { D_ASSERT(tar > 0); }

  ColEdge(std::pair<int, int> p)
  : tar(p.first),
    col(p.second)
  { D_ASSERT(tar > 0); }

  // we want to allow colour = 0
  ColEdge flipped() const
  {
    ColEdge c(tar, 0);
    c.col = -1 - col;
    return c;
  }

  int target() const { return tar; }
  int colour() const { return col; }
  
  static std::string type()
  { return "coloured edge"; }

  friend bool operator==(const ColEdge& lhs, const ColEdge& rhs)
  { return lhs.tar == rhs.tar && lhs.col == rhs.col; }

  friend bool operator<(const ColEdge& lhs, const ColEdge& rhs)
  {
    if(lhs.tar < rhs.tar) return true;
    if(lhs.tar > rhs.tar) return false;
    if(lhs.col < rhs.col) return true;
    if(lhs.col > rhs.col) return false;
    return false;
  }
  
  friend std::ostream& operator<<(std::ostream& o, ColEdge c) {
      return o << "[" << c.target() << "," << c.colour() << "]";
  }
};


// Fake implementation of an coloured edge, for uncoloured graphs!
class UncolouredEdge {
  unsigned int tar:31;
  unsigned int orient:1;
public:
  UncolouredEdge(int _target, int _colour = 0)
  : tar(_target),
    orient(_colour)
  { 
    D_ASSERT(_target > 0); 
    D_ASSERT(_colour == 0 || _colour == 1);
  }

  // we want to allow colour = 0
  UncolouredEdge flipped() const
  {
    UncolouredEdge c(tar, 1 - orient);
    return c;
  }

  int target() const { return tar; }
  int colour() const { return orient; }
  
  static std::string type() {
    return "uncoloured edge" ;
  }

  friend bool operator==(const UncolouredEdge& lhs, const UncolouredEdge& rhs)
  { return lhs.tar == rhs.tar && lhs.orient == rhs.orient; }

  friend bool operator<(const UncolouredEdge& lhs, const UncolouredEdge& rhs)
  {
    if(lhs.tar < rhs.tar) return true;
    if(lhs.tar > rhs.tar) return false;
    if(lhs.orient < rhs.orient) return true;
    if(lhs.orient > rhs.orient) return false;
    return false;
  }
  
  friend std::ostream& operator<<(std::ostream& o, UncolouredEdge c) {
      return o << "[" << c.target() << "," << c.colour() << "]";
  }
};

// This file stores some simple generic graph related structures

enum GraphDirected
{ GraphDirected_no = 0, GraphDirected_yes = 1};

struct Graph
{
	int graph_size;
	std::vector<std::set<int> > parts;
	vec1<vec1<UncolouredEdge> > edges;

	Graph(int _size = 0) : graph_size(_size), edges(_size)
	{ }

	void make_symmetric()
	{
		for(int i = 1; i <= graph_size; ++i)
		{
			for(int j = 1; j <= edges[i].size(); ++j)
			{
				edges[edges[i][j].target()].push_back(UncolouredEdge(j));
			}
		}
	}

	void clean()
	{
		for(int i = 1; i <= graph_size; ++i)
		{
			std::set<UncolouredEdge> e(edges[i].begin(), edges[i].end());
			edges[i] = vec1<UncolouredEdge>(e.begin(), e.end());
		}
	}
};

// Takes a graph with multi-coloured (and possibly multiple occurrences)
// of edges, and replaces it with one where there is at most one edge between
// each pair of vertices.
inline vec1<vec1<ColEdge> > compressGraph(const vec1<vec1<ColEdge> >& graph)
{
  std::map<std::multiset<int>, int> seen_maps;
  
  vec1<vec1<ColEdge> > output_graph(graph.size());
  for(int i = 1; i <= graph.size(); i++) {
    std::map<int, std::multiset<int> > edges;
    for(int j = 1; j <= graph[i].size(); ++j) {
      edges[graph[i][j].target()].insert(graph[i][j].colour());
    }
    
    for(int j = 1; j <= graph[i].size(); ++j) {
      if(edges[i].size() > 0) {
        if(seen_maps.count(edges[i]) == 0) {
          int val = seen_maps.size() + 1;
          seen_maps[edges[i]] = val;
        }
        output_graph[i].push_back(ColEdge(j, seen_maps[edges[i]]));
      }
    }
  }
  return output_graph;
}

inline vec1<vec1<UncolouredEdge> > compressGraph(const vec1<vec1<UncolouredEdge> >& graph)
{
  return graph;
}


// store how to configure graph propagators
struct GraphConfig
{
    int start_path_length;
    int normal_path_length;
    GraphConfig(int spl = 1, int npl = 1)
    : start_path_length(spl), normal_path_length(npl)
    { }
    
    GraphConfig(const GraphConfig& gc)
    : start_path_length(gc.start_path_length), normal_path_length(gc.normal_path_length)
    { }
};

#endif
