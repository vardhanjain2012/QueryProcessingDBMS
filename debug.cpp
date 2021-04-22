//Sample file for students to get their code running

#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include "constants.h"
#include<cstring>

//included by me
#include <fstream>
#include <string>
#include <sstream>
#define pi pair<int,int>
using namespace std;

void printPage(FileHandler fhin, int pg){
	char *smalldata = fhin.PageAt(pg).GetData();
    pi num;
	for(int i=0;i<PAGE_CONTENT_SIZE;i+= sizeof(pi)){
		memcpy (&num,&smalldata[i],  sizeof(pi));
        cout<<"("<<num.first<<","<<num.second<<")"<<" | ";
	}
	cout<<endl;	
}

FileManager *fm;
int main(int argc, char** argv) {
    string s = "TestCases/TC_search/myOutputBinary";
	const char* outfileName  = s.c_str();

	fm = new FileManager();
	FileHandler fhin = fm->OpenFile(outfileName);


    for(int i=fhin.FirstPage().GetPageNum();i<fhin.LastPage().GetPageNum();i++){
        printPage(fhin, i);
		fm->ClearBuffer();
    }

	fm->CloseFile(fhin);
	fm->ClearBuffer();
	delete fm;
	return 0;
}
