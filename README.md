# -NeuTalk
## 大二暑期小学期实践项目 聊天软件 NeuTalk


## 开发环境：Ubuntu20.04.2 LTS、Qt 5.12.11


开发时长一共十天，ui是其他同学做的，代码部分我负责


## 软件介绍


使用请先在linux系统上安装相应版本的QT
打开.pro文件build后运行即可


服务端会自行获取localhost的ip地址(含ipv6,ipv4)


点击start server后服务端开启 按钮变为stop server 点击关闭服务端


客户端需在服务端开启后才能连接和注册


并且请手动在clientsocket.cpp的void ClientSocket::ConnectToServer()函数中将ip改为服务端ip地址


## 下面是一些软件截图


### 登录界面
![Image-text](https://github.com/lihanddd/img/blob/main/1.png)
### 注册页面
![Image-text](https://github.com/lihanddd/img/blob/main/2.png)
### 好友页面
![Image-text](https://github.com/lihanddd/img/blob/main/3.png)
### 聊天页面
![Image-text](https://github.com/lihanddd/img/blob/main/4.png)
![Image-text](https://github.com/lihanddd/img/blob/main/5.png)
