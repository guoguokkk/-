#服务器端IP地址，加@之后可以不输出这句话
strIP="any"
#服务器端端口
nPort=8099
#消息处理线程数
nThread=1
#最多支持的客户端的数量
nClient=2

./server $strIP $nPort $nThread $nClient

read -p "..press any key to exit.." var
