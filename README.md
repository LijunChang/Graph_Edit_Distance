# Graph_Edit_Distance

## Running format 
./ged [1. query_graph_file] [2. data_graph_file] [3. “astar” or “bfs”] [4. “LS” or “LSa” or “BM” or “BMa” or “SM” or “SMa” or “BMb”] [5. threshold, optional]

Computes or verifies GED between graphs in query_graph_file and graphs in data_graph_file in a pairwise manner

### Running Example for GED computation
```
./ged graph_q.txt graph_g.txt astar BMa
```
### Running Example for GED verification
```
./ged graph_q.txt graph_g.txt astar BMa 7
```
Note that, the code is compiled on Debian Linux System. The fastest algorithm is astar + BMa

## graph format
t [starts a new graph, followed by two arbitrary strings]

v [vertex_id] [vertex_label]

e [vertex_id1] [vertex_id2] [edge_label]

# Note
If you have any question, please contact ljchang@outlook.com.

If you are using this code, please kindly cite the following paper.

Lijun Chang, Wei Li, Xuemin Lin, Lu Qin, Wenjie Zhang,
"pSCAN: Fast and Exact Structural Graph Clustering"
Proceedings of the 31st International Conference on Data Engineering (ICDE’16), 2016

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
