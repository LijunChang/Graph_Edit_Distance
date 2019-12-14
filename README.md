# Graph_Edit_Distance

This project aims at exact graph edit distance (GED) computation and GED verification (verify whether the GED between two  (labeled) graphs is smaller than a given threshold), where all edit operators are assumed to have unit costs. It outperforms the state-of-the-art algorithms by several orders of magnitude. Details can be found in our paper https://lijunchang.github.io/ged.pdf.

[ged](ged) is the executable, and is compiled on macOS Sierra. [ged_debian](ged_debian) is the exectuable compiled on Debian. 

Source code will be available soon.

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

# Note
If you have any question, please contact ljchang@outlook.com.

If you are using this code, please kindly cite the following paper.
```
Lijun Chang, Xing Feng, Xuemin Lin, Lu Qin, Wenjie Zhang, Dian Ouyang:
Speeding Up GED Verification for Graph Similarity Search.
Proceedings of the 36th International Conference on Data Engineering (ICDE’20), 2020
```

```
@inproceedings{ICDE20:Chang,
  author    = {Lijun Chang and
               Xing Feng and
               Xuemin Lin and
               Lu Qin and
               Wenjie Zhang and
               Dian Ouyang},
  title     = {Speeding Up GED Verification for Graph Similarity Search},
  booktitle = {Proc. of ICDE'20},
  year      = {2020}
}
```
