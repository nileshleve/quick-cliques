#include "OrderingTools.h"
#include "DegeneracyTools.h"
#include "GraphTools.h"
#include "CliqueColoringStrategy.h"

#include <vector>
#include <iostream>

using namespace std;

void OrderingTools::InitialOrderingMCQ(vector<vector<char>> const &adjacencyMatrix, vector<int> &vOrderedVertices, vector<int> &vColoring)
{
    size_t maxDegree(0);
    {
        vector<int> vDegree(adjacencyMatrix.size(), 0);
        for (size_t u = 0; u < adjacencyMatrix.size(); ++u) {
            for (size_t v = 0; v < adjacencyMatrix.size(); ++v) {
                if (adjacencyMatrix[u][v]) vDegree[u]++;
            }
            maxDegree = max(maxDegree, static_cast<size_t>(vDegree[u]));
        }
        vOrderedVertices = std::move(GraphTools::OrderVerticesByDegree(adjacencyMatrix, vDegree, false /* non-increasing order */));
    }

    vColoring.reserve(adjacencyMatrix.size());
    vColoring.clear();
    for (int degree = 1; degree <= maxDegree; degree++ ) {
        vColoring.push_back(degree);
    }

    vColoring.resize(adjacencyMatrix.size(), maxDegree + 1);
}

void OrderingTools::InitialOrderingMCR(vector<vector<char>> const &adjacencyMatrix, vector<int> &vOrderedVertices, vector<int> &vColoring, size_t &cliqueSize)
{
    vOrderedVertices.resize(adjacencyMatrix.size(), -1);
    vColoring.resize(adjacencyMatrix.size(), -1);

    // create an adjacencyArray, much faster for degeneracy ordering.
    vector<vector<int>> adjacencyArray(adjacencyMatrix.size());
    for (int vertex = 0; vertex < adjacencyMatrix.size(); ++vertex) {
        for (int otherVertex = 0; otherVertex < adjacencyMatrix.size(); ++otherVertex) {
            if (adjacencyMatrix[vertex][otherVertex]) {
                adjacencyArray[vertex].push_back(otherVertex);
            }
        }
    }

    size_t const size(adjacencyArray.size());

    // array of lists of vertices, indexed by degree
    vector<list<int>> verticesByDegree(size);

    // array of lists of vertices, indexed by degree
    vector<list<int>::iterator> vertexLocator(size);

    vector<int> degree(size);

    // fill each cell of degree lookup table
    // then use that degree to populate the 
    // lists of vertices indexed by degree

    size_t maxDegree(0);
    for(size_t i = 0; i < size; i++) {
        degree[i] = adjacencyArray[i].size();
        verticesByDegree[degree[i]].push_front(i);
        vertexLocator[i] = verticesByDegree[degree[i]].begin();

        maxDegree = max(maxDegree, adjacencyArray[i].size());
    }

    int currentDegree = 0;
    int numVerticesRemoved = 0;

    vector<NeighborListArray> vOrderingArray(size);

    while (numVerticesRemoved < size) {
        if (!verticesByDegree[currentDegree].empty()) {

            int vertex(-1);
            if (verticesByDegree[currentDegree].size() > 1) {

                // if remaining graph is regular.
                if ((size - numVerticesRemoved) == verticesByDegree[currentDegree].size()) {
////                    cout << "Remaining graph of " << verticesByDegree[currentDegree].size() << " vertices is regular with degree " << currentDegree << endl;

                    // if regular, and degree is # vertices - 1, then it's a clique.
                    if (static_cast<int>(verticesByDegree[currentDegree].size()) == currentDegree + 1) {
                        cliqueSize = currentDegree + 1;
                    }

                    vector<int> remainingVertices(verticesByDegree[currentDegree].begin(), verticesByDegree[currentDegree].end());
                    vector<int> remainingColors(remainingVertices.size(), 0);
                    CliqueColoringStrategy coloringStrategy(adjacencyMatrix);
                    coloringStrategy.Color(adjacencyMatrix, remainingVertices /* evaluation order */, remainingVertices /* color order */, remainingColors);
                    //copy orderingt to output arrays
                    for (size_t index = 0; index < remainingVertices.size(); ++index) {
                        vOrderedVertices[index] = remainingVertices[index];
                        vColoring[index] = remainingColors[index];
                    }

                    for (size_t index = remainingColors.size(); index < vColoring.size(); ++index) {
                        vColoring[index] = min(vColoring[index-1] + 1, static_cast<int>(maxDegree + 1));
                    }

                    return;
                } else {
                    // break ties by neighborhood-degree
                    size_t maxNeighborhoodDegree(0);
                    int    chosenVertex=verticesByDegree[currentDegree].front();
                    for (int const candidate : verticesByDegree[currentDegree]) {
                        size_t neighborhoodDegree(0);
                        for (int const neighbor : adjacencyArray[candidate]) {
                            if (degree[neighbor] != -1) {
                                neighborhoodDegree += degree[neighbor];
                            }
                        }

                        if (neighborhoodDegree > maxNeighborhoodDegree) {
                            maxNeighborhoodDegree = neighborhoodDegree;
                            chosenVertex = candidate;
                        }
                    }
                    vertex = chosenVertex;
                    verticesByDegree[currentDegree].erase(vertexLocator[vertex]);
                }
            } else {
                vertex = verticesByDegree[currentDegree].front();
                verticesByDegree[currentDegree].pop_front();
            }

            vOrderingArray[vertex].vertex = vertex;
            vOrderingArray[vertex].orderNumber = numVerticesRemoved;
            vOrderedVertices[vOrderedVertices.size() - numVerticesRemoved - 1] = vertex;

            degree[vertex] = -1;

            // will swap later neighbors to end of neighbor list
            vector<int> &neighborList = adjacencyArray[vertex];

            int splitPoint(neighborList.size());
            for(int i=0; i < splitPoint; ++i) {
                int const neighbor(neighborList[i]);
                // if later neighbor, swap to end of neighborList (there are few of these)
                if(degree[neighbor]!=-1) {
                    verticesByDegree[degree[neighbor]].erase(vertexLocator[neighbor]);

                    neighborList[i] = neighborList[--splitPoint];
                    neighborList[splitPoint] = neighbor;
                    i--;

                    degree[neighbor]--;

                    if (degree[neighbor] != -1)
                    {
                        verticesByDegree[degree[neighbor]].push_front(neighbor);
                        vertexLocator[neighbor] = verticesByDegree[degree[neighbor]].begin();
                    }
                }
                // earlier neighbor, do nothing.
            }

            // create space for later neighbors to ordering
            vOrderingArray[vertex].laterDegree = neighborList.size() - splitPoint;
            vOrderingArray[vertex].later.resize(neighborList.size() - splitPoint);

            // create space for earlier neighbors to ordering
            vOrderingArray[vertex].earlierDegree = splitPoint;
            vOrderingArray[vertex].earlier.resize(splitPoint);

            // fill in earlier and later neighbors
            for (int i = 0; i < splitPoint; ++i) {
////            cout << "earlier: " << vOrderingArray[vertex].earlier.size() << endl;
////            cout << "split  : " << splitPoint << endl;
////            cout << "earlier[" << i << "]" << endl;
                vOrderingArray[vertex].earlier[i] = neighborList[i];
            }

            for (int i = splitPoint; i < neighborList.size(); ++i) {
////            cout << "later  : " << vOrderingArray[vertex].later.size() << endl;
////            cout << "split  : " << splitPoint << endl;
////            cout << "later [" << i - splitPoint << "]" << endl;
                vOrderingArray[vertex].later[i-splitPoint] = neighborList[i];
            }

            numVerticesRemoved++;
            currentDegree = 0;
        }
        else {
            currentDegree++;
        }
    }
}