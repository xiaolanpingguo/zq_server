@echo ��ʼ���ɷ����pb��Ϣ

protoc.exe --cpp_out=./ test.proto
protoc.exe --cpp_out=./ game_ss.proto
protoc.exe --cpp_out=./ game_db_account.proto

@echo �����pb��Ϣ�������
pause