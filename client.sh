#服务端IP地址
cmd="strIP=127.0.0.1"
#服务端端口
cmd=$cmd" nPort=9006"
#消息处理线程数量
cmd=$cmd" nThread=1"
#客户端连接上限
cmd=$cmd" nClient=10000"
#客户端发送缓冲区大小（字节）
cmd=$cmd" nSendBufSize=10240"
#客户端接收缓冲区大小（字节）
cmd=$cmd" nRecvBufSize=10240"
#收到消息后将返回应答消息
cmd=$cmd" -sendback"
#提示发送缓冲区已写满
#当出现sendfull提示时，表示当次消息被丢弃
cmd=$cmd" -sendfull"
#检查接收到的客户端消息ID是否连续
cmd=$cmd" -checkMsgID"
#自定义标志 未使用
cmd=$cmd" -p"
#启动程序 传入参数
./client $cmd
read -p "..press any key to exit.." var
