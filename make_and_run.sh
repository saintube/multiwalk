#! /bin/bash

# v1.3 update at 06/11 17:02

function GraphLiteTest()
{

if [ $# != 3 ]; then

  echo "usage: $0 <input_file_prefix> <output_file_prefix> <number of workers>"

elif [ $3 -lt 1 -o $3 -gt 16 ]; then

  echo "Error: Invalid parallelsim!"

else

  echo $1
  echo $3

  hash-partitioner.pl $1 $3
  infiles="$1_$3w"
  sync;
 # sync; echo 3 > /proc/sys/vm/drop_caches

  echo "make_and_run.sh: start"
  start=$(date +%s.%N)

  start-graphlite multi_walk.so $infiles $2 >> tmp.dat

  end=$(date +%s.%N)
  echo "make_and_run.sh: end"

  start_s=$(echo $start | cut -d '.' -f 1)
  start_ns=$(echo $start | cut -d '.' -f 2)
  end_s=$(echo $end | cut -d '.' -f 1)
  end_ns=$(echo $end | cut -d '.' -f 2)

  time=$(( (10#$end_s - 10#$start_s) * 1000 + (10#$end_ns - 10#$start_ns) / 1000000))
  echo "GraphLite: input $1 parallel $3 time: $time ms" >> result_time
  tail -1 tmp.dat >> result_time
fi

}


echo "make"
make

if [ $# != 2 ]; then
  echo "usage: $0 <input_file_prefix> <output_file_prefix>"
fi
sync;
#sync; echo 3 > /proc/sys/vm/drop_caches
start=$(date +%s.%N)
./a.out $1 >> result_time
end=$(date +%s.%N)

start_s=$(echo $start | cut -d '.' -f 1)
start_ns=$(echo $start | cut -d '.' -f 2)
end_s=$(echo $end | cut -d '.' -f 1)
end_ns=$(echo $end | cut -d '.' -f 2)

time=$(( (10#$end_s - 10#$start_s) * 1000 + (10#$end_ns - 10#$start_ns) / 1000000))
echo "se: input $1 time: $time ms" >> result_time


for p in 1 2 4 8 16
do
  GraphLiteTest $1 $2 $p
done
