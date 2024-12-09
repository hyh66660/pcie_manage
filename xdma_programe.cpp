﻿#include "xdma_programe.h"

xdma_programe::xdma_programe()
    :QThread()
{
    //清0句柄
    memset(devReadHandle, 0, sizeof(devReadHandle));
    memset(devWriteHandle, 0, sizeof(devWriteHandle));
    pReadyData = NULL;
}

xdma_programe::~xdma_programe()
{
    for(int i=0;i<4;i++)
    {
        if(devReadHandle[i] != INVALID_HANDLE_VALUE)
        {
            CloseHandle(devReadHandle[i]);
        }
    }
    for(int i=0;i<3;i++)
    {
        if(devWriteHandle[i] != INVALID_HANDLE_VALUE)
        {
            CloseHandle(devWriteHandle[i]);
        }
    }
}

//从指定地址读出指定长度的数据,存入pData中,返回值指示实际的传输量
//hand      操作句柄
//pData:    数据缓存
//len:      传输长度
//offset:   操作地址
//return size 实际传输量
int xdma_programe::read_pack(HANDLE hand, char *pData, int len, unsigned int offset)
{
    DWORD size;
    LARGE_INTEGER addr;

    memset(&addr, 0, sizeof(addr));
    addr.QuadPart = offset;

    if ((int)INVALID_SET_FILE_POINTER == SetFilePointerEx(hand, addr, NULL, FILE_BEGIN))
    {
        return 0;
    }
    if (!ReadFile(hand, pData, len, &size, NULL))
    {
        return 0;
    }
    return (int)size;
}

//从指定地址写入指定长度的数据,数据暂存于pData中,返回值指示实际的传输量
//hand      操作句柄
//pData:    数据缓存
//len:      传输长度
//offset:   操作地址
//return size 实际传输量
int xdma_programe::write_pack(HANDLE hand, char *pData, int len, unsigned int offset)
{
    DWORD size;
    LARGE_INTEGER addr;

    memset(&addr, 0, sizeof(addr));
    addr.QuadPart = offset;

    //指定地址
    if ((int)INVALID_SET_FILE_POINTER == SetFilePointerEx(hand, addr, NULL, FILE_BEGIN))
    {
        return 0;
    }
    //写入操作
    if (!WriteFile(hand, pData, len, &size, NULL))
    {
        return 0;
    }
    return (int)size;
}

//多线程
void xdma_programe::run()
{
    while(1)
    {
        if(pReadyData)
        {
            //读数据
            if(mode < 3)
            {
                read_pack(devReadHandle[mode], pReadyData, pReadyLen, pReadyOffset);
                emit opt_end(0);
            }
            //写数据
            else if(mode >= 3 && mode < 6)
            {
                write_pack(devWriteHandle[mode-3], pReadyData, pReadyLen, pReadyOffset);
                emit opt_end(1);
            }else {  // 使用 bypass 句柄读数据
                read_pack(devReadHandle[3], pReadyData, pReadyLen, pReadyOffset);
                emit opt_end(0);
            }
            pReadyData = NULL;
        }
        usleep(10000);
    }
}

void xdma_programe::opt_pack(int mode, char *pData, int len, unsigned int offset)
{
    this->mode = mode;              //读模式(0,1,2,6) 写模式(3,4,5)
    this->pReadyLen = len;          //读写长度
    this->pReadyOffset = offset;    //读写地址
    this->pReadyData = pData;       //读写buffer
}


bool xdma_programe::getDevice()
{
    bool b = false;
    HDEVINFO device_info;
    GUID guid = GUID_DEVINTERFACE_XDMA;
    SP_DEVICE_INTERFACE_DATA device_interface;
    DWORD index;
    int len;
    char *xdma_path;

    //复位已连接的设备句柄
    for(int i=0;i<4;i++)
    {
        if(devReadHandle[i] != INVALID_HANDLE_VALUE)
        {
            CloseHandle(devReadHandle[i]);
        }
    }
    for(int i=0;i<3;i++)
    {
        if(devWriteHandle[i] != INVALID_HANDLE_VALUE)
        {
            CloseHandle(devWriteHandle[i]);
        }
    }
    memset(devReadHandle, 0, sizeof(devReadHandle));
    memset(devWriteHandle, 0, sizeof(devWriteHandle));

    //获取设备路径
    device_info = SetupDiGetClassDevs((LPGUID)&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (device_info != INVALID_HANDLE_VALUE)
    {
        device_interface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        for (index = 0; SetupDiEnumDeviceInterfaces(device_info, NULL, &guid, index, &device_interface); ++index)
        {
            // get required buffer size
            ULONG detailLength = 0;
            if (!SetupDiGetDeviceInterfaceDetail(device_info, &device_interface, NULL, 0, &detailLength, NULL) && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
            {
                break;
            }

            // allocate space for device interface detail
            PSP_DEVICE_INTERFACE_DETAIL_DATA dev_detail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, detailLength);
            if (!dev_detail)
            {
                break;
            }
            dev_detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

            // get device interface detail
            if (!SetupDiGetDeviceInterfaceDetail(device_info, &device_interface, dev_detail, detailLength, NULL, NULL))
            {
                HeapFree(GetProcessHeap(), 0, dev_detail);
                break;
            }

            len = wcslen( dev_detail->DevicePath);
            xdma_path = (char *)malloc((len+40)*2);
            if(xdma_path)
            {
                memcpy(xdma_path, dev_detail->DevicePath, len*2);


                //创建control句柄
                memcpy(xdma_path+len*2, XDMA_FILE_CONTROL, sizeof(XDMA_FILE_CONTROL));
                devReadHandle[0] = CreateFile((LPCWSTR)xdma_path, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                devWriteHandle[0] = devReadHandle[0];

                //创建用户句柄
                memcpy(xdma_path+len*2, XDMA_FILE_USER, sizeof(XDMA_FILE_USER));
                devReadHandle[1] = CreateFile((LPCWSTR)xdma_path, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                devWriteHandle[1] = devReadHandle[1];


                //创建内存读句柄
                memcpy(xdma_path+len*2, XDMA_FILE_C2H_0, sizeof(XDMA_FILE_C2H_0));
                devReadHandle[2] = CreateFile((LPCWSTR)xdma_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                //创建内存写句柄
                memcpy(xdma_path+len*2, XDMA_FILE_H2C_0, sizeof(XDMA_FILE_H2C_0));
                devWriteHandle[2] = CreateFile((LPCWSTR)xdma_path, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

                // 创建 bypass 句柄
                memcpy(xdma_path+len*2, XDMA_FILE_BYPASS, sizeof(XDMA_FILE_BYPASS));
                devReadHandle[3] = CreateFile((LPCWSTR)xdma_path, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

                free(xdma_path);
                b = true;

                COMMTIMEOUTS CommTimeouts;
                CommTimeouts.ReadIntervalTimeout = 1000;
                CommTimeouts.ReadTotalTimeoutMultiplier = 1000;
                CommTimeouts.ReadTotalTimeoutConstant = 1000;
                CommTimeouts.WriteTotalTimeoutMultiplier = 1000;
                CommTimeouts.WriteTotalTimeoutConstant = 1000;
                SetCommTimeouts(devReadHandle[2], &CommTimeouts);
                SetCommTimeouts(devWriteHandle[2], &CommTimeouts);
            }
            HeapFree(GetProcessHeap(), 0, dev_detail);
            break;
        }
        SetupDiDestroyDeviceInfoList(device_info);
    }
    return b;
}


