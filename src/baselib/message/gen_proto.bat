@echo ��ʼ���ɷ����pb��Ϣ

protoc.exe --cpp_out=./ game_ss.proto
protoc.exe --cpp_out=./ game_db_account.proto
protoc.exe --cpp_out=./ game_db_object.proto

@echo �����pb��Ϣ�������
pause