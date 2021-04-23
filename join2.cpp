// TORUN: ./join2 TestCases/TC_join2/input1_join2 TestCases/TC_join2/input2_join2_updated TestCases/TC_join2/myOutput
// cmp TestCases/TC_join2/myOutput TestCases/TC_join2/output_join2 && echo 'SUCCESS' || echo 'FAILED'

#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include<cstring>

#include <climits>
#include <cstdint> 

using namespace std;

int isLess(PageHandler pg, int val, bool lastpage);
int getFirstPage(FileHandler fhin, int val);

FileManager *fm;

int main(int argc, char** argv) {

	if(argc!=4){
		cout << "Incorrect arguements!!" << endl << "Expected run command: ./join2 <unsorted_input_file1name> <sorted_input_file2name> <output_filename>"<<endl;
		exit(0);
	}
	const char* inputfile1  = argv[1];
	const char* inputfile2 = argv[2];
	const char* outfile = argv[3];

	fm = new FileManager();

	FileHandler fhin1 = fm->OpenFile(inputfile1);
	FileHandler fhin2 = fm->OpenFile(inputfile2);
	FileHandler fhout = fm->CreateFile(outfile);

	PageHandler outPage = fhout.NewPage();

	int tmp = INT_MIN;
	char* dataOut_tmp = outPage.GetData();
	for(int k=0;k<PAGE_CONTENT_SIZE;k+=sizeof(int)){
		memcpy (&dataOut_tmp[k], &tmp, sizeof(int));
	}

	int outPageNumber = 0;
	int outIndex = 0;

	int startPageNum1;
	PageHandler inputPage1 = fhin1.FirstPage();
	startPageNum1 = inputPage1.GetPageNum();
	fhin1.UnpinPage(startPageNum1);
	fhin1.FlushPage(startPageNum1);
	int endPageNum1;
	inputPage1 = fhin1.LastPage();
	endPageNum1 = inputPage1.GetPageNum();
	fhin1.UnpinPage(endPageNum1);
	fhin1.FlushPage(endPageNum1);

	int startPageNum2;
	PageHandler inputPage2 = fhin2.FirstPage();
	startPageNum2 = inputPage2.GetPageNum();
	fhin2.UnpinPage(startPageNum2);
	fhin2.FlushPage(startPageNum2);

	int endPageNum2;
	inputPage2 = fhin2.LastPage();
	endPageNum2 = inputPage2.GetPageNum();
	fhin2.UnpinPage(endPageNum2);
	fhin2.FlushPage(endPageNum2);

	int num1, num2;

	for(int currPageNum1 = startPageNum1; currPageNum1<=endPageNum1; currPageNum1+=1){
		inputPage1 = fhin1.PageAt(currPageNum1);
		char* dataIn1 = inputPage1.GetData();
		for(int i=0; i<PAGE_CONTENT_SIZE; i+=sizeof(int)){
			memcpy (&num1, &dataIn1[i], sizeof(int));
			if(num1 == INT_MIN){
					break;
			}
			int currPageNum2 = getFirstPage(fhin2, num1);
			while(currPageNum2<=endPageNum2){
				
				inputPage2 = fhin2.PageAt(currPageNum2);
				char* dataIn2 = inputPage2.GetData();
				
				for(int j=0; j<PAGE_CONTENT_SIZE; j+=sizeof(int)){

					memcpy (&num2, &dataIn2[j], sizeof(int));
					if(num2 == INT_MIN || num2 > num1){
						break;
					}
					if(num1 == num2){
						if(outIndex>=PAGE_CONTENT_SIZE){
							fhout.UnpinPage(outPageNumber);
							fhout.FlushPage(outPageNumber);
							outPage = fhout.NewPage();
							int tmp = INT_MIN;
							char* dataOut_tmp = outPage.GetData();
							for(int k=0;k<PAGE_CONTENT_SIZE;k+=sizeof(int)){
								memcpy (&dataOut_tmp[k], &tmp, sizeof(int));
							}
							outPageNumber+=1;
							outIndex=0;
						}
						char* dataOut = outPage.GetData();
						memcpy (&dataOut[outIndex], &num1, sizeof(int));
						fhout.MarkDirty(outPageNumber);
						outIndex+=sizeof(int);

					}
				}

				fhin2.UnpinPage(currPageNum2);
				if(num2 == INT_MIN || num2 > num1){
						break;
				}
				else{
					currPageNum2 += 1;
				}
			}	
		}
		fhin1.UnpinPage(currPageNum1);
		fhin1.FlushPage(currPageNum1);
	}

	fhout.UnpinPage(outPageNumber);
	fhout.FlushPage(outPageNumber);

	fm->CloseFile(fhin1);
	fm->CloseFile(fhin2);
	fm->CloseFile(fhout);

	fm->ClearBuffer();

	delete fm;
	return 0;
}


// this function takes pageHandler, and checks if the values in page are less than or greater than val
// but returns integer because there are several possible cases.
// isLastpage is there because the lastpage of file can be half filled, so to handle that
int isLess(PageHandler inputPage, int val, bool isLastpage) {
	char *data = inputPage.GetData();
	int nums, nume;
	memcpy(&nums, &data[0], sizeof(int));

	if(!isLastpage)
		memcpy(&nume, &data[PAGE_CONTENT_SIZE-sizeof(int)], sizeof(int));
	else{
		for(int i = 0; i < PAGE_CONTENT_SIZE; i += sizeof(int)){
			memcpy (&nume, &data[i], sizeof(int));
			if(nume == INT_MIN){
				memcpy (&nume, &data[i-4], sizeof(int));
				break;
			}
		}
	}

	if(nums < val){
		if (nume >= val)
			return 0;
		else
			return 1;
	}
	if(nums == val)
		return -1;
	
	return INT_MIN;
	
}


int getFirstPage(FileHandler fhin, int val){

	PageHandler firstPage = fhin.FirstPage();
	int startPageNumber = firstPage.GetPageNum();
	fhin.UnpinPage(startPageNumber);
	fhin.FlushPage(startPageNumber);

	PageHandler lastPage = fhin.LastPage();
	int endPageNumber = lastPage.GetPageNum();
	fhin.UnpinPage(endPageNumber); 
	fhin.FlushPage(endPageNumber); 
	
	bool isLastPage = true;

	while(startPageNumber < endPageNumber){

		PageHandler startPage = fhin.PageAt(startPageNumber);
		int isLessStart = isLess(startPage, val, false);
		fhin.UnpinPage(startPageNumber);
		fhin.FlushPage(startPageNumber);

		int midPageNumber = floor((startPageNumber+endPageNumber)/2);

		PageHandler midPage = fhin.PageAt(midPageNumber);
		int isLessMid = isLess(midPage, val,false);
		fhin.UnpinPage(midPageNumber);
		fhin.FlushPage(midPageNumber);

		// PageHandler endPage = fhin.PageAt(endPageNumber);
		// int isLessEnd = isLess(endPage, val,false);
		// fhin.UnpinPage(endPageNumber);
		// fhin.FlushPage(endPageNumber);	

		if(isLessStart == INT_MIN){
			startPageNumber = INT_MIN;
			break;
		}
		if(isLessStart == 0){
			break;
		}
		if(isLessStart == -1){
			endPageNumber = midPageNumber;
			isLastPage = false;
		}	
		if(isLessStart == 1){
			if(isLessMid < 0)
				endPageNumber = midPageNumber;
			else
				startPageNumber = midPageNumber;
		}			
	}

	return startPageNumber;
}
