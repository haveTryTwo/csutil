csutil [英文](https://github.com/haveTryTwo/csutil/blob/master/Readme.md)
=====

# 简介
csutil 是一个方便用户编写网络程序的工具库，用户要进行网络编程只需要进行调用库中Client/Server类即可达到目的；

# 功能
1. Client/Server 提供非阻塞网络处理
2. Client/Server 提供网络数据读取，用户只需关注自己的数据
3. 网络处理和存储分离，用户可以提供自己的存储模块
4. 多线程网络处理，可以达到较高的网络信息

# 安装使用
1. cd csutil/demo/server && make && ./server
2. cd csutil/demo/client && make && ./client

# 后续
1. 提供基于内存和磁盘的存储机制
2. 提供分布式控制
