//TORUN: ./deletion TestCases/TC_delete/sorted_input TestCases/TC_delete/myOutput

#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include<cstring>

#include <fstream>
#include <string>
#include <sstream>
#include <climits>
#include <cstdint> 


using namespace std;

FileManager *fm;

// void printFile(FileHandler& fhin){
// 	PageHandler firstPage = fhin.FirstPage();
// 	int firstPageNumber = firstPage.GetPageNum();
// 	fhin.UnpinPage(firstPageNumber);
// 	PageHandler lastPage = fhin.LastPage();
// 	int lastPageNumber = lastPage.GetPageNum();
// 	fhin.UnpinPage(lastPageNumber);
// 	int currPageNumber = firstPageNumber;
// 	while(currPageNumber<=lastPageNumber){
// 		PageHandler currPage = fhin.PageAt(currPageNumber);
// 		char* data = currPage.GetData();
// 		int num;
// 		for(unsigned int i=0; i<PAGE_CONTENT_SIZE; i+=sizeof(int)){
// 			memcpy (&num, &data[i], sizeof(int));
// 			cout<<"Page: "<<currPageNumber<<" index: "<<i<<" num: "<<num<<endl;
// 		}

// 		fhin.UnpinPage(currPageNumber);
// 		currPageNumber+=1;
// 	}
// 	return ;
// }

//This function should return the first occurence as pair(pageNumber, index) of myInt in the sorted input file
pair<int, int> findFirst(int myInt, FileHandler& fhin){
	PageHandler firstPage = fhin.FirstPage();
	int firstPageNumber = firstPage.GetPageNum();
	fhin.UnpinPage(firstPageNumber);
	PageHandler lastPage = fhin.LastPage();
	int lastPageNumber = lastPage.GetPageNum();
	fhin.UnpinPage(lastPageNumber);

	pair<int, int> mySearch = make_pair(-1, -1);
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
				mySearch = make_pair(currPageNumber, i);
				return mySearch;
			}
		}

		fhin.UnpinPage(currPageNumber);
		currPageNumber+=1;
	}
	return mySearch;
}

pair<int, int> nextPosition(pair<int, int> q){
	if(q.second>=(PAGE_CONTENT_SIZE - sizeof(int))){
		q.second=0;
		q.first+=1;
	}
	else{
		q.second+=sizeof(int);
	}
	return q;
}

//This finction should return the last occurence as pair(pageNumber, index) of myInt in the sorted input file
pair<int, int> findLast(int myInt, FileHandler& fhin, pair<int, int> firstOccurence){
	int firstPageNumber = firstOccurence.first;
	PageHandler lastPage = fhin.LastPage();
	int lastPageNumber = lastPage.GetPageNum();
	fhin.UnpinPage(lastPageNumber);

	pair<int, int> mySearch = make_pair(-1, -1);
	if(firstPageNumber==-1) return mySearch;
	mySearch = make_pair(firstOccurence.first, firstOccurence.second);
	pair<int, int> tmp;
	int num;
	while(mySearch.first<=lastPageNumber){
		tmp = nextPosition(mySearch);
		if(tmp.first>lastPageNumber) break;
		lastPage = fhin.PageAt(tmp.first);
		char* data = lastPage.GetData();
		// ASK: is there a chance repeated unpinning and pageat happening on the page can make it inefficient?
		fhin.UnpinPage(tmp.first);
		memcpy(&num, &data[tmp.second], sizeof(int));
		// cout<<"Page22: "<<tmp.first<<" index: "<<tmp.second<<" num: "<<num<<endl;
		if(num!=myInt) break;
		mySearch = tmp;
	}
	return mySearch;
}

bool copyRecord(pair<int, int> firstOccurence, pair<int, int> lastOccurence, FileHandler& fhin){
	PageHandler p1 = fhin.PageAt(firstOccurence.first);
	PageHandler p2 = fhin.PageAt(lastOccurence.first);
	char* data1 = p1.GetData();
	char* data2 = p2.GetData();
	int num;
	memcpy(&num, &data2[lastOccurence.second], sizeof(int));
	// ASK: What if the file doesn't end in INT_MIN?
	if(num==INT_MIN) return true;
	memcpy(&data1[firstOccurence.second], &data2[lastOccurence.second], sizeof(int));
	if(!fhin.MarkDirty(p1.GetPageNum())){
		//ASK: Why is printing this needed?
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
		pair<int, int> lastOccurence = findLast(myInt, fhin, firstOccurence);

		// printFile(fhin);
		// cout<<"Delete "<<myInt<<endl;
		// cout<<firstOccurence.first<<" "<<firstOccurence.second<<" "<<lastOccurence.first<<" "<<lastOccurence.second<<endl;

		PageHandler lastPage = fhin.LastPage();
		int lastPageNumber = lastPage.GetPageNum();
		fhin.UnpinPage(lastPageNumber);

		if(firstOccurence.first<0||firstOccurence.first>lastPageNumber||lastOccurence.first<0||lastOccurence.first>lastPageNumber){
			continue;
		}
		//ASK: What if the last occurance is last element of the last page?
		lastOccurence = nextPosition(lastOccurence);
		while(lastOccurence.first<=lastPageNumber){
			if(copyRecord(firstOccurence, lastOccurence, fhin)){
				break;
			}
			//ASK: Why is this necessary?
			if(firstOccurence.second==(PAGE_CONTENT_SIZE - sizeof(int))){
				fhin.FlushPage(firstOccurence.first);
			}
			firstOccurence = nextPosition(firstOccurence);
			lastOccurence = nextPosition(lastOccurence);
		}
		if(firstOccurence.second>0){
			int tmp = INT_MIN;
			PageHandler lp = fhin.PageAt(firstOccurence.first);
			char* data = lp.GetData(); 
			for(int i=firstOccurence.second; i<PAGE_CONTENT_SIZE; i+=sizeof(int)){
				memcpy(&data[i], &tmp, sizeof(int));
			}
			fhin.MarkDirty(firstOccurence.first);
			fhin.UnpinPage(firstOccurence.first);
		}
		int disposeFrom;
		if(firstOccurence.second==0) disposeFrom = firstOccurence.first;
		else disposeFrom = firstOccurence.first+1;
		for(int i=disposeFrom; i<=lastPageNumber; i++){
			//ASK: What does DisposePage() do? The documentation says it just marks dirty and unpins. Looks like there is now way to delete pages. Should we fill all remaining pages with INT_MIN?
			if(!fhin.DisposePage(i)){ 
				//ASK: Why is printing this error necessary?
				//ANS: Jus for debugging purposes
				cout<<"Error"<<endl;
			}
		}
		//ASK: Why is this essential to happen here? Can it not happpen outside the while loop? (for better efficiency)
		fhin.FlushPages();
	}
	fm->CloseFile(fhin);
	fm->ClearBuffer();
	delete fm;
	queryfile.close();
}
