#ifndef _DJS_MISC_H_
#define _DJS_MISC_H_

/* 
    This program is free software: you can redistribute it and/or modify 
    it under the terms of the GNU General Public License as published by 
    the Free Software Foundation, either version 3 of the License, or 
    (at your option) any later version. 
 
    This program is distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
    GNU General Public License for more details. 
 
    You should have received a copy of the GNU General Public License 
    along with this program.  If not, see <http://www.gnu.org/licenses/> 
*/

#include <list>
#include <vector>
#include <stdio.h>

class MaximalCliquesAlgorithm;

/*! \file Tools.h

    \brief see misc.c

    \author Darren Strash (first name DOT last name AT gmail DOT com)

    \copyright Copyright (c) 2011 Darren Strash. This code is released under the GNU Public License (GPL) 3.0.

    \image html gplv3-127x51.png

    \htmlonly
    <center>
    <a href="gpl-3.0-standalone.html">See GPL 3.0 here</a>
    </center>
    \endhtmlonly
*/

#include <cmath>
//#define max(x,y) (x > y? x:y)
//#define min(x,y) (x < y? x:y)

int nodeComparator(int node1, int node2);

void printArray(int* array, int size);

void printArrayOfLinkedLists(std::vector<std::list<int>> const &listOfLists, int size);

void printInt(int integer);

void destroyCliqueResults(std::list<std::list<int>> &cliques);

std::vector<std::list<int>> readInGraphAdjList(int* n, int* m);

void runAndPrintStatsMatrix(long (*function)(char**,
                                             #ifdef RETURN_CLIQUES_ONE_BY_ONE
                                             std::list<std::list<int>> &,
                                             #endif
                                             int),
                            const char* algName,
                            char** adjMatrix,
                            #ifdef RETURN_CLIQUES_ONE_BY_ONE
                            std::list<std::list<int>> &cliques,
                            #endif
                            int n );

void RunAndPrintStats(MaximalCliquesAlgorithm* pAlgorithm, std::list<std::list<int>> &cliques);

void runAndPrintStatsListList( long (*function)(std::vector<std::list<int>> const &, 
                                                int**, 
                                                #ifdef RETURN_CLIQUES_ONE_BY_ONE
                                                std::list<std::list> &,
                                                #endif
                                                int*, int),
                               const char* algName,
                               std::vector<std::list<int>> const &adjListLinked,
                               int** adjListArray,
                               #ifdef RETURN_CLIQUES_ONE_BY_ONE
                               std::list<std::list<int>> &cliques,
                               #endif
                               int* degree,
                               int n );


void printList(std::list<int> const &linkedList, void (*printFunc)(int));

void printListAbbv(std::list<int> const &linkedList, void (*printFunc)(int));

/*! \brief process a clique, which may include printing it in
           one of several formats and/or adding the 
           clique to a linked list.

    \param cliques A linked list of cliques to return. <b>(only available when compiled 
                   with RETURN_CLIQUES_ONE_BY_ONE defined)</b>

    \param clique the clique to add to the linked list

*/

inline void processClique(
                          #ifdef RETURN_CLIQUES_ONE_BY_ONE
                          std::list<std::list<int>> &cliques,
                          #endif
                          std::list<int> const &clique)
{
    #ifdef PRINT_CLIQUES_TOMITA_STYLE
    printf("c ");
    #endif

    #ifdef PRINT_CLIQUES_ONE_BY_ONE
    printList(clique, &printInt);
    #endif

    #ifdef RETURN_CLIQUES_ONE_BY_ONE
    cliques.push_back(clique);
    #endif
}
#endif
