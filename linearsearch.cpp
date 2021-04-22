//Sample file for students to get their code running

#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include<cstring>

//included by me
#include <fstream>
#include <string>
#include <sstream>
#include <climits>
#include <cstdint> 

using namespace std;

FileManager *fm;
int main(int argc, char** argv) {

	if(argc!=4){
		cout << "Incorrect arguements!!" << endl << "Expected run command: ./linearsearch <input_filename> <query_filename>.txt <output_filename>"<<endl;
		exit(0);
	}
	const char* inputfileName  = argv[1];
	const char* queryfileName = argv[2];
	const char* outfileName = argv[3];

	fm = new FileManager();
	FileHandler fhin = fm->OpenFile(inputfileName);
	FileHandler fhout = fm->CreateFile(outfileName);
	ifstream queryfile(queryfileName);
	string myText;
	int myInt;
	PageHandler outPage = fhout.NewPage();
	int tmp = INT_MIN;
	char* dataOut_tmp = outPage.GetData();
	for(int k=0;k<PAGE_CONTENT_SIZE;k+=sizeof(int)){
		memcpy (&dataOut_tmp[k], &tmp, sizeof(int));
	}
	int outPageNumber = 0;
	int outIndex = 0;
	while (getline (queryfile, myText)) {
		stringstream ss(myText);
		ss>>myText;
		ss>>myInt;
		// cout<<"Search "<<myInt<<endl;

		PageHandler firstPage = fhin.FirstPage();
		int firstPageNumber = firstPage.GetPageNum();
		fhin.UnpinPage(firstPageNumber);
		PageHandler lastPage = fhin.LastPage();
		int lastPageNumber = lastPage.GetPageNum();
		fhin.UnpinPage(lastPageNumber);

		int currPageNumber = firstPageNumber;
		while(currPageNumber<=lastPageNumber){
			PageHandler currPage = fhin.PageAt(currPageNumber);
			char* data = currPage.GetData();
			int num;
			for(unsigned int i=0; i<PAGE_CONTENT_SIZE; i+=sizeof(int)){
				memcpy (&num, &data[i], sizeof(int));
				// cout<<"Page: "<<currPageNumber<<" index: "<<i<<" num: "<<num<<endl;
				if(num==myInt){
					// cout<<"Found! "<<currPageNumber<<" "<<i<<endl;
					pair<int, int> mySearch = make_pair(currPageNumber, i/(sizeof(int)));
					if(outIndex>=PAGE_CONTENT_SIZE&&fhout.FlushPage(outPageNumber)){
						fhout.UnpinPage(outPageNumber);
						outPage = fhout.NewPage();
						pair<int, int> tmpPair = make_pair(INT_MIN, INT_MIN);
						char* dataOut1 = outPage.GetData();
						for(int j=0;j<PAGE_CONTENT_SIZE;j+=sizeof(pair<int, int>)){
							memcpy (&dataOut1[j], &tmpPair, sizeof(pair<int, int>));
						}
						outPageNumber+=1;
						outIndex=0;
					}
					char* dataOut = outPage.GetData();
					memcpy (&dataOut[outIndex], &mySearch, sizeof(pair<int, int>));
					fhout.MarkDirty(outPageNumber);
					outIndex+=sizeof(pair<int, int>);
				}
			}

			fhin.UnpinPage(currPageNumber);
			currPageNumber+=1;
		}
		pair<int, int> mySearch = make_pair(-1, -1);
		if(outIndex>=PAGE_CONTENT_SIZE&&fhout.FlushPage(outPageNumber)){
			fhout.UnpinPage(outPageNumber);
			outPage = fhout.NewPage();
			pair<int, int> tmpPair = make_pair(INT_MIN, INT_MIN);
			char* dataOut1 = outPage.GetData();
			for(int j=0;j<PAGE_CONTENT_SIZE;j+=sizeof(pair<int, int>)){
				memcpy (&dataOut1[j], &tmpPair, sizeof(pair<int, int>));
			}
			outPageNumber+=1;
			outIndex=0;
		}
		char* dataOut = outPage.GetData();
		memcpy (&dataOut[outIndex], &mySearch, sizeof(pair<int, int>));
		fhout.MarkDirty(outPageNumber);
		outIndex+=sizeof(pair<int, int>);
		// break;
	}
	// fm->PrintBuffer();
	fm->CloseFile(fhin);
	fm->CloseFile(fhout);
	fm->ClearBuffer();
	delete fm;
	queryfile.close();
	return 0;
}
