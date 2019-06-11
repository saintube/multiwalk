/**
 * 3 201828015059027 Yange Fang
 * @file DirectedTriangleCount.cc
 * @author  Yange Fang
 * @version 0.1
 *
 * @section LICENSE 
 * 
 * Copyright 2016 Shimin Chen (chensm@ict.ac.cn) and
 * Songjie Niu (niusongjie@ict.ac.cn)
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * @section DESCRIPTION
 * 
 * This file implements the DirectedTriangleCount algorithm using graphlite API.
 *
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <cstdlib>
#include <ctime>

#include "GraphLite.h"

#include <string>

#define VERTEX_CLASS_NAME(name) DirectedTriangleCount##name

using namespace std;

int parallelism;
const int64_t ONE = (int64_t)1;
const int64_t LENGTH = 20;
struct Results {
    int64_t arr[LENGTH];
} results;

class VERTEX_CLASS_NAME(InputFormatter): public InputFormatter {
public:
    int64_t getVertexNum() {
        unsigned long long n;
        sscanf(m_ptotal_vertex_line, "%lld", &n);
        m_total_vertex= n;
        return m_total_vertex;
    }
    int64_t getEdgeNum() {
        unsigned long long n;
        sscanf(m_ptotal_edge_line, "%lld", &n);
        m_total_edge= n;
        return m_total_edge;
    }
    int getVertexValueSize() {
        m_n_value_size = sizeof(Results);
        return m_n_value_size;
    }
    int getEdgeValueSize() {
        m_e_value_size = sizeof(double);
        return m_e_value_size;
    }
    int getMessageValueSize() {
        m_m_value_size = sizeof(Results);
        return m_m_value_size;
    }
    void loadGraph() {
        unsigned long long last_vertex;
        unsigned long long from;
        unsigned long long to;
        double weight = 0;
        
        int64_t value = {0};
        int outdegree = 0;
        
        const char *line= getEdgeLine();

        // Note: modify this if an edge weight is to be read
        //       modify the 'weight' variable

        sscanf(line, "%lld %lld", &from, &to);
        addEdge(from, to, &weight);

        last_vertex = from;
        ++outdegree;
        for (int64_t i = 1; i < m_total_edge; ++i) {
            line= getEdgeLine();

            // Note: modify this if an edge weight is to be read
            //       modify the 'weight' variable

            sscanf(line, "%lld %lld", &from, &to);
            if (last_vertex != from) {
                addVertex(last_vertex, &value, outdegree);
                last_vertex = from;
                outdegree = 1;
            } else {
                ++outdegree;
            }
            addEdge(from, to, &weight);
        }
        addVertex(last_vertex, &value, outdegree);
    }
};

class VERTEX_CLASS_NAME(OutputFormatter): public OutputFormatter {
public:
    void writeResult() {
        char s[1024];
        int n;
        int64_t vid;
        Results value;
        // all vertex are set to same value, as global aggregator.
        for (ResultIterator r_iter; ! r_iter.done(); r_iter.next() )
        {
            r_iter.getIdValue(vid, &value);
            n = 0;
            for (int i = 0; i < LENGTH; i++)
            {
                if(value.arr[i] == (int64_t)-1){
                    n += sprintf(s + n, "Dead end.");
                    break;
                }
                else{
                    n += sprintf(s + n, "%lld ", (unsigned long long)value.arr[i]);
                }
            }
            n += sprintf(s + n, "\n");
            writeNextResLine(s, n);
        }
        
    }
};

class VERTEX_CLASS_NAME(): public Vertex <Results, double, Results> {
public:
    void compute(MessageIterator* pmsgs) {
        if (getSuperstep() == 0){
            //
            //printf("Node %lld initializing...\n", getVertexId());
            //printf("....out degree: %d\n", m_pme->m_out_degree);

            int64_t mid = getVertexId();
            Results msg;
            if (m_pme->m_out_degree == 0){
                msg.arr[0] = mid;
                for (int i = 1; i < LENGTH; i++){
                    msg.arr[i] = -1;
                }
                *mutableValue() = msg;
            }
            else{
                // int64_t num = m_out_degree;
                srand((unsigned)time(NULL));
                int64_t index = rand() % m_pme->m_out_degree;
                OutEdgeIterator itr = getOutEdgeIterator();
                for (int64_t i = 0; i < index; i++){
                    itr.next();
                }
                msg.arr[0] = mid;
                sendMessageTo(itr.target(), msg);
            }
        } else if (getSuperstep() < LENGTH-1){
            int64_t mid = getVertexId();
            int64_t index;
            // OutEdgeIterator itr;
            Results tmp;
            //cout << "....superstep: " << getSuperstep() << " node: " << getVertexId() << endl;
            for (; !pmsgs->done(); pmsgs->next()){
                // Dead ends should resend msg to itself, to keep msg alive
                // first time getting msg, end nodes set the i_th as mid, then set space left as -1
                // otherwise, just resend it
                tmp = pmsgs->getValue();
                
                if (m_pme->m_out_degree == 0){
                    if(mid != *((int64_t *)(pmsgs->getCurrent()))){
                        tmp.arr[getSuperstep()] = mid;
                        for (int64_t i = getSuperstep()+1; i < LENGTH; i++){
                            tmp.arr[i] = -1;
                        }
                    }
                    sendMessageTo(mid, tmp);
            //        cout << "....[msg]: ";
                //    for (int64_t v : tmp.arr){
                        //cout << v << " ";
                //        printf("%lld ", v);
               //     }
              //      cout << endl;
                }
                else{
                    //srand((unsigned)time(NULL));
                    index = rand() % m_pme->m_out_degree;
                    OutEdgeIterator itr = getOutEdgeIterator();
                    for (int64_t i = 0; i < index; i++){
                        itr.next();
                    }
                    tmp = pmsgs->getValue();
                    tmp.arr[getSuperstep()] = mid;

                    sendMessageTo(itr.target(), tmp);
                }
            }
        }else if (getSuperstep() == LENGTH-1){
            //
            //printf("Handle node %lld, sending back...\n", getVertexId());

            //send back to the first node
            int64_t mid = getVertexId();
            // int64_t first;
            Results tmp;
            for (; !pmsgs->done(); pmsgs->next()){
                tmp = pmsgs->getValue();
                // first = tmp[0];
                if (m_pme->m_out_degree > 0){
                    tmp.arr[getSuperstep()] = mid;
                }
                //
              //  printf("....send back message from node %lld to node %lld\n", mid, tmp.arr[0]);
               // cout << "....[msg]: ";
               // for (int64_t v : tmp.arr){
                //    cout << v << " ";
                //}
                //cout << endl;

                sendMessageTo(tmp.arr[0], tmp);

            }
        }else{
            // write vec into node value
            if (m_pme->m_out_degree > 0){
                *mutableValue() = pmsgs->getValue();
            }
            //
            //printf("Superstep 10 latter, node %lld should halt at this moment.\n", getVertexId());

            voteToHalt(); return;
        }
    }
};

class VERTEX_CLASS_NAME(Graph): public Graph {
public:
    // argv[0]: PageRankVertex.so
    // argv[1]: <input path>
    // argv[2]: <output path>
    void init(int argc, char* argv[]) {

        if (argc < 3) {
           printf ("Usage: %s <input path> <output path>\n", argv[0]);
           exit(1);
        }
        else ;

        //parallelism = atoi(argv[3]);
        string s = argv[1];
        int sl = s.length();
        if (sl >= 4 && s[sl - 1] == 'w'){
            int i;
            for (i = sl - 1; i >= 0; i--){
                if (s[i] == '_') break;
            }
            string num = s.substr(i + 1, sl - 2 - i);
            parallelism = atoi(num.c_str());
        }
        else {
            printf("Error: Invalid partition number!\n");
            exit(1);
        }

        if (parallelism <= 0 || parallelism > 16){
            printf ("Error: Invalid parallelism. Please give a parallelism between 1 to 16!\n");
            exit(1);
        }

        //
        printf("[Parallelsim]: %d\n", parallelism);
        setNumHosts(parallelism + 1);
        setHost(0, "localhost", 1412);

        int port_table[] = {1413, 1414, 1415, 1422, 1423, 1424, 1425, 1432, 1433, 1434, 1442, 1443, 1444, 1452, 1453, 1454};

        for (int i = 1; i <= parallelism; i++){
            //int p = 1411 + 10 * i;
            int p = port_table[i - 1];
            setHost(i, "localhost", p);
        }
        /*
        setHost(0, "localhost", 1411);
        setHost(1, "localhost", 1421);
        setHost(2, "localhost", 1431);
        setHost(3, "localhost", 1441);
        setHost(4, "localhost", 1451);
        */

        m_pin_path = argv[1];
        m_pout_path = argv[2];

    }

    void term() {
    }
};

/* STOP: do not change the code below. */
extern "C" Graph* create_graph() {
    Graph* pgraph = new VERTEX_CLASS_NAME(Graph);

    pgraph->m_pin_formatter = new VERTEX_CLASS_NAME(InputFormatter);
    pgraph->m_pout_formatter = new VERTEX_CLASS_NAME(OutputFormatter);
    pgraph->m_pver_base = new VERTEX_CLASS_NAME();

    return pgraph;
}

extern "C" void destroy_graph(Graph* pobject) {
    delete ( VERTEX_CLASS_NAME()* )(pobject->m_pver_base);
    delete ( VERTEX_CLASS_NAME(OutputFormatter)* )(pobject->m_pout_formatter);
    delete ( VERTEX_CLASS_NAME(InputFormatter)* )(pobject->m_pin_formatter);
    delete ( VERTEX_CLASS_NAME(Graph)* )pobject;
}
