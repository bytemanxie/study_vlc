#include "CSocket.h"
#include "CEdoyunQueue.h"
#include "EdoyunThread.h"
#include <string>
#include <map>
#include "RTPHelper.h"
#include "MediaFile.h"

class RTSPRequest {
public:
	RTSPRequest();
	RTSPRequest(const RTSPRequest& protocol);
	RTSPRequest& operator=(const RTSPRequest& protocol);
	~RTSPRequest() { m_method = -1; }
	void SetMethod(const EBuffer& method);
	void SetUrl(const EBuffer& url);
	void SetSequence(const EBuffer& seq);
	void SetClientPort(int ports[]);
	void SetSession(const EBuffer& session);
	int method() const { return m_method; }
	const EBuffer& url() const { return m_url; }
	const EBuffer& session() const { return m_session; }
	const EBuffer& sequence() const { return m_seq; }
	const EBuffer& port(int index = 0) const { return index ? m_client_port[1] : m_client_port[0]; }
private:
	int m_method;//-1初始化 0 OPTIONS 1 DESCRIBE 2 SETUP 3 PLAY 4 TEARDOWN
	EBuffer m_url;
	EBuffer m_session;
	EBuffer m_seq;
	EBuffer m_client_port[2];
};

class RTSPReply {
public:
	RTSPReply();
	RTSPReply(const RTSPReply& protocol);
	RTSPReply& operator=(const RTSPReply& protocol);
	~RTSPReply() {}
	EBuffer toBuffer();
	void SetMethod(int method);
	void SetOptions(const EBuffer& options);
	void SetSequence(const EBuffer& seq);
	void SetSdp(const EBuffer& sdp);
	void SetClientPort(const EBuffer& port0, const EBuffer& port1);
	void SetServerPort(const EBuffer& port0, const EBuffer& port1);
	void SetSession(const EBuffer& session);
private:
	int m_method;//0 OPTIONS 1 DESCRIBE 2 SETUP 3 PLAY 4 TEARDOWN
	int m_client_port[2];
	int m_server_port[2];
	EBuffer m_sdp;
	EBuffer m_options;
	EBuffer m_session;
	EBuffer m_seq;
};

class RTSPSession;
class RTSPServer;
typedef void (*RTSPPLAYCB)(RTSPServer* thiz, RTSPSession& session);

class RTSPSession
{
public:
	RTSPSession();
	RTSPSession(const ESocket& client);
	RTSPSession(const RTSPSession& session);
	RTSPSession& operator=(const RTSPSession& session);
	~RTSPSession() {}
	int PickRequestAndReply(RTSPPLAYCB cb, RTSPServer* thiz);
	EAddress GetClientUDPAddress()const;
private:
	EBuffer PickOneLine(EBuffer& buffer);
	EBuffer Pick();
	RTSPRequest AnalyseRequest(const EBuffer& buffer);
	RTSPReply Reply(const RTSPRequest& request);

private:
	EBuffer m_id;
	ESocket m_client;
	short m_port;
};



class RTSPServer :public ThreadFuncBase
{
public:
	RTSPServer()
		:m_socket(true), m_status(0), m_pool(10)
	{
		m_threadMain.UpdateWorker(ThreadWorker(this, (FUNCTYPE)&RTSPServer::threadWorker));
		m_h264.Open("./test.h264");
	}
	int Init(const std::string& strIP = "0.0.0.0", short port = 554);
	int Invoke();
	void Stop();
	~RTSPServer();
protected:
	//返回0继续，返回负数终止，返回其他警告
	int threadWorker();
	int ThreadSession();
	static void PlayCallBack(RTSPServer* thiz, RTSPSession& session);
	void UdpWorker(const EAddress& client);
private:
	static SocketIniter m_initer;
	ESocket m_socket;
	EAddress m_addr;
	int m_status;//0 未初始化 1 初始化完成 2 正在运行 3 关闭 
	EdoyunThread m_threadMain;
	EdoyunThreadPool m_pool;
	CEdoyunQueue<RTSPSession> m_lstSession;
	RTPHelper m_helper;
	MediaFile m_h264;
};
