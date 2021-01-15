# NS3模拟区域智能体开机组网

## 1.正常情况下的开机组网

### 程序描述

1个控制中心，3个终端之间的开机组网过程。

### 配置环境

Linux Ubuntu 20.04+NS3 3.32+eclipse+netanim 3.108

### 各层协议

物理层：缺省

数据链路层：CSMA

网络层：IP协议

传输层：UDP

应用层：OnOffHelper（NS3自带的一个发送数据包的应用）

PacketSinkHelper（NS3自带的一个接收数据包的应用）

### 通信效果

<video id="video" controls="" preload="none">
    <source id="mp4" src="C:/Users/fengyuting/Desktop/normal.mp4" type="video/mp4">
</video>

### 代码问题与反思

1.两个节点之间通信交互次数总是出现问题；待优化；

2.目前基于自带的应用程序比较死板，只是模拟了他们之间的通信过程，没有涉及到智能，比如是否知道终端IP等等的。待优化；

3.传输数据的数据包还只是数据流，没有涉及数据内容，待优化。
