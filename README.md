# Graph_Edit_Distance

## Running format 
./ged [query_graph_file] [data_graph_file] [“astar” or “bfs”] [“LS” or “LSa” or “BM” or “BMa” or “SM” or “SMa” or “BMb”] [threshold, optional]
Computes of verifies GED between graphs in query_graph_file and graphs in data_graph_file in a pairwise manner

## Running Example for GED computation
./ged graph_q.txt graph_g.txt astar BMa

## Running Example for GED verification
./ged graph_q.txt graph_g.txt astar BMa 7

Note that, this is compiled on Debian Linux System.


## graph format
t [starts a new graph, followed by two arbitrary strings]
v [vertex_id] [vertex_label]
e [vertex_id1] [vertex_id2] [edge_label]
