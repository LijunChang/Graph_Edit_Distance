# Graph_Edit_Distance

This project includes the codes for exactly computing and verifying the graph edit distance between two graphs. It outperforms the state-of-the-art algorithms by several orders of magnitude. Details can be found in our paper https://lijunchang.github.io/ged.pdf.

[ged](ged) is the executable, and is compiled on macOS Sierra. ged_debian is the exectuable compiled on Debian.

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
Lijun Chang, Xing Feng, Xuemin Lin, Lu Qin, Wenjie Zhang:
Efficient Graph Edit Distance Computation and Verification via Anchor-aware Lower Bound Estimation.
CoRR abs/1709.06810 (2017)
```

```
@article{DBLP:journals/corr/abs-1709-06810,
  author    = {Lijun Chang and
               Xing Feng and
               Xuemin Lin and
               Lu Qin and
               Wenjie Zhang},
  title     = {Efficient Graph Edit Distance Computation and Verification via Anchor-aware
               Lower Bound Estimation},
  journal   = {CoRR},
  volume    = {abs/1709.06810},
  year      = {2017},
  url       = {http://arxiv.org/abs/1709.06810},
  archivePrefix = {arXiv},
  eprint    = {1709.06810},
  timestamp = {Thu, 05 Oct 2017 09:42:50 +0200},
  biburl    = {http://dblp.org/rec/bib/journals/corr/abs-1709-06810},
  bibsource = {dblp computer science bibliography, http://dblp.org}
}
```
