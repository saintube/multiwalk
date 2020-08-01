MultiWalk
=============

This is a graph computing implementation of network embedding algorithm [DeepWalk](https://arxiv.org/abs/1403.6652) and is based on [GraphLite](https://github.com/schencoding/GraphLite/tree/master/GraphLite-0.20). In a short word, it uses GraphLite to compute the [random walk sequence](https://www.investopedia.com/terms/r/randomwalktheory.asp).  

## Prerequisites

1. JDK (> 1.7)  
2. Hadoop (> 2.6.0)  
3. [protocol buffers](https://developers.google.cn/protocol-buffers/docs/downloads)  
4. [GraphLite-0.20](https://github.com/schencoding/GraphLite/tree/master/GraphLite-0.20)  

## Build

```console
cd GraphLite/GraphLite-0.20/example/  
```

Then please modify `Makefile`:
```
    EXAMPLE_ALGOS=PageRankVertex
```
to:
```
    EXAMPLE_ALGOS=multi_walk  
```

```console
make
```

## Run

Before running, you should decide what the parallelism (i.e. the number of workers) is and run the command below to partition the input.  
```console
hash-partitioner.pl <input_path> <parallelism>  
```

Then use graphlite to generate the results.  
```console
start-graphlite multi_walk.so <partitioned_input_path> <output_path>
```

## Quick Start (recommended)

Actually, we have simplified the procedure by using scripts, which you need not to build and partition by yourself.  
The things you only need to do is to modify the Makefile and run the command below.  

### test all parallelism (1, 2, 4, 8, 16)
```console
make_and_run.sh <input_path> <output_path> 
```

### simply test given datasets
```console
test.sh
```

Notice: all results would be saved as `tmp.dat`.

## References

[DeepWalk: Online Learning of Social Representations](https://arxiv.org/abs/1403.6652)  

[GraphLite: A lightweight graph computation platform in C/C++](https://github.com/schencoding/GraphLite)


