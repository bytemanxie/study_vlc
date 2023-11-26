#include "RTSPServer.h"
#include <rpc.h>
#pragma comment(lib, "rpcrt4.lib")
#pragma comment(lib, "ws2_32.lib")

SocketIniter RTSPServer::m_initer;

RTSPServer::~RTSPServer()
{
    Stop();
} 

bool RTSPServer::Init(const std::string& ip, int port)
{
    m_address.Update(ip, port);
    m_socket.Bind(m_address);
    m_socket.Listen(5);
    return false;
}

int RTSPServer::Ivoke()
{
    m_threadMain.Start();
    m_threadPool.Invoke();
    return 0;
}

void RTSPServer::Stop()
{
    m_socket.Close();
    m_threadMain.Stop();
    m_threadPool.Stop();
}

int RTSPServer::threadworker()
{
    EAddress client_addr;
    ESocket client_socket = m_socket.Accept(client_addr);

    if(client_socket != INVALID_SOCKET)
    {
        RTSPSession session(client_socket);
        m_lstSession.PushBack(session);
        m_threadPool.DispatchWorker(ThreadWorker(this, (FUNCTYPE)&RTSPServer::ThreadSession));
    }
    else
    { 
        return -1;
    }
    return 0;
}

int RTSPServer::ThreadSession()
{
    RTSPSession session;
    if(m_lstSession.PopFront(session))
    {
        return session.PickRequestAndResponse();
    }

    return -1;
}

RTSPSession::RTSPSession()
{
    UUID uuid;
    UuidCreate(&uuid);
    RPC_CSTR str;
    UuidToStringA(&uuid, &str);
    m_id = (char*)str;
    RpcStringFreeA(&str);
}

RTSPSession::RTSPSession(const ESocket & socket)
: m_client(socket)
{
    //生成一个id
    UUID uuid;
    UuidCreate(&uuid);
    RPC_CSTR str;
    UuidToStringA(&uuid, &str);
    m_id = (char*)str;
    RpcStringFreeA(&str);

}

RTSPSession::RTSPSession(const RTSPSession & protocol)
{
    m_client = protocol.m_client;
    m_id = protocol.m_id;
}

RTSPSession & RTSPSession::operator=(const RTSPSession & protocol)
{
    if (this == &protocol)
    {
        return *this;
    }
    m_client = protocol.m_client;
    m_id = protocol.m_id;
    return *this;
}

int RTSPSession::PickRequestAndResponse()
{
    int ret = -1;
    do{
        EBuffer buffer = Pick();
        if(buffer.size() <= 0)
        {
            return -1;
        }
        RTSPRequest request = AnalysisRequest(buffer);

        if (request.method() < 0)
        {
            TRACE("request error %s\n", buffer.c_str());
            return -1;
        }

        RTSPReply reply = Response(request);
        int ret = m_client.Send(reply.tobuffer());
        
    } while (ret >= 0);
    
    if (ret < 0)
    {
        return -1;
    }
    return 0;
}

EBuffer RTSPSession::Pick()
{
    EBuffer result;
    int ret = 1;
    while (/* condition */ ret > 0)
    {
        /* code */
        EBuffer buffer(1024);
        buffer.Zero();
        ret = m_client.Recv(buffer);
        int i = GetLastError();
        if(ret > 0)
        {
            buffer.resize(ret);
            result += buffer;
            //判断result最后四位是否是\r\n\r\n
            if(result.size() >= 4)
            {
                if(result.substr(result.size() - 4, 4) == "\r\n\r\n")
                {
                    break;
                }
            }
        }

    }
    
    return result;
}

inline RTSPRequest RTSPSession::AnalysisRequest(const EBuffer& request)
{
    //打印request
    printf("%s\n", request.c_str());
    EBuffer data = request;
    RTSPRequest result;
    //解析request
    EBuffer line = PickOneLine(data);
    EBuffer method(32), url(256), version(32), seq(64);

    if (sscanf(line.c_str(), "%s %s %s\r\n", method.c_str(), url.c_str(), version.c_str()) < 3)
    {
        //打印错误
        printf("sscanf error request\n");
        return result;
    }

    line = PickOneLine(data);

    if (sscanf(line.c_str(), "CSeq: %s\r\n", seq.c_str()) < 1)
    {
        //打印错误
        printf("sscanf error CSeq\n");
        return result;
    }

    result.SetMethod(method);
    result.SetUrl(url);
    result.SetSequence(seq);

    if ((strcmp(method.c_str(), "OPTIONS") == 0) || (strcmp(method.c_str(), "DESCRIBE") == 0))
    {
        return result;
    }
    else if (strcmp(method.c_str(), "SETUP") == 0)
    {
        line = PickOneLine(data);
        int port[2] = { 0, 0 };
        if (sscanf(line.c_str(), "Transport: RTP/AVP;unicast;client_port=%d-%d\r\n", &port[0], &port[1]) == 2)
        {
            result.SetClientPort(port);
            return result;
        }
        else
        {
            //打印错误
            printf("sscanf error Transport\n");
            return result;
        }
    }
    else if (strcmp(method.c_str(), "PLAY") == 0 || (strcmp(method.c_str(), "TEARDOWN") == 0))
    {
        line = PickOneLine(data);
        EBuffer session(64);
        if (sscanf(line.c_str(), "Session: %s\r\n", session.c_str()) == 1)
        {
            result.SetSession(session);
            return result;
        }
        else
        {
            //打印错误
            printf("sscanf error Session\n");
            return result;
        }
    }
    else if (strcmp(method.c_str(), "PAUSE") == 0)
    {

    }
    else
    {
        //打印错误
        printf("method error\n");
        return result;
    }

    return result;
}

RTSPReply RTSPSession::Response(const RTSPRequest & request)
{
    RTSPReply result;
    //生成reply
    result.SetSequence(request.sequence());
    if(request.session().size() > 0)
    {
        result.SetSession(request.session());
    }
    else
    {
        result.SetSession(m_id.c_str());
    }

    result.SetMethod(request.method());

    switch (request.method())
    {
    case 0:
        result.SetOptions("Public: OPTIONS, DESCRIBE, SETUP, PLAY, TEARDOWN\r\n");
		break;
    case 1:
    {
        EBuffer sdp;
        sdp << "v=0\r\n";
        sdp << "o=- " << m_id <<  "1 IN IP4 127.0.0.1\r\n";
        sdp << "a = range:96 H264/90000\r\n" << "a = control:track0\r\n";
        result.SetSdp(sdp);
    }
        break;
    case 2:
        result.SetClientPort(request.client_port(0), request.client_port(1));
        result.SetServerPort("55000", "55001");
        result.SetSession(m_id.c_str());
        break;
    case 3:
        break;
    case 4:
        break;
    default:
        break;
    }
    return result;
}

EBuffer RTSPSession::PickOneLine(EBuffer & data)
{
    EBuffer result;
    int index = data.find("\r\n");
    if(index >= 0)
    {
        result = data.substr(0, index + 2).c_str();
        data = data.substr(index + 2).c_str();
    }
    return result; // Fixed: Return the 'result' variable instead of an empty EBuffer
}


RTSPRequest::RTSPRequest()
{
    m_method = -1;
    m_url = ""; // Fixed: Initialize m_url with an empty string
    m_session = ""; // Fixed: Initialize m_session with an empty string
}

RTSPRequest::~RTSPRequest()
{
    m_method = -1;
}

RTSPRequest::RTSPRequest(const RTSPRequest & protocol)
{
    m_method = protocol.m_method;
    m_url = protocol.m_url;
    m_session = protocol.m_session;
    m_seq = protocol.m_seq;
    m_client_port[0] = protocol.m_client_port[0];
    m_client_port[1] = protocol.m_client_port[1];
}

RTSPRequest & RTSPRequest::operator=(const RTSPRequest & protocol)
{
    if (this == &protocol)
    {
        return *this;
    }
    m_method = protocol.m_method;
    m_url = protocol.m_url;
    m_session = protocol.m_session;
    m_seq = protocol.m_seq;
    m_client_port[0] = protocol.m_client_port[0];
    m_client_port[1] = protocol.m_client_port[1];
    return *this;
}

void RTSPRequest::SetMethod(const EBuffer & method)
{
    if(method == "OPTIONS")
    {
        m_method = 0;
    }
    else if(method == "DESCRIBE")
    {
        m_method = 1;
    }
    else if(method == "SETUP")
    {
        m_method = 2;
    }
    else if(method == "PLAY")
    {
        m_method = 3;
    }
    else if(method == "TEARDOWN")
    {
        m_method = 4;
    }
    else
    {
        m_method = -1;
    }
}

void RTSPRequest::SetUrl(const EBuffer & url)
{
    m_url = (char*)url;
}

void RTSPRequest::SetSequence(const EBuffer & seq)
{
    m_seq = (char*)seq;
}

void RTSPRequest::SetClientPort(int ports[])
{
    m_client_port[0] << ports[0];
    m_client_port[1] << ports[1];
}
 
void RTSPRequest::SetSession(const EBuffer & session)
{
    m_session = (char*)session;
}

RTSPReply::RTSPReply()
{
    m_method = -1;
}

RTSPReply::RTSPReply(const RTSPReply & protocol)
{
    m_method = protocol.m_method;
    m_sdp = protocol.m_sdp;
    m_options = protocol.m_options;
    m_session = protocol.m_session;
    m_client_port[0] = protocol.m_client_port[0];
    m_client_port[1] = protocol.m_client_port[1];
    m_server_port[0] = protocol.m_server_port[0];
    m_server_port[1] = protocol.m_server_port[1];
}

RTSPReply & RTSPReply::operator=(const RTSPReply & protocol)
{
// TODO: 在此处插入 return 语句
    if (this == &protocol)
    {
        return *this;
    }
    m_method = protocol.m_method;
    m_sdp = protocol.m_sdp;
    m_options = protocol.m_options;
    m_session = protocol.m_session;
    m_client_port[0] = protocol.m_client_port[0];
    m_client_port[1] = protocol.m_client_port[1];
    m_server_port[0] = protocol.m_server_port[0];
    m_server_port[1] = protocol.m_server_port[1];
    return *this;
}

EBuffer RTSPReply::tobuffer()
{
    EBuffer result;
    result << "RTSP/1.0 200 OK\r\n";
    result << "CSeq: " << m_seq << "\r\n";
    switch (m_method)
    {
        
    case 0: //OPTIONS
        result << "Public: OPTIONS, DESCRIBE, SETUP, PLAY, TEARDOWN\r\n";
        break;
    case 1: // DESCRIBE
        result << "Content-Base: 127.0.0.1 \r\n";
        result << "Content-Type: application/sdp\r\n";
        
        result << "Content-Length: " << m_sdp.size() << "\r\n";
        result << "\r\n";
        result << m_sdp;
        break;
    case 2://SETUP
       
        result << "Transport: RTP/AVP;unicast;client_port=" << m_client_port[0] << "-" << m_client_port[1] << ";server_port=" << m_server_port[0] << "-" << m_server_port[1] << ";ssrc=00000000\r\n";
        result << "Session: " << m_session << "\r\n";
        result << "\r\n";
        break;
    case 3://PLAY
        result << "Range : npt = 0.000-\r\n";
        result << "Session: " << m_session << "\r\n";
        result << "\r\n";
        break;
    case 4: //TEARDOWN 
        result << "Session: " << m_session << "\r\n";
        result << "\r\n";
        break;
    default:
        break;
    }
    return result;
}

void RTSPReply::SetOptions(const EBuffer & options)
{
    m_options = options;
}
void RTSPReply::SetSequence(const EBuffer & seq)
{
    m_seq = seq;
}
 

void RTSPReply::SetSdp(const EBuffer & sdp)
{
    m_sdp = sdp;
}

void RTSPReply::SetClientPort(const EBuffer & port0, const EBuffer & port1)
{
    port0 >> m_client_port[0];
    port1 >> m_client_port[1];
}

void RTSPReply::SetServerPort(const EBuffer & port0, const EBuffer & port1)
{
    port0 >> m_server_port[0];
    port1 >> m_server_port[1];
}

void RTSPReply::SetSession(const EBuffer & session)
{
    m_session = session;
}

void RTSPReply::SetMethod(int method)
{
    m_method = method;
}


