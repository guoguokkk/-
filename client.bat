@echo off

::�����IP��ַ
::set cmd="strIP=192.168.153.129"
set cmd="strIP=fe80::61dd:873a:c1a0:ab7c%8"

::����˶˿�
set cmd=%cmd% nPort=8099

::�����߳�����
set cmd=%cmd% nThread=1

::ÿ�������̣߳��������ٸ��ͻ���
set cmd=%cmd% nClient=1

::::::���ݻ���д�뷢�ͻ�����
::::::�ȴ�socket��дʱ��ʵ�ʷ���
::ÿ���ͻ�����nSendSleep(����)ʱ����
::����д��nMsg��Login��Ϣ

::ÿ����Ϣ100�ֽڣ�Login��
set cmd=%cmd% nMsg=1
set cmd=%cmd% nSendSleep=100

::�ͻ��˷��ͻ�������С���ֽڣ�
set cmd=%cmd% nSendBufSize=81920

::�ͻ��˽��ջ�������С���ֽڣ�
set cmd=%cmd% nRecvBufSize=81920

::�����յ��ķ������ϢID�Ƿ�����
set cmd=%cmd% -checkMsgID

::�������� �������
F:\AA\bin\x64\Release\Client.exe %cmd%

pause