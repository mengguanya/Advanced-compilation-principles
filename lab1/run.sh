#!/bin/bash
echo "begin compiling"
./compiler collatz
echo "collatz finished"
./compiler gcd
echo "gcd finished"
./compiler hanoifibfac
echo "hanoifibfac finished"
./compiler loop
echo "loop finished"
./compiler prime
echo "prime finished"

./compiler sieve
echo "sieve finished"

./compiler sort
echo "sort finished"

#./compiler regslarge
#echo "regslarge finished"
echo "all finished"

