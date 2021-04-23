make binarysearch
rm TestCases/TC_search/myOutputBinary
./binarysearch TestCases/TC_search/sorted_input TestCases/TC_search/query_search.txt TestCases/TC_search/myOutputBinary
diff TestCases/TC_search/myOutputBinary TestCases/TC_search/output_search
