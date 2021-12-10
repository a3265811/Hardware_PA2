#!/bin/bash

limit_time=3600
d1=$(date +"%s")
check_flag=0
new=0

read -p "running mode(simulation or result parser):" mode

if [ ${mode} = "r" ]
then
	reportDirList=$(ls ./report)
	for r_dir in ${reportDirList}
	do
		if test -d ./report/${r_dir}
		then
			echo "benchmark,#input,#keys,#outputs,#gates,#iteration,cpu_time" > ./report/${r_dir}.csv
			echo "---------------------------------------------------------"
			echo "Now directory: "${r_dir}
			reportFileList=$(ls ./report/${r_dir})
			for r_file in ${reportFileList}
			do
				./parser.o ./report/${r_dir}/${r_file} ./report/${r_dir}.csv
			done
			echo "---------------------------------------------------------"
		fi
	done
elif [ "${mode}" = "s" ]
then
	while [ ${check_flag} -ne 1 ]
	do
		read -p "Please input directory name: " dir
		
		if  test -d ./benchmarks/${dir}
		then
			echo "Your input directory is: " ${dir}
			check_flag=1
		else
			echo "Can't find input directory!!"
		fi
	done
	
	check_flag=0
	while [ ${check_flag} -ne 1 ]
	do
		read -p "Last simulate benchmark name: " l_bench
		if [ ${l_bench} = "none" ]
		then
			check_flag=1
			new=1
		elif  test -f ./benchmarks/${dir}/${l_bench}
		then
			echo "Your input benchmark name is: " ${l_bench}
			check_flag=1
		else
			echo "Can't find input benchmark name!!"
		fi
	done
	
	check_flag=0
	while [ ${check_flag} -ne 1 ]
	do
		read -p "Original benchmark directory name: " ori_dir
		if  test -d ./benchmarks/${ori_dir}
		then
			echo "Original benchmark directory name is: " ${ori_dir}
			check_flag=1
		else
			echo "Can't find original benchmark directory name!!"
		fi
	done
	
	filelist=$(ls ./benchmarks/${dir})
	check_flag=0
	if [ ${new} -eq 1 ]
	then
		check_flag=1
	fi
	
	echo "Execution Log:" > ./execution_log.txt
	for filename in ${filelist}
	do
		if [ ${filename} != ${l_bench} ] && [ ${check_flag} -ne 1 ]
		then
			continue
		else
			check_flag=1
		fi
		d2=$(date +"%s")
		ori_name=$(echo ${filename} | awk -F '_' '{print $1}')".bench"
		timeout 2h ./bin/sld ./benchmarks/${dir}/${filename} ./benchmarks/${ori_dir}/${ori_name} > ./report/${filename}.txt
		if [ $? = 124 ]
		then
			echo "Unfinished execution file: "${dir}/${filename} >> ./execution_log.txt
			echo "Please check Unfinished execution file!!"
			mv ./report/${filename}.txt ./report/${filename}_unfinished.txt
		else
			echo "Successful execution file: "${dir}/${filename} >> ./execution_log.txt
		fi
		d3=$(date +"%s")
		echo ${filename} "used $((${d3}-${d2})) seconds"
	#	if [ $((${d3}-${d1})) -gt ${limit_time} ]
	#	then
	#		read -p "run out of time limit, do you want continue?" yn
	#		if [ "${yn}" = "Y" -o "${yn}" = "y" ] 
	#		then 
	#			echo "OK, continue."
	#			d1=$(date +"%s")
	#		elif [ "${yn}" = "N" -o "${yn}" = "n" ]
	#		then 
	#			echo "Oh, interrupt!" && exit 0
	#		else
	#			echo "Can't understand command!! then quit." && exit 0
	#		fi
	#	fi

	done

	mkdir -p ./report/${dir}
	mv ./report/*.txt ./report/${dir}
	mv ./execution_log.txt ./report/${dir}
	echo "Total execution file: " && ls -l ./report/${dir}/*.txt | wc -l
else
	echo "Can't understand input command, please try r(report) or s(simulation)."
fi

exit 0
