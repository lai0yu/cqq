# Linuxc网络应用层项目--类QQ

组员：赖林宇 罗炽龙 梁锦涛 粟磊

## 数据交流结构体

用json 进行打包解包

1. 通用数据

```c
struct data{
  int code,
  char msg[256];
  char data[1024];
}
```

客户端和服务器都要实现拆包解包

code 表示状态，msg表示拆包动作，如何拆包data

2.用户数据包  

```c
struct user_data{
  char username[128];
  char password[128];
}
```

函数原型

解包函数原型：

```c
struct data unpack_json(char* json_str)
```  

打包函数原型:

```c
char* pack_json(void* data)
```

## 客户端

### 登录界面

```c
void login_ui(void)
```

#### 1. 登录功能

```cc
int login(const char* username, const char* password)
```

#### 3.注册功能

```c
int register(const char* username, const char* password)
```

#### 4. 退出功能

直接调用exit

### 主界面

```c
void main_ui(void)
```

#### 1. 在线功能

开一个线程定时发送心跳包，间隔时间为180s,客户端发送`ping` 接收`pong` 不使用通用格式数据
发送`ping`后3s内没有接收到`pong`，断开服务器并退出

## 服务器

#### 1.数据库

##### 1. user表


#### 消息

```
{"code":0,"msg":"sign_up","data:{ "username":lai0yu,"password":"123456"}"}
```

##### 2
