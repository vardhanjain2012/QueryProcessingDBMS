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

using namespace std;

bool ifInPage(PageHandler pg, int val);

int isLess(PageHandler pg, int val, bool lastpage);

int getFirstPage(FileHandler fhin, int myInt);
int getLastPage(FileHandler fhin, int myInt);

void printPage(FileHandler fhin, int pg){
	char *smalldata = fhin.PageAt(pg).GetData();
	for(int i=0, num =0;i<PAGE_CONTENT_SIZE;i+= sizeof(int)){
		memcpy (&num,&smalldata[i],  sizeof(int));
		cout<<num<<",";
	}
	cout<<endl;	
}

FileManager *fm;
int main(int argc, char** argv) {

	if(argc!=4){
		cout << "Incorrect arguements!!" << endl << "Expected run command: ./binarysearch <sorted_input_filename> <query_filename>.txt <output_filename>"<<endl;
		exit(0);
	}
	const char* inputfileName  = argv[1];
	const char* queryfileName = argv[2];
	const char* outfileName = argv[3];

	fm = new FileManager();
	FileHandler fhin = fm->OpenFile(inputfileName);
	FileHandler fhout = fm->CreateFile(outfileName);
	int small(INT32_MAX), big(INT32_MIN);
	char * smalldata = fhin.FirstPage().GetData();
	char * bigdata = fhin.LastPage().GetData();

	memcpy(&small, &smalldata[0], sizeof(int));
	for(int i=0, num =0;i<PAGE_CONTENT_SIZE;i+= sizeof(int)){
		memcpy (&num,&bigdata[i],  sizeof(int));
		if(num == INT32_MIN && i!=0){
			memcpy (&big, &bigdata[i-sizeof(int)],  sizeof(int));
			break;
		}
	}

	cout<<"LIMITS:"<<small<<","<<big<<endl;
	ifstream queryfile(queryfileName);
	string myText;
	int myInt;
	PageHandler outPage = fhout.NewPage();
	int outPageNumber = 0;
	int outIndex = 0;

	cout<<"STARTING"<<endl;

	while (getline (queryfile, myText)) {
		stringstream ss(myText);
		ss>>myText;
		ss>>myInt;
		cout<<"NEW INPUT "<<myInt<<endl;
		// cout<<"Search "<<myInt<<endl;

		if(myInt>=small && myInt<=big){
			//get first and last occurence of number
			int firstPageNumber = getFirstPage(fhin,myInt);
			cout<<"GOOD -> "<<firstPageNumber<<endl;
			int lastPageNumber = getLastPage(fhin, myInt);
			cout<<"GREAT -> "<<lastPageNumber<<endl;

			printPage(fhin, firstPageNumber);
			printPage(fhin, lastPageNumber);
			PageHandler firstPage = fhin.PageAt(firstPageNumber);
			PageHandler lastPage = fhin.PageAt(lastPageNumber);

			int firstIndex(-1), lastIndex(-1);
			char* data = firstPage.GetData();
			for(int i=0, num =0;i<PAGE_CONTENT_SIZE;i+= sizeof(int)){
				memcpy (&num, &data[i], sizeof(int));
				if(num == myInt){
					firstIndex = i;
					break;
				}
			}

			data = lastPage.GetData();
			for(int i=PAGE_CONTENT_SIZE-4, num =0; i>=0;i-=sizeof(int)){
				memcpy (&num, &data[i], sizeof(int));
				if(num == myInt){
					lastIndex = i;
					break;
				}			
			}

			//ready to write
			if(outIndex>=PAGE_CONTENT_SIZE&&fhout.FlushPage(outPageNumber)){
				fhout.UnpinPage(outPageNumber);
				outPage = fhout.NewPage();
				outPageNumber+=1;
				outIndex=0;
			}

			//debugging
			cout<<"The first occurence is Page, index: "<<firstPageNumber<<","<<firstIndex<<endl;
			cout<<"The last occurence is Page, index: "<<lastPageNumber<<","<<lastIndex<<endl<<endl;
			if(firstIndex != -1){
				//write all occurences in between
				while(firstPageNumber!=lastPageNumber || firstIndex!=lastIndex){
					//ready to write
					if(outIndex>=PAGE_CONTENT_SIZE&&fhout.FlushPage(outPageNumber)){
						fhout.UnpinPage(outPageNumber);
						outPage = fhout.NewPage();
						outPageNumber+=1;
						outIndex=0;
					}

					//write the value
					pair<int,int> out = make_pair(firstPageNumber,firstIndex);
					char* dataOut = outPage.GetData();
					memcpy (&dataOut[outIndex], &out, sizeof(pair<int, int>));
					cout<<"----------------OUT-----"<<outPageNumber<<","<<outIndex<<" -> "<<out.first<<","<<out.second<<endl;
					fhout.MarkDirty(outPageNumber);
					outIndex+=sizeof(pair<int, int>);

					//next page, index
					firstIndex+=sizeof(int);
					if(firstIndex>=PAGE_CONTENT_SIZE){
						firstPageNumber+=1;
						firstIndex =0;
					}
				}

				//write the last value
				//ready to write
				if(outIndex>=PAGE_CONTENT_SIZE&&fhout.FlushPage(outPageNumber)){
					fhout.UnpinPage(outPageNumber);
					outPage = fhout.NewPage();
					outPageNumber+=1;
					outIndex=0;
				}

				pair<int,int> out = make_pair(firstPageNumber,firstIndex);
				char* dataOut = outPage.GetData();
				memcpy (&dataOut[outIndex], &out, sizeof(pair<int, int>));
				cout<<"----------------OUT-----"<<outPageNumber<<","<<outIndex<<" -> "<<out.first<<","<<out.second<<endl;
				fhout.MarkDirty(outPageNumber);
				outIndex+=sizeof(pair<int, int>);
			}
		}

		
		//ready to write
		if(outIndex>=PAGE_CONTENT_SIZE&&fhout.FlushPage(outPageNumber)){
			fhout.UnpinPage(outPageNumber);
			outPage = fhout.NewPage();
			outPageNumber+=1;
			outIndex=0;
		}
		//write (-1,-1) in the end;
		pair<int,int> out = make_pair(-1,-1);
		char* dataOut = outPage.GetData();
		memcpy (&dataOut[outIndex], &out, sizeof(pair<int, int>));
		cout<<"----------------OUT-----"<<outPageNumber<<","<<outIndex<<" -> "<<out.first<<","<<out.second<<endl;
		fhout.MarkDirty(outPageNumber);
		fhout.FlushPage(outPageNumber);
		outIndex+=sizeof(pair<int, int>);
		fhout.FlushPages();
		fm->ClearBuffer();
	}

	fm->CloseFile(fhin);
	fm->CloseFile(fhout);
	fm->ClearBuffer();
	delete fm;
	queryfile.close();
	return 0;
}


bool ifInPage(PageHandler pg, int val){
	char *data = pg.GetData();
	int num(0);

	for(int i=0;i<PAGE_CONTENT_SIZE;i+=sizeof(int)){
		memcpy (&num, &data[i], sizeof(int));
		if(num == val)
			return true;
	}
	return false;
}

//this function takes pageHandler, and checks if the values in page are less than or greater than val
// but returns integer because there are several possible cases.
//lastpage is there because the lastpage of file can be half filled, so to handle that.
int isLess(PageHandler pg, int val, bool lastpage) {
	char *data = pg.GetData();
	int nums(0), numl(0);
	memcpy(&nums, &data[0], sizeof(int));
	if(nums == INT32_MIN){
		cerr<<"GIVEN PAGE IS EMPTY!!"<<endl;
		exit(0);		
	}

	if(!lastpage)
		memcpy(&numl, &data[PAGE_CONTENT_SIZE-sizeof(int)], sizeof(int));
	else{
		for(int i=0;i<PAGE_CONTENT_SIZE;i+=sizeof(int)){
			memcpy (&numl, &data[i], sizeof(int));
			if(numl == INT32_MIN){
				// cout<<"WOW"<<endl;
				memcpy (&numl, &data[i-4], sizeof(int));
				// cout<<nums<<numl<<val<<endl;
				break;
			}
		}
	}

	// +2 if all values in page are greater than val.
	if(nums>val)
		return 2;
	// -2 if all values in page are smaller than val.
	if(numl<val)
		return -2;
	if(nums == val && numl > val)
		return 1;	//if last value is greater than val.
	if(nums<val && numl == val)
		return -1;	// if first value is less than val.
	if(nums<val && numl>val)
		return 3;  //if both the above cases simultaneously occur.
	return 0;
}


int getFirstPage(FileHandler fhin, int myInt){

	PageHandler firstPageObj = fhin.FirstPage();
	PageHandler *firstPage = &firstPageObj;
	int firstPageNumber = firstPage->GetPageNum();
	// fhin.UnpinPage(firstPageNumber);

	PageHandler lastPageObj = fhin.LastPage();
	PageHandler *lastPage = &lastPageObj;
	int lastPageNumber = lastPage->GetPageNum();
	// fhin.UnpinPage(lastPageNumber);
	
	bool isLastPage = true;
	int a(-1),b(-1);
	while(firstPageNumber<lastPageNumber){
		int a = isLess(firstPageObj, myInt, false);
		int b = isLess(lastPageObj, myInt, isLastPage);
		if(a == -1 || a == 0 || a == 3){
			fhin.UnpinPage(lastPageNumber);
			break;
		}
		if(a == -2){
			int midPageNumber = (int)(firstPageNumber+lastPageNumber)/2;
			PageHandler midPageObj = fhin.PageAt(midPageNumber);
			PageHandler *midPage = &midPageObj;
			int c = isLess(midPageObj, myInt,false);
			if(c == -2){
				fhin.UnpinPage(firstPageNumber);
				fhin.UnpinPage(midPageNumber);
				firstPageNumber = midPageNumber+1;
				PageHandler temp = fhin.PageAt(midPageNumber+1);
				firstPageObj = temp;
			}
			else if(c == -1 || c == 3){
				fhin.UnpinPage(lastPageNumber);
				fhin.UnpinPage(firstPageNumber);					
				firstPageNumber = midPageNumber;
				firstPageObj = midPageObj;
				break;								
			}
			else if(c >= 0){
				fhin.UnpinPage(lastPageNumber);
				lastPageNumber = midPageNumber;
				lastPageObj = midPageObj;
				isLastPage = false;
			}
		}			
	}

	return firstPageNumber;
}

int getLastPage(FileHandler fhin, int myInt){

	PageHandler firstPageObj = fhin.FirstPage();
	PageHandler *firstPage = &firstPageObj;
	int firstPageNumber = firstPage->GetPageNum();
	// fhin.UnpinPage(firstPageNumber);

	PageHandler lastPageObj = fhin.LastPage();
	PageHandler *lastPage = &lastPageObj;
	int lastPageNumber = lastPage->GetPageNum();
	// fhin.UnpinPage(lastPageNumber);
	
	PageHandler midPageObj;
	PageHandler *midPage;
	bool isLastPage = true;
	int a(-1),b(-1);
	while(firstPageNumber<lastPageNumber){
		int a = isLess(firstPageObj, myInt, false);
		int b = isLess(lastPageObj, myInt, isLastPage);
		// cout<<firstPageNumber<<","<<lastPageNumber<<","<<a<<","<<b<<endl;
		if(b == 1 || b == 0 || b == 3){
			fhin.UnpinPage(firstPageNumber);
			break;
		}
		if(b == 2){
			int midPageNumber = (int)(firstPageNumber+lastPageNumber)/2;
			midPageObj = fhin.PageAt(midPageNumber);
			// midPage = &midPageObj;
			int c = isLess(midPageObj, myInt,false);
			// cout<<c<<endl;
			// cout<<"Extra"<<isLess(lastPageObj, myInt, isLastPage);
			if(c == 2){
				fhin.UnpinPage(lastPageNumber);
				lastPageNumber = midPageNumber;
				lastPageObj = midPageObj;
				isLastPage = false;
			}
			else if(c == 1 || c == 3){
				fhin.UnpinPage(lastPageNumber);
				fhin.UnpinPage(firstPageNumber);					
				lastPageNumber = midPageNumber;
				lastPageObj = midPageObj;
				break;								
			}
			else if(c == -2){
				fhin.UnpinPage(firstPageNumber);
				fhin.UnpinPage(midPageNumber);
				firstPageNumber = midPageNumber+1;
				PageHandler temp = fhin.PageAt(midPageNumber+1);
				firstPageObj = temp;
			}
			else if(c == 0 || c == -1){
				PageHandler temp = fhin.PageAt(midPageNumber+1);
				if(ifInPage(temp, myInt)){
					fhin.UnpinPage(firstPageNumber);
					fhin.UnpinPage(midPageNumber);
					firstPageNumber = midPageNumber+1;
					firstPageObj = temp;				
				}
				else{
					fhin.UnpinPage(firstPageNumber);
					fhin.UnpinPage(lastPageNumber);
					lastPageNumber = midPageNumber;
					lastPageObj = midPageObj;
					break;				
				}
			}
			
		}			
	}

	return lastPageNumber;
}
