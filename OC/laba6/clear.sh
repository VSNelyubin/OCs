#!/bin/sh
for pid in $(ps -ef | awk '/interface.out/ {print $2}'); do kill -9 $pid; done
for pid in $(ps -ef | awk '/node.out/ {print $2}'); do kill -9 $pid; done
for pid in $(ps -ef | awk '/nodo.out/ {print $2}'); do kill -9 $pid; done
unexpand node2.cpp > tmpfile1
cat tmpfile1 > node2.cpp
unexpand interface.cpp > tmpfile2
cat tmpfile2 > interface.cpp
