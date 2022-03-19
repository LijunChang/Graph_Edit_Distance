# Graph Edit Distance Computation

This repository implements graph edit distance (GED) computation and GED verification (i.e., verify whether the GED between two (labeled) graphs is smaller than a given threshold) algorithms proposed in the following two papers. Note that, our implementations assume uniform edit cost. If you are using the code, please cite our papers.

<pre>
Lijun Chang, Xing Feng, Xuemin Lin, Lu Qin, Wenjie Zhang, Dian Ouyang.
<a href="https://lijunchang.github.io/pdf/2020-ged-icde.pdf">Speeding Up GED Verification for Graph Similarity Search.</a>
Proceedings of the 36th International Conference on Data Engineering (ICDE’20), 2020
</pre>

<pre>
Lijun Chang, Xing Feng, Kai Yao, Lu Qin, Wenjie Zhang.
<a href="https://lijunchang.github.io/pdf/2022-ged-tkde.pdf">Accelerating Graph Similarity Search via Efficient GED Computation.</a>
Accepted by TKDE in Feburary 2022
</pre>

## Compile the code

```sh
$ make clean
$ make
```
It generates an executable "ged".

## Run the code

You can find how to use the code by
```sh
$ ./ged -h
```

An example of graph similarity search on the AIDS dataset with threshold 5 is as follows
```sh
$ ./ged -d datasets/AIDS.txt -q datasets/AIDS_query100.txt -m search -p astar -l LSa -t 5
``` 

An example of one-by-one GED computation between graphs in two files is as follows
```sh
$ ./ged -d datasets/graph_g.txt -q datasets/graph_q.txt -m pair -p astar -l LSa -g
```

## Data format
t [starts a new graph, followed by two arbitrary strings]

v [vertex_id] [vertex_label]

e [vertex_id1] [vertex_id2] [edge_label]

[graph_q.txt](datasets/graph_q.txt) and [graph_g.txt](datasets/graph_g.txt) are two example data files. Note that, vertex_id must be consecutive numbers starting from 0.
