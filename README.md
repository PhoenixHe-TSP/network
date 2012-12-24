首先，安装boost和g++，然后直接编译：

make -j2

然后运行server和client

./server 314159
./client localhost 314159

然后就能测在client和server间的网速了~
按照以上方法测出来的是你电脑的网卡速度，
想要测不同机子间的网速请在不同的机子上
运行client和server并调整client的运行参
数，把localhost改成server的ip。
