#include "CSocket.h"
#include <string>
#include "EdoyunThread.h"
#include <map>

class RTSPRequest
{
public:
    RTSPRequest() {}
    ~RTSPRequest() {}
    //赋值构造与拷贝构造函数
    RTSPRequest(const RTSPRequest& protocol);
    RTSPRequest& operator=(const RTSPRequest& protocol);
private:
    int m_method;//0 option 1 describe 2 setup 3 play 4 pause 5 teardown
};

class RTSPReply
{
public:
    RTSPReply() {}
    ~RTSPReply() {}
    //赋值构造与拷贝构造函数
    RTSPReply(const RTSPReply& protocol);
    RTSPReply& operator=(const RTSPReply& protocol);
private:
    int m_method;//0 option 1 describe 2 setup 3 play 4 pause 5 teardown
};

class RTSPSession
{
public:
    RTSPSession() {}
    ~RTSPSession() {}
    //赋值构造与拷贝构造函数
    RTSPSession(const RTSPSession& protocol);
    RTSPSession& operator=(const RTSPSession& protocol);
};

class RTSPServer: public ThreadFuncBase
{
public:
    RTSPServer(): m_socket(true), m_status(0) {}
    ~RTSPServer();

    //初始化服务器
    bool Init(const std::string& ip = "0.0.0.0", int port = 554);
    //启动服务器
    int Ivoke();
    //关闭服务器
    void Stop();

protected:
    int ThreadWorker();
    RTSPRequest AnlysisRequest(const std::string& request);
    RTSPReply Response(const RTSPRequest& protocol);
    int ThreadSession();

private:
    ESocket m_socket;
    int m_status;//0:未初始化, 1:初始化完成, 2:正在运行, 3:关闭

    EdoyunThread m_threadMain;
    EdoyunThreadPool m_threadPool;
    std::map<std::string, RTSPSession> m_mapSession;
    static SocketIniter initer;
};
