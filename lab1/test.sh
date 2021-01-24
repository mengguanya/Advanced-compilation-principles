#!/bin/bash
cd ./examples/collatz/
gcc ./collatz_new.c -o collatz
echo "************collatz***************"
./collatz

cd ../gcd/
gcc ./gcd_new.c -o gcd
echo "************gcd*******************"
./gcd

cd ../hanoifibfac/
gcc ./hanoifibfac_new.c -o hanoifibfac
echo "***********hanoifibfac************"
./hanoifibfac

cd ../loop/
gcc ./loop_new.c -o loop
echo "***********loop********************"
./loop

cd ../prime/
gcc ./prime_new.c -o prime
echo "**********prime*******************"
./prime

cd ../sieve/
gcc ./sieve_new.c -o sieve
echo "*********sieve********************"
./sieve

cd ../sort/
gcc ./sort_new.c -o sort
echo "**********sort********************"
./sort
