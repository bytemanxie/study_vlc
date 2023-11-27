#pragma once
#include "base.h"

class RTPHeader
{
public:
    
    unsigned short extension : 1;
    unsigned short csrcCount : 4;
    unsigned short padding : 1;
    unsigned short version : 2;
    unsigned short payloadType : 7;
    unsigned short marker : 1;
    unsigned short serial;
    unsigned timestamp;
    unsigned ssrc;
    unsigned csrc[15];

public:
    RTPHeader()
    {
        memset(this, 0, sizeof(RTPHeader));
    }
    ~RTPHeader()
    {

    }

    operator EBuffer()
    {
        return EBuffer(this, sizeof(RTPHeader));
    }

};

class RTPHelper
{
public:
    RTPHelper(){}
    ~RTPHelper(){}
private:
    EBuffer m_head;
    EBuffer m_payload;
};

