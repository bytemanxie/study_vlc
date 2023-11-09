#pragma once
#include <WinSock2.h>
#include <memory>
// warning(disable:6031)
//#pragma comment(lib, "ws2_32.lib")

class CSocket
{
    public:
    CSocket(bool isTcp = true)
    {
        if (isTcp)
        {
            m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        }
        else
        {
            m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        }
    }

    CSocket(CSocket& s)
    {
        m_socket = s;
    } 

    void Close()
    {
        if(m_socket != INVALID_SOCKET)
        {   
            SOCKET s = m_socket;
            m_socket = INVALID_SOCKET;
            closesocket(s);
        }
    }

    operator SOCKET()
    {
        return m_socket;
    }

    ~CSocket()
    {
        Close();
    }
    private:
        SOCKET m_socket;
};

class ESocket
{
    public:
    ESocket(bool isTcp = true): m_socket(new CSocket(isTcp)) {}

    //拷贝构造函数
    ESocket(const ESocket& s)
    {
        m_socket.reset(new CSocket(*s.m_socket));
    }

    ~ESocket()
    {
        m_socket.reset();
    }

    //赋值运算符
    ESocket& operator=(const ESocket& s)
    {
        if (this == &s)
        {
            return *this;
        }
        m_socket.reset(new CSocket(*s.m_socket));
        return *this;
    }

    operator SOCKET()
    {
        return *m_socket;
    }

    private:
        std::shared_ptr<CSocket> m_socket;
};

class SocketIniter
{
public:
	SocketIniter()
	{
		WSADATA data;
		WSAStartup(MAKEWORD(2, 2), &data);
	}

	~SocketIniter()
	{
		WSACleanup();
	}
};