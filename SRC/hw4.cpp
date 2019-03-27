#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<list>
#include<cmath>
#include<algorithm>

using namespace std;

void Initialize(char* inputfileName);
void output(char* filename);

int cacheSize, blockSize, associative, algorithm;
int offset, blocks, sets, index;
int inputcount=0;
vector<int> hit,miss;
vector<int> cahceV;
list<int> cacheL;
list<int>::iterator Litr;
vector<int>::iterator Vitr;

int main(int argc, char* argv[]){
    
    int infilePosition;
    if((string)argv[1]=="-input"){
        infilePosition = 2;
    }else if((string)argv[3]=="-input"){
        infilePosition = 4;
    }
    ifstream inFile(argv[infilePosition], ios::in);
    if(!inFile){
        cerr<<"Failed opening"<<endl;
        exit(1);
    }
    string input;
    getline(inFile, input);
    cacheSize = stoi(input);
    getline(inFile, input);
    blockSize = stoi(input);
    getline(inFile, input);
    associative = stoi(input);
    getline(inFile, input);
    algorithm = stoi(input);

    offset = log2(blockSize);
    blocks = cacheSize*1024/blockSize;
    sets = (associative == 0)? blocks: (associative == 1)? blocks/4 : 1;
    index = log2(sets);
    list<int> cacheL2D[sets];

    unsigned int inputnum,dataindex;
    int inputtag;
    while(getline(inFile, input)){
        ++inputcount;
        inputnum = stoull(input,nullptr,16);
        if(associative==2){
            cahceV.assign(blocks,0);
            inputtag = inputnum>>offset;

            Litr=find(cacheL.begin(),cacheL.end(),inputtag);
            if(Litr!=cacheL.end()){
                hit.push_back(inputcount);//hit
                if(algorithm==1){
                    cacheL.remove(inputtag);  //mv to end
                    cacheL.push_back(inputtag);
                }
            }else{
                miss.push_back(inputcount);//miss
                if(cacheL.size()==blocks)
                    cacheL.pop_front();
                cacheL.push_back(inputtag);
            }
        }else if(associative==0||associative==1){
            dataindex = (inputnum>>offset) % sets;
            inputtag = inputnum >> (index + offset);

            Litr=find(cacheL2D[dataindex].begin(),cacheL2D[dataindex].end(),inputtag);
            if(Litr!=cacheL2D[dataindex].end()){
                hit.push_back(inputcount);//hit
                if(algorithm==1){
                    cacheL2D[dataindex].remove(inputtag);  //mv to end
                    cacheL2D[dataindex].push_back(inputtag);
                }
            }else{
                miss.push_back(inputcount);//miss
                int blocks_in_a_set=(associative==0)?1:4;
                if(cacheL2D[dataindex].size()==blocks_in_a_set){
                    cacheL2D[dataindex].pop_front();
                }
                cacheL2D[dataindex].push_back(inputtag);
            }
        }
    }
    if((string)argv[3]=="-output"){
        output(argv[4]);
    }else if((string)argv[1]=="-output"){
        output(argv[2]);
    }
    return 0;
}

void output(char* filename){
    ofstream outFile((string)filename,ios::out);
    if(!outFile){
        cerr<<"Failed open output file"<<endl;
        exit(1);
    }
    for (Vitr = hit.begin(); Vitr != hit.end(); ++Vitr){
        if(Vitr == hit.begin())
            outFile<<"Hits instructions: "<<*Vitr;
        else
            outFile<<","<<*Vitr;
    }
    for (Vitr = miss.begin(); Vitr != miss.end(); ++Vitr){
        if(Vitr == miss.begin())
            outFile<<endl<<"Misses instructions: "<<*Vitr;
        else
            outFile<<","<<*Vitr;
    }
    outFile<<endl<<"Miss rate: "<< (float)miss.size()/(hit.size()+miss.size()) <<endl;
}