#########################################################################################
#		Makefile for the 2-FAULT TOLERANT STRONG CONNECTIVITY ORACLE		#
#########################################################################################

TARGET  = main 
SOURCES = common/connected_component.cpp common/dimacs_graph.cpp \
common/strongly_connected_component.cpp common/timer_cndp.cpp \
common/dominator.cpp common/loop_nesting.cpp \
scc/scc_input_graph.cpp \
algorithms-n-heuristics/linear-time/cndp_linear_time.cpp \
algorithms-n-heuristics/linear-time/dominator_only.cpp \
algorithms-n-heuristics/linear-time/lnf_and_dominator.cpp \
algorithms-n-heuristics/linear-time/lnf_only.cpp \
algorithms-n-heuristics/linear-time/tree_graph.cpp \
algorithms-n-heuristics/linear-time/trees.cpp \
algorithms-n-heuristics/page-rank/page_rank.cpp \
algorithms-n-heuristics/label-propagation/label_propagation.cpp \
algorithms-n-heuristics/q-separator/bfs_tree.cpp \
algorithms-n-heuristics/special-nodes-sap-2vcuts/sap_2vcuts.cpp \
algorithms-n-heuristics/special-nodes-sap-2vcuts/dominator_only.cpp \
sccs-tarjan/sccs.cpp \
tree-node-srcs/tree_node.cpp \
choudhary-srcs/choudhary.cpp \
rmq-srcs/RMQ.cpp \
one-ftsco/oneFTSC.cpp \
main.cpp

GCC_NAME    = g++
GCC_FLAGS   = -g -Wall -O3 -ansi -pedantic -fpermissive -std=c++11
GCC_LIBS    = -lm -L/usr/lib/ -lrt  -static-libstdc++
GCC_DEFINES = -DRFW_RUSAGE
GCC_OBJECTS = $(SOURCES:.cpp=.o)

VCC_NAME    = cl
VCC_FLAGS   = -O2 /W4
VCC_DEFINES = -DWIN32 -DNDEBUG -D_CONSOLE
VCC_LIBS    = -lm
VCC_OBJECTS = $(SOURCES:.cpp=.obj)

CC_NAME     = CC
CC_FLAGS    = -O3 -OPT:Olimit_opt=on -LANG:std
#CC_FLAGS    = -LANG:std
CC_DEFINES  = -DRFW_RUSAGE
CC_LIBS     = -lm -L/usr/lib32/mips3/
CC_OBJECTS  = $(SOURCES:.cpp=.o)

#
# CHANGE THESE LINES TO USE YOUR FAVORITE COMPILER
CCC      = $(GCC_NAME)
FLAGS    = $(GCC_FLAGS)
LIBS     = $(GCC_LIBS)
DEFINES  = $(GCC_DEFINES)
OBJECTS  = $(GCC_OBJECTS)
INCLUDES = -I.

.SUFFIXES: .cpp

# $< is replaced with name of the input file
# $@ is replaced with the name out the output file.

.cpp.o:
	$(CCC) $(DEFINES) $(FLAGS) -c $< -o $@

.cpp.obj:
	$(CCC) $(DEFINES) $(FLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CCC) $(FLAGS) $(DEFINES) $(INCLUDES) $(OBJECTS) $(LIBS) -o $(TARGET)


all: make clean

clean:
	rm -f $(OBJECTS) #$(TARGET) *~
