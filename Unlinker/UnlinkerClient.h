#pragma once
#include <Windows.h>
#include <QObject>

class UnlinkerClient  : public QObject
{
	Q_OBJECT

public:
	UnlinkerClient(QObject *parent);
	~UnlinkerClient();

	bool hide(int pid);
	bool isDriverAvailable() const;

private:
	HANDLE m_diver = NULL;
	bool m_isDriverAvailable = true;
};
