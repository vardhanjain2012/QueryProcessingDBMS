// TORUN: ./join1 TestCases/TC_join1/input1_join1 TestCases/TC_join1/input2_join1 TestCases/TC_join1/myOutput

#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include<cstring>

#include <climits>
#include <cstdint> 

using namespace std;

FileManager *fm;

int main(int argc, char** argv) {

	if(argc!=4){
		cout << "Incorrect arguements!!" << endl << "Expected run command: ./join1 <unsorted_input_file1name> <unsorted_input_file2name> <output_filename>"<<endl;
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

	for(int currPageNum1 = startPageNum1; currPageNum1<=endPageNum1; currPageNum1+=1){
		inputPage1 = fhin1.PageAt(currPageNum1);
		char* dataIn1 = inputPage1.GetData();

		for(int currPageNum2 = startPageNum2; currPageNum2<=endPageNum2; currPageNum2+=1){
			inputPage2 = fhin2.PageAt(currPageNum2);
			char* dataIn2 = inputPage2.GetData();

			int num1, num2;

			for(int i=0; i<PAGE_CONTENT_SIZE; i+=sizeof(int)){
				memcpy (&num1, &dataIn1[i], sizeof(int));
				if(num1 == INT_MIN){
						break;
				}
				for(int j=0; j<PAGE_CONTENT_SIZE; j+=sizeof(int)){
					memcpy (&num2, &dataIn2[j], sizeof(int));
					if(num2 == INT_MIN){
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
			}
			fhin2.UnpinPage(currPageNum2);
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
