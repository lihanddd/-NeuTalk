# -NeuTalk
大二暑期小学期实践项目 聊天软件 NeuTalk


开发环境：Ubuntu20.04.2 LTS、Qt 5.12.11


使用请先在linux系统上安装相应版本的QT
打开.pro文件build后运行即可


服务端会自行获取localhost的ip地址(含ipv6,ipv4)


点击start server后服务端开启 按钮变为stop server 点击关闭服务端


客户端需在服务端开启后才能连接和注册


并且请手动在clientsocket.cpp的void ClientSocket::ConnectToServer()函数中将ip改为服务端ip地址
