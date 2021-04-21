make deletion
cp TestCases/TC_delete/sorted_input TestCases/TC_delete/myOutput
./deletion TestCases/TC_delete/myOutput TestCases/TC_delete/query_delete.txt
diff TestCases/TC_delete/myOutput TestCases/TC_delete/output_delete
