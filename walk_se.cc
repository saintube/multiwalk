#include <iostream>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

using namespace std;

#define LENGTH 20 
struct node{
    unsigned int edge_num = 0;
    vector<unsigned int> target; 
};

int main(int argc, char *argv[]){
    struct timeval startTime,endTime;
    float Timeuse;

    gettimeofday(&startTime,NULL);

    ifstream ifs;
    ifs.open(argv[1]);
    int n_v, n_e;
    ifs >> n_v >> n_e;
    node * vertex = new node[n_v];
    unsigned int from, to;
    while(!ifs.eof()){
        ifs >> from >> to;
        vertex[from].edge_num++;
        vertex[from].target.push_back(to);
    }
    ifs.close();

    ofstream ofs;
    ofs.open("out.dat");
    srand((unsigned)time(NULL));
    for(auto id = 0; id< n_v; id++){
        auto now = id;
        ofs << now << " ";
        for(auto i = 1; i < LENGTH; i++){
            if(vertex[now].edge_num > 0){
                int next = rand() % vertex[now].edge_num;
                now = vertex[now].target[next];
                ofs << now << " ";
            }
            else{
                ofs << "dead end";
                break;
            }

        }
        ofs << endl;
    }
    ofs.close();

    gettimeofday(&endTime,NULL);	
    Timeuse = 1000000*(endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec);
    Timeuse /= 1000000;	
    printf("Timeuse = %f\n",Timeuse);

    return 0;
}
