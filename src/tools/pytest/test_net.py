# 导入socket库:
import socket

IP = "192.168.0.169"
PORT = 19001

# TCP客户端
class TcpClient(object):

    def __init__(self, id, ip, port):
        self.id = id
        self.ip = ip
        self.port = port
        self.fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM);

    def encode(self, input_buf):

        output_buf = bytearray()

        data_len = len(input_buf)
        all_len = data_len
        output_buf = all_len.to_bytes(4, 'big')
        output_buf += input_buf

        return output_buf

    def begin_connect(self):
        self.fd.connect((self.ip, self.port))

        print("connect success, id: %s, ip: %s, port:%d.", self.id, self.ip, self.port)

    def test_qps(self):
        const_byts_num = 200
        buffer = bytearray(const_byts_num)
        buffer = self.encode(buffer)

        print("send len: ", len(buffer))
        self.fd.send(buffer)
    
    def close(self):
        self.fd.close()

def test():

    client_list = [];

    const_client_num = 1
    for x in list(range(const_client_num)):
        client = TcpClient(x, IP, PORT)
        client.begin_connect()
        client_list.append(client)

    while (1) : 
        const_msg_num = 1
        str = input("input: \n");
        if (str == '1') :
            for v in client_list:
                for x in list(range(const_msg_num)):
                    v.test_qps()


# TCP服务端
class TcpServer(object):

    def __init__(self, ip, port):
        self.ip = ip
        self.port = port

    def encode(self, input_buf):

        output_buf = bytearray()

        data_len = len(input_buf)
        all_len = data_len
        output_buf = all_len.to_bytes(4, 'big')
        output_buf += input_buf

        return output_buf

    def begin_connect(self):
        self.fd.connect((self.ip, self.port))

        print("connect success, id: %s, ip: %s, port:%d.", self.id, self.ip, self.port)

    def test_qps(self):
        const_byts_num = 200
        buffer = bytearray(const_byts_num)
        buffer = self.encode(buffer)

        print("send len: ", len(buffer))
        self.fd.send(buffer)
    
    def close(self):
        self.fd.close()

def test():

    client_list = [];

    const_client_num = 1
    for x in list(range(const_client_num)):
        client = TcpClient(x, IP, PORT)
        client.begin_connect()
        client_list.append(client)

    while (1) : 
        const_msg_num = 1
        str = input("input: \n");
        if (str == '1') :
            for v in client_list:
                for x in list(range(const_msg_num)):
                    v.test_qps()


if __name__=='__main__':
    test()