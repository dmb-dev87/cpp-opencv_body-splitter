#include "LineToLoopSolver.h"
#include "Vertex.h"
#include "Line.h"

#include <spdlog/spdlog.h>
#include <lemon/list_graph.h>
#include <lemon/dijkstra.h>

#include <unordered_map>
using namespace lemon;

typedef lemon::ListGraph Graph;
typedef Graph::EdgeIt EdgeIt;
typedef Graph::Edge Edge;
typedef Graph::NodeIt NodeIt;
typedef Graph::Node Node;
typedef Graph::EdgeMap<int> LengthMap;

using lemon::INVALID;

class vertex_pair {
public:
	Vertex m_Vert;
	Graph::Node m_Node;
	vertex_pair() {

	}
	vertex_pair(const Vertex &vert, const Graph::Node &node) : m_Vert(vert), m_Node(node)
	{

	}
	size_t hash() const
	{
		const double HASH_TOLERANCE = 0.0005;
		std::size_t hash = 0;
		boost::hash_combine(hash, m_Vert.x - fmod(m_Vert.x, HASH_TOLERANCE));
		boost::hash_combine(hash, m_Vert.y - fmod(m_Vert.y, HASH_TOLERANCE));
		return hash;
	}
	bool operator==(const vertex_pair &in)
	{
		return m_Vert.MatchesLocation(in.m_Vert);
	}
};

namespace std {
	template <> struct hash<vertex_pair>
	{
		size_t operator()(const vertex_pair & x) const
		{
			const double HASH_TOLERANCE = 0.0005;
			std::size_t hash = 0;
			boost::hash_combine(hash, x.m_Vert.x - fmod(x.m_Vert.x, HASH_TOLERANCE));
			boost::hash_combine(hash, x.m_Vert.y - fmod(x.m_Vert.y, HASH_TOLERANCE));
			return hash;
		}
	};
}

bool getPair(const Vertex &in, std::unordered_map<std::size_t, vertex_pair> lists,
	vertex_pair &out)
{
	vertex_pair temp(in, Graph::Node());
	if (lists.count(in.hash()) > 0)
	{
		out = lists.at(in.hash());
		return true;
	}
	//Not in the list
	return false;
}

bool LineToLoopSolver::findLoop(std::vector< Line >& output, const std::vector< Line >& insets, const Vertex& pt1, const Vertex& pt2)
{
	//Create a graph of the inset lines, use Dijkstra's algo to determine shortest route from perim endpt to endpt
	Graph g;
	LengthMap length(g);
	std::unordered_map<std::size_t, vertex_pair> vertexPair;
	std::size_t pointID = 0;
	for (auto it : insets)
	{
		vertex_pair outputPair;
		if (getPair(it.pt1, vertexPair, outputPair) == false)
		{
			vertex_pair tempPair(Vertex(it.pt1), g.addNode());
			vertexPair[tempPair.hash()] = tempPair;
		}
		if (getPair(it.pt2, vertexPair, outputPair) == false)
		{
			vertex_pair tempPair(Vertex(it.pt1), g.addNode());
			vertexPair[tempPair.hash()] = tempPair;
		}
	}
	for (auto it : insets)
	{
		vertex_pair outputPair;
		if (getPair(it.pt1, vertexPair, outputPair) == false)
		{
			spdlog::get("DEBUG_FILE")->error("Couldnt find matching pair!");
			return false;
		}
		Graph::Node index1 = outputPair.m_Node;// vertexHashes[it.pt1];
		if (getPair(it.pt2, vertexPair, outputPair) == false)
		{
			spdlog::get("DEBUG_FILE")->error("Couldnt find matching pair!");
			return false;
		}
		Graph::Node index2 = outputPair.m_Node;
		auto newArc = g.addEdge(index1, index2);
		length.set(newArc, static_cast<int>(it.sqrdLength()*100000.0));



		//g.m_edges.back().m_property.weight = it.sqrdLength();
	}
	vertex_pair outPair;
	if (getPair(pt1, vertexPair, outPair))
	{
		Dijkstra<Graph, LengthMap> dijkstra(g, length);
		//bool reached = dijkstra(g, length).path(p).dist(d).run(s, t);
		dijkstra.run(outPair.m_Node);
		vertex_pair endPair;
		if (getPair(pt2, vertexPair, endPair))
		{

			Graph::Node endNode = endPair.m_Node;
			if (dijkstra.reached(endNode))
			{
				while (endNode != outPair.m_Node)
				{
					Vertex pt1;
					for (const auto &it : vertexPair)
					{
						if (it.second.m_Node == endNode)
						{
							pt1 = it.second.m_Vert;
							break;
						}
					}
					try {
						Graph::Node nextPoint = dijkstra.predNode(endNode);
						Vertex pt2;
						for (const auto &it : vertexPair)
						{
							if (it.second.m_Node == nextPoint)
							{
								pt2 = it.second.m_Vert;
								break;
							}
						}
						output.push_back(Line(pt1, pt2,0));
						endNode = nextPoint;
					}
					catch (...)
					{
						spdlog::get("DEBUG_FILE")->error("Dijkstra path said it reached goal, failed to find path");
						//Broken loop
						output.clear();
						return false;
					}

				}
			}
			else
				spdlog::get("DEBUG_FILE")->error("Couldnt match points on inset line: SS error");
			//std::cout << "Loop found\n";
		}
		else
			spdlog::get("DEBUG_FILE")->error("Perimeter vertex doesnt match inset vertices PT2");
	}
	else
		spdlog::get("DEBUG_FILE")->error("Perimeter vertex doesnt match inset vertices PT1");
	return true;
}

LineToLoopSolver::LineToLoopSolver()
{
}


LineToLoopSolver::~LineToLoopSolver()
{
}
