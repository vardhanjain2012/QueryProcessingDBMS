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

//This finction should return the first occurence as pair(pageNumber, index) of myInt in the sorted input file
pair<int, int> findFirst(int myInt, FileHandler& fhin){
	pair<int, int> ans = make_pair(-1, -1);
	return ans;
}

//This finction should return the last occurence as pair(pageNumber, index) of myInt in the sorted input file
pair<int, int> findLast(int myInt, FileHandler& fhin){
	pair<int, int> ans = make_pair(-1, -1);
	return ans;
}

pair<int, int> nextPosition(pair<int, int> q){
	if(q.second==(PAGE_CONTENT_SIZE)){
		q.second=0;
		q.first+=1;
	}
	else{
		q.second+=sizeof(int);
	}
	return q;
}
bool copyRecord(pair<int, int> firstOccurence, pair<int, int> lastOccurence, FileHandler& fhin){
	PageHandler p1 = fhin.PageAt(firstOccurence.first);
	PageHandler p2 = fhin.PageAt(lastOccurence.first);
	char* data1 = p1.GetData();
	char* data2 = p2.GetData();
	int num;
	memcpy(&num, &data2[lastOccurence.second], sizeof(int));
	memcpy(&data1[firstOccurence.second], &data2[lastOccurence.second], sizeof(int));
	if(!fhin.MarkDirty(p1.GetPageNum())){
		cout<<"Error2"<<endl;
	}
	fhin.UnpinPage(p1.GetPageNum());
	fhin.UnpinPage(p2.GetPageNum());
	return (num==INT_MIN);
}
int main(int argc, char** argv) {

	if(argc!=3){
		cout << "Incorrect arguements!!" << endl << "Expected run command: ./deletion <sorted_input_filename> <query_filename>.txt"<<endl;
		exit(0);
	}

	const char* inputfileName  = argv[1];
	const char* queryfileName = argv[2];
	fm = new FileManager();
	FileHandler fhin = fm->OpenFile(inputfileName);
	ifstream queryfile(queryfileName);
	string myText;
	int myInt;
	while (getline (queryfile, myText)) {
		stringstream ss(myText);
		ss>>myText;
		ss>>myInt;

		pair<int, int> firstOccurence = findFirst(myInt, fhin);
		pair<int, int> lastOccurence = findLast(myInt, fhin);

		PageHandler lastPage = fhin.LastPage();
		int lastPageNumber = lastPage.GetPageNum();
		fhin.UnpinPage(lastPageNumber);

		if(firstOccurence.first<0||firstOccurence.first>lastPageNumber||lastOccurence.first<0||lastOccurence.first>lastPageNumber){
			continue;
		}
		lastOccurence = nextPosition(lastOccurence);
		while(lastOccurence.first<=lastPageNumber){
			if(copyRecord(firstOccurence, lastOccurence, fhin)){
				break;
			}
			if(firstOccurence.second==PAGE_CONTENT_SIZE){
				fhin.FlushPage(firstOccurence.first);
			}
			firstOccurence = nextPosition(firstOccurence);
			lastOccurence = nextPosition(lastOccurence);
		}

		for(int i=lastOccurence.first+1; i<=lastPageNumber; i++){
			if(!fhin.DisposePage(i)){ 
				cout<<"Error"<<endl;
			}
		}
		fhin.FlushPages();
	}
	fm->CloseFile(fhin);
	fm->ClearBuffer();
	delete fm;
	queryfile.close();
}
