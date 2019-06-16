::服务器端IP地址，加@之后可以不输出这句话
@set strIP=any
:: 服务器端端口
@set nPort=8099
::消息处理线程数
@set nThread=1
::最多支持的客户端的数量
@set nClient=2

F:\AA\guoguokkk\bin\x64\Debug\Server.exe %strIP% %nPort% %nThread% %nClient%

::暂停一下
@pause