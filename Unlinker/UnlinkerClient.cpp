#include "UnlinkerClient.h"
#include "..\UnlinkerDrv\IOCTLs.h"
#include "..\UnlinkerDrv\Common.h"
#include <QDebug>

UnlinkerClient::UnlinkerClient(QObject *parent)
	: QObject(parent)
{
    HANDLE hDriver = CreateFile(
        L"\\\\.\\Unlinker",
        GENERIC_WRITE,
        FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr);

    if (hDriver == INVALID_HANDLE_VALUE)
    {
        qDebug() << "UnlinkerClient::Cannot open handler for driver";
        this->m_isDriverAvailable = false;
        return;
    }

    this->m_diver = hDriver;
}

UnlinkerClient::~UnlinkerClient()
{
    CloseHandle(this->m_diver);
}

bool UnlinkerClient::hide(int pid)
{
    InputParameters input;
    input.pid = pid;

    qDebug() << "UnlinkerClient::target pid: " << input.pid;

    BOOL success = DeviceIoControl(
        this->m_diver,
        UNLINKER_HIDDEN_PROCESS,
        &input,          // pointer to the data
        sizeof(input),   // the size of the data
        nullptr,
        0,
        nullptr,
        nullptr);

    if (!success)
    {
        return false;
    }

    return true;
}

bool UnlinkerClient::isDriverAvailable() const
{
    return this->m_isDriverAvailable;
}
