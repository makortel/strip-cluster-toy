CXXFLAGS += -std=c++14 -g
LDFLAGS += -std=c++14
CC = c++

strip-cluster : strip-cluster.o Clusterizer.o FEDChannel.o
strip-cluster.o: strip-cluster.cc Clusterizer.h FEDChannel.h FEDZSChannelUnpacker.h
Clusterizer.o: Clusterizer.cc Clusterizer.h
FEDChannel.o : FEDChannel.cc FEDChannel.h Clusterizer.h
