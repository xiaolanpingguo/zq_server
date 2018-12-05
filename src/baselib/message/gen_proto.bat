@echo 开始生成服务端pb消息

protoc.exe --cpp_out=./ game_ss.proto
protoc.exe --cpp_out=./ game_db_account.proto
protoc.exe --cpp_out=./ game_db_object.proto

@echo 服务端pb消息生成完成
pause