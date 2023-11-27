#pragma once
#include <string>
#include <WinSock2.h>
class EBuffer :public std::string
{
public:
    EBuffer(const char* str)
    {
        resize(strlen(str));
        memcpy((void*)c_str(), str, size());
    }

    EBuffer(size_t size = 0) : std::string()
    {
        if (size > 0)
        {
            resize(size);
            memset(*this, 0, this->size());
        }

    }

    EBuffer(void* buffer, size_t size) : std::string()
    {
        resize(size);
        memcpy((void*)c_str(), buffer, size);
    }

    // //���ں����أ�ʹEBuffer����ֱ�Ӹ�ֵΪstd::string
    // EBuffer& operator=(const std::string& str)
    // {
    //     std::string::operator=(str);
    //     return *this;
    // }

    //����==�������ʹEBuffer����ֱ����const char*�Ƚ�
    bool operator==(const char* str) const
    {
        return strcmp(c_str(), str) == 0;
    }

    operator std::string()
    {
        return *this;
    }

    ~EBuffer()
    {
        std::string::~basic_string();
    }

    operator char* ()
    {
        return (char*)c_str();
    }

    operator const char* () const
    {
        return c_str();
    }

    operator BYTE* () const
    {
        return (BYTE*)c_str();
    }

    operator void* () const
    {
        return (void*)c_str();
    }

    //����EBufferת��Ϊchar*
    operator char* () const
    {
        return (char*)c_str();
    }


    void Update(void* buffer, size_t size)
    {
        resize(size);
        memcpy((void*)c_str(), buffer, size);
    }

    void Zero()
    {
        memset(*this, 0, size());

    }

    EBuffer& operator << (const EBuffer& buffer)
    {
        append(buffer);
        return *this;
    }

    EBuffer& operator << (const std::string& buffer)
    {
        append(buffer);
        return *this;
    }

    EBuffer& operator << (const char* buffer)
    {
        append(buffer);
        return *this;
    }

    EBuffer& operator << (const int& buffer)
    {
        char str[16] = { 0 };
        sprintf(str, "%d", buffer);
        append(str);
        return *this;
    }

    //����>>�������ʹEBuffer�����short
    const EBuffer& operator>>(short& value) const
    {
        value = (short)atoi(c_str());
        return *this;
    }

    //����>>�������ʹEBuffer�����int
    const EBuffer& operator>>(int& value) const
    {
        value = atoi(c_str());
        return *this;
    }
};