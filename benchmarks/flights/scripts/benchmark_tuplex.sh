#!/usr/bin/env bash
#start via nohup bash benchmark_tuplex.sh & > stdout.txt 2> stderr.txt
# use 5 runs and a timeout after 15min
NUM_RUNS=5
TIMEOUT=900

# log memory every second
date > memusage_tuplex.txt
free -s 1 -b >> memusage_tuplex.txt &
FREEPID=$!

for i in $(seq 5); do
echo "sleep for 1s to measure memory..."
sleep 1
done

for i in $(seq 1 11); do
  v=$(printf %02d $i)
  for ((r = 1; r <= NUM_RUNS; r++)); do
    LOG="tuplex-01-$v-run-$r.txt"
    echo $LOG
    timeout $TIMEOUT python3 runtuplex.py --path /disk/data/flights/ --num-months $v >$LOG 2>$LOG.stderr
  done
done

# log end date of mem log
date >> memusage_tuplex.txt
kill $FREEPID
