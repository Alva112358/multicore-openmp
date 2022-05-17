#!/bin/sh

gcc -g -Wall -fopenmp -o genprime genprimes.c
gcc -g -Wall -fopenmp -o genprime_seq genprimes_sequence.c

rm -f out.txt

for N in 1000 1000000
do
  for t in 2 5 10 25 50 100
  do
    # shellcheck disable=SC2039
    for (( i=1; i<=1000; i++ ))
    do
      ./genprime $N 1 >> seq.txt
      ./genprime $N $t >> par.txt
    done
    seq=$(awk '{ total += $7; count++ } END { print total/count }' seq.txt)
    par=$(awk '{ total += $7; count++ } END { print total/count }' par.txt)
    echo "$N $t $seq $par" >> result.txt
    awk '{ speedup = $3/$4; res = $1" "$2" seq="$3" par="$4" speedup="speedup } END { print res }' result.txt >> out.txt
    rm -f result.txt
    rm -f seq.txt
    rm -f par.txt
  done
done