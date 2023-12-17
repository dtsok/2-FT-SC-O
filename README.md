# 2-FT-SC-O
Source code for the 2-Fault-Tolerant Strong Connectivity Oracles paper accepted in ALENEX24. Link for the full version (arXiv) https://arxiv.org/abs/2311.00854

1. Compile: Open a terminal in 2ftsco-project/ directory and write **make && make clean**
2. Run: Use the executable **main** as follows:
    1. ./main `<input_graph>` `<int>` `<bool>`
    - `<input_graph>` is the input graph file. Some instances are in the graphs/ subdirectory.
    - `<int>` integer $0\leq x \leq 4$. Select algorithm for the decomposition tree.
        - 0 select random node
        - 1 select most critical node (MCN)
        - 2 select label propagation node
        - 3 select page rank node
        - 4 use q-separator and mcn heuristic        
    - `<bool>` 0 [false]/1 [true]
        - If true (1) then the auxilliary data structures (2-FT-SSR-O, 1-FT-SC-O) will be initialized for every tree node. When the decomposition is done 1.000.000 (1M) random queries will be tested. Note that selecting bool as true the decomposition scheme will take much longer time to complete.
        - If false (0) only the decomposition tree is beeing constructed.

    2. Examples:
        - ./main graphs/Rome99.txt 1 1 (selecting Rome99 graph, MCN node for the decomposition tree, initializing aux ds and answering queries)
        - ./main graphs/twitter.txt 3 0 (construct only the decomposition tree of twitter graph while removing the page rank node)
3. Notes:
    - If you select algorithm 4, q-separator + mcn, you cannot initialize the extra data structures and answer queries because it is not supported yet.
    - Input graph file should have the format of the instances in graph/ subdirectory.
        - Vertices should be labeled from 1 to N = number_of_nodes
        - First line of the file should be "p number_of_nodes number_of_edges"
        - Next lines (edges of the graph) "a from_vertex to_vertex"

**Acknowledgement:** Part of the code is taken from the work of Georgiadis, Italiano, Paudel: *Computing Critical Nodes in Directed Graphs* (https://dl.acm.org/doi/10.1145/3228332)
