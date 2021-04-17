for pid in $(ps -ef | awk '/a.out/ {print $2}'); do kill -9 $pid; done
