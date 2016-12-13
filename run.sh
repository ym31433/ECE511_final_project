#!/bin/bash

###### parse arguments ######
# gem5 version
if [ "$#" -eq 3 ] && [ "$3" == "opt" -o "$3" == "fast" ]; then
    gem5_version=${3}
elif [ "$#" -eq 2 ]; then #default
    gem5_version="opt"
else
    echo "Wrong arguments!"
    echo "Usage: ./run.sh [orig/nn] [benchmark] [opt/fast]"
    exit 1
fi
# benchmark
benchmark_name=${2}
if [ ${benchmark_name} != "inversek2j" -a ${benchmark_name} != "fft" ]; then
    echo "Wrong arguments!"
    echo "Usage: ./run.sh [orig/nn] [benchmark] [opt/fast]"
    exit 1
fi
echo "benchmark: ${benchmark_name}."
# original version or nn version
version=${1}
if [ ${version} == "orig" ]; then
    script="se.py"
    exe="${benchmark_name}.out"
elif [ ${version} == "nn" ]; then
    script="npu_se.py"
    exe="${benchmark_name}.nn.out"
else
    echo "Wrong arguments!"
    echo "Usage: ./run.sh [orig/nn] [benchmark] [opt/fast]"
    exit 1
fi

##### set path #####
HOME=/home/cosine/ECE511_final_project
BENCHMARK=${HOME}/benchmark/${benchmark_name}
IN_FILE=${BENCHMARK}/test.data/input/*.data
FILENAME=$(basename "$IN_FILE")
OUT_FILE=${BENCHMARK}/test.data/output/${FILENAME}_${benchmark_name}_gem5_out_${version}.data
LOG_FILE=${BENCHMARK}/log/${benchmark_name}_${version}.txt

##### run #####
case ${benchmark_name} in
    inversek2j)
        ${HOME}/gem5/build/X86/gem5.${gem5_version} --outdir=${BENCHMARK}/m5out/${version} ${HOME}/gem5/configs/example/${script} --cpu-type=npu_cpu -c ${BENCHMARK}/bin/${exe} -o "${IN_FILE} ${OUT_FILE}" &> ${LOG_FILE}
        ;;
    fft)
        ${HOME}/gem5/build/X86/gem5.${gem5_version} --outdir=${BENCHMARK}/m5out/${version} ${HOME}/gem5/configs/example/${script} --cpu-type=npu_cpu -c ${BENCHMARK}/bin/${exe} -o "32768 ${OUT_FILE}" &> ${LOG_FILE}
        ;;
    *)
        echo "no default benchmark name allowed."
        ;;
esac
