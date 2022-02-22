This repository implements graph edit distance (GED) computation and GED verification (i.e., verify whether the GED between two (labeled) graphs is smaller than a given threshold) algorithms proposed in the following two papers. Note that, our implementations assume uniform edit cost. If you are using the code, please cite our papers.

```
Lijun Chang, Xing Feng, Xuemin Lin, Lu Qin, Wenjie Zhang, Dian Ouyang.
Speeding Up GED Verification for Graph Similarity Search.
Proceedings of the 36th International Conference on Data Engineering (ICDE’20), 2020
```

```
Lijun Chang, Xing Feng, Kai Yao, Lu Qin, Wenjie Zhang.
Accelerating Graph Similarity Search via Efficient GED Computation.
Accepted by TKDE in Feburary 2022
```

[graph_q.txt](graph_q.txt) and [graph_g.txt](graph_g.txt) are example graphs.

## Running Format 
./ged [1. query_graph_file] [2. data_graph_file] [3. “astar” or “bfs”] [4. “LS” or “LSa” or “BMo” or “BMao” or “SM” or “SMa” or “BMa”] [5. threshold, optional]

Computes or verifies GED between graphs in query_graph_file and graphs in data_graph_file in a pairwise manner

* **Running example for GED computation**
```
./ged graph_q.txt graph_g.txt astar BMao
```
* **Running example for GED verification**
```
./ged graph_q.txt graph_g.txt astar BMao 7
```
Note that, the fastest and scalable algorithm is astar+BMao

## Graph Format
t [starts a new graph, followed by two arbitrary strings]

v [vertex_id] [vertex_label]

e [vertex_id1] [vertex_id2] [edge_label]
