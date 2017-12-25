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

