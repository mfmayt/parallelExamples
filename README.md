# Parallel Examples

[OpenMP](https://en.wikipedia.org/wiki/OpenMP) must be installed to run code.
```sh
$ brew install llvm
```
## Primes
To run program, open directory of the code and run following commands on terminal.
```sh
$ /usr/local/opt/llvm/bin/clang -fopenmp -L/usr/local/opt/llvm/lib primes_parallel.c -o primes
$ ./primes
```

#### Output
There are 2 output files, list of primes and a `csv` formatted file, in order of:

```
M,Openmp Loop Scheduling Method,Chunck Size, T1,T2,T4,T8,S1,S2,S3
```
where `M` is limit of max prime value,`T{number}` is time and `number` is number of processors, `S{number}` and `number` is the order of speed up.

##### Note:
This code is written on macOS Catalina and Intel Core i5.