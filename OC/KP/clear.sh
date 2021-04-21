#!/bin/sh
for pid in $(ps -ef | awk '/c.out/ {print $2}'); do kill -9 $pid; done
for pid in $(ps -ef | awk '/b.out/ {print $2}'); do kill -9 $pid; done
for pid in $(ps -ef | awk '/a.out/ {print $2}'); do kill -9 $pid; done
unexpand c.cpp > tmpfile1
cat tmpfile1 > c.cpp
unexpand a.cpp > tmpfile2
cat tmpfile2 > a.cpp
unexpand b.cpp > tmpfile3
cat tmpfile3 > b.cpp
