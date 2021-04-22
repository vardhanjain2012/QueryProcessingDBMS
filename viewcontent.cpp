// TORUN: ./viewcontent TestCases/TC_join1/myOutput

#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include<cstring>

// included by me
#include <climits>
#include <cstdint> 

using namespace std;

FileManager *fm;

int main(int argc, char** argv) {

    if(argc!=2){
		cout << "Incorrect arguements!!" << endl << "Expected run command: /viewcontent <input_filename>"<<endl;
		exit(0);
	}
	const char* inputfile  = argv[1];
	
	fm = new FileManager();

	FileHandler fhin = fm->OpenFile(inputfile);

    int startPageNum;
	PageHandler inputPage = fhin.FirstPage();
	startPageNum = inputPage.GetPageNum();
	fhin.UnpinPage(startPageNum);
	int endPageNum;
	inputPage = fhin.LastPage();
	endPageNum = inputPage.GetPageNum();
	fhin.UnpinPage(endPageNum);
    int num;
    for(int currPageNum = startPageNum; currPageNum<=endPageNum; currPageNum+=1){
		inputPage = fhin.PageAt(currPageNum);
		char* dataIn = inputPage.GetData();
        cout<<"currPageNum: "<<currPageNum<<endl;
        for(int i=0; i<PAGE_CONTENT_SIZE; i+=sizeof(int)){
            memcpy (&num, &dataIn[i], sizeof(int));
            cout<<num<<endl;
        }
        cout<<"----------"<<endl;
        fhin.UnpinPage(currPageNum);
    }
    fm->CloseFile(fhin);
    fm->ClearBuffer();
    
	delete fm;
	return 0;
}
