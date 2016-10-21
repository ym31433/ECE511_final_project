#!/bin/bash
BENCHMARK=/research/yhtseng2/axbench/applications/inversek2j
FILE=$BENCHMARK/train.data/input/*.data
FILENAME=$(basename "$FILE")

build/X86/gem5.opt --outdir=m5out/inversek2j configs/example/se.py --cpu-type=timing -c $BENCHMARK/bin/inversek2j.out -o "$FILE $BENCHMARK/train.data/output/${FILENAME}_inversek2j_gem5_out.data" &> final_project/log/inversek2j_orig.txt
