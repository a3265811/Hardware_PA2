#!/bin/bash

td=0
filelist=$(ls ./benchmarks/original)

read -p "method type: " type

make clean
make
mkdir -p new_bench
mkdir -p report
for filename in ${filelist}
do
	echo "-----------------------------------------------------------------------------------"
	echo "now executing: "${filename}
	enc_name=new_${filename}
	./M11007428.o ./benchmarks/original/${filename} ./new_bench/${enc_name} ${type}
	d2=$(date +"%s")
	./satattack/bin/sld ./new_bench/${enc_name} ./benchmarks/original/${filename} > ./report/${enc_name}
	d3=$(date +"%s")
	echo ${filename} "used $((${d3}-${d2})) seconds"
	td=$((${td}+${d3}-${d2}))
done
echo "-----------------------------------------------------------------------------------"
echo "total used ${td} seconds"

echo "benchmark(enc),#input,#keys,#outputs,#gates,#iteration,cpu_time(sec)" > ./enc.csv
reportFileList=$(ls ./report)
for r_file in ${reportFileList}
do
	./parser.o ./report/${r_file} ./enc.csv
done

echo "Total execution file: " && ls -l ./new_bench/*.bench | wc -l
exit 0
