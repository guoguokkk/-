::�ر����
@echo off

::�����IP��ַ
set cmd="strIP=any"

::����˶˿�
set cmd=%cmd% nPort=8099

::��Ϣ�����߳�����
set cmd=%cmd% nThread=1

::�ͻ�����������
set cmd=%cmd% nMaxClient=10000

::�ͻ��˷��ͻ�������С���ֽڣ�
set cmd=%cmd% nSendBuffSize=81920

::�ͻ��˽��ջ�������С���ֽڣ�
set cmd=%cmd% nRecvBuffSize=81920

::�յ���Ϣ�󽫷���Ӧ����Ϣ
set cmd=%cmd% -sendback

::��ʾ���ͻ�������д��
::������sendfull��ʾʱ����ʾ������Ϣ������
set cmd=%cmd% -sendfull

::�����յ��Ŀͻ�����ϢID�Ƿ�����
set cmd=%cmd% -checkMsgID

::�Զ����־ δʹ��
set cmd=%cmd% -p

::F:\AA\guoguokkk\bin\x64\Debug\Server.exe %cmd%
F:\AA\guoguokkk\bin\x64\Release\Server.exe %cmd%

::��ͣһ��
pause