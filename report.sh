#!/bin/bash

limit_time=3600
d1=$(date +"%s")
check_flag=0
new=0


filelist=$(ls ./benchmarks/original)
check_flag=0

mkdir -p new_bench
echo "Execution Log:" > ./new_bench/execution_log.txt
for filename in ${filelist}
do
	echo "-----------------------------------------------------------------------------------"
	echo "now executing: "${filename}
	d2=$(date +"%s")
	enc_name=new_${filename}
	./M11007428.o ./benchmarks/original/${filename} ./new_bench/${enc_name}
	if [ $? = 124 ]
	then
		echo "Unfinished execution file: "${filename} >> ./new_bench/execution_log.txt
		echo "Please check Unfinished execution file!!"
		mv ./new_bench/new_${filename} ./new_bench/new_${filename}_unfinished.txt
	else
		echo "Successful execution file: "${filename} >> ./execution_log.txt
	fi
	d3=$(date +"%s")
	echo ${filename} "used $((${d3}-${d2})) seconds"
done
echo "total used $((${d3}-${d1})) seconds"

echo "Total execution file: " && ls -l ./new_bench/*.bench | wc -l
exit 0
