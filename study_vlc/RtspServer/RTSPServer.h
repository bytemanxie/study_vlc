#include "CSocket.h"
#include "CEdoyunQueue.h"
#include "EdoyunThread.h"
#include <string>
#include <map>


class RTSPRequest
{
public:
    RTSPRequest();
    ~RTSPRequest();
    //赋值构造与拷贝构造函数
    RTSPRequest(const RTSPRequest& protocol);
    RTSPRequest& operator=(const RTSPRequest& protocol);

    void SetMethod(const EBuffer& method);
    void SetUrl(const EBuffer& url);
    void SetSequence(const EBuffer& seq);
    void SetClientPort(int ports[]);
    void SetSession(const EBuffer& session);
    const int method()const {return m_method;};
    const EBuffer& url()const {return m_url;};
    const EBuffer& session()const {return m_session;};
    const EBuffer& sequence()const {return m_seq;};

    const EBuffer& client_port(int index = 0)const {return m_client_port[index];};
    
private:
    int m_method;//0 option 1 describe 2 setup 3 play 4 teardown
    EBuffer m_url;
    EBuffer m_session;
    EBuffer m_seq;
    EBuffer m_client_port[2];
};

class RTSPReply
{
public:
    RTSPReply();
    ~RTSPReply(){};
    //赋值构造与拷贝构造函数
    RTSPReply(const RTSPReply& protocol);
    RTSPReply& operator=(const RTSPReply& protocol);

    EBuffer tobuffer();
    void SetOptions(const EBuffer& options);
    void SetSequence(const EBuffer& seq);
    void SetSdp(const EBuffer& sdp);
    void SetClientPort(const EBuffer& port0, const EBuffer& port1);
    void SetServerPort(const EBuffer& port0, const EBuffer& port1);
    void SetSession(const EBuffer& session);
    void SetMethod(int method);
    

private:
    int m_method;//0 option 1 describe 2 setup 3 play 4 teardown
    
    int m_client_port[2];
    int m_server_port[2];
    EBuffer m_sdp;
    EBuffer m_options;
    EBuffer m_session;
    EBuffer m_seq;
};

class RTSPSession
{
public:
    RTSPSession();
    ~RTSPSession() {}
    //赋值构造与拷贝构造函数
    RTSPSession(const ESocket& socket);
    RTSPSession(const RTSPSession& protocol);
    RTSPSession& operator=(const RTSPSession& protocol);

    // RTSPRequest AnlysisRequest(const std::string& request);
    // RTSPReply Response(const RTSPRequest& protocol);
    int PickRequestAndResponse();
    EBuffer Pick();

    RTSPRequest AnalysisRequest(const EBuffer& request);
    RTSPReply Response(const RTSPRequest& protocol); 
    EBuffer PickOneLine(EBuffer& data);

private:
    ESocket m_client;
    std::string m_id;

};

class RTSPServer: public ThreadFuncBase
{
public:
    RTSPServer(): m_socket(true), m_status(0), m_threadPool(10) {
        m_threadMain.UpdateWorker(ThreadWorker(this, (FUNCTYPE)&RTSPServer::threadworker));
    }
    
    ~RTSPServer();

    //初始化服务器
    bool Init(const std::string& ip = "0.0.0.0", int port = 554);
    //启动服务器
    int Ivoke();
    //关闭服务器
    void Stop();

protected:
    //返回0继续，返回负数终止，返回其他警告
    int threadworker();
    
    int ThreadSession();

private:
    static SocketIniter m_initer;
    ESocket m_socket;
    EAddress m_address;

    int m_status;//0:未初始化, 1:初始化完成, 2:正在运行, 3:关闭

    EdoyunThread m_threadMain;
    EdoyunThreadPool m_threadPool;

    CEdoyunQueue<RTSPSession> m_lstSession;
};
