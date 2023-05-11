#include "pch.h"
#include "SerialCtrl.h"

SerialCtrl::SerialCtrl(void) :m_portStatus(FALSE), m_portHandle(NULL)
{
	m_portConfig.ByteSize = 8;            // Byte of the Data.
	m_portConfig.StopBits = ONESTOPBIT;   // Use one bit for stopbit.
	m_portConfig.Parity = NOPARITY;       // No parity bit
	m_portConfig.BaudRate = CBR_115200;     // Buadrate 9600 bit/sec
}

SerialCtrl::~SerialCtrl(void)
{
	m_portHandle = NULL;
}

void SerialCtrl::SetPortStatus(BOOL bOnOff)
{
	m_portStatus = bOnOff;
}

BOOL SerialCtrl::GetPortStatus(void)
{
	return m_portStatus;
}

HANDLE SerialCtrl::GetPortHandle(void)
{
	return m_portHandle;
}

BOOL SerialCtrl::OpenPort(DCB dcb, CString portName)
{

	if (m_portStatus == FALSE)  // if port is opened already, not open port again.
	{
		m_portHandle = CreateFile((LPCTSTR)portName,  // Specify port device: default "COM1"
			GENERIC_READ | GENERIC_WRITE,       // Specify mode that open device.
			0,                                  // the divide isn't shared.
			NULL,                               // the object gets a default security.
			OPEN_EXISTING,                      // Specify which action to take on file. 
			0,                                  // default.
			NULL);                              // default.
		/*
		if (SetupComm(m_portHandle, 64, 64) != 0)
			printf("Windows In/Out serial buffers changed to 1000 bytes\n");
		else
			printf("Buffers not changed with error %d.\n", GetLastError());										// Get current configuration of serial communication port.

		SecureZeroMemory(&dcb, sizeof(DCB));
		dcb.DCBlength = sizeof(DCB);
		*/
		if (GetCommState(m_portHandle, &m_portConfig) == 0)
		{
			AfxMessageBox((CString)"Get configuration port has problem.");
			return FALSE;
		}
		// Assign user parameter.
		m_portConfig.BaudRate = dcb.BaudRate;    // Specify buad rate of communicaiton.
		m_portConfig.StopBits = dcb.StopBits;    // Specify stopbit of communication.
		m_portConfig.Parity = dcb.Parity;        // Specify parity of communication.
		m_portConfig.ByteSize = dcb.ByteSize;    // Specify  byte of size of communication.

												 // Set current configuration of serial communication port.
		if (SetCommState(m_portHandle, &m_portConfig) == 0)
		{
			AfxMessageBox((CString)"Set configuration port has problem.");
			return FALSE;
		}

		// instance an object of COMMTIMEOUTS.
		COMMTIMEOUTS comTimeOut;
		comTimeOut.ReadIntervalTimeout = 3;
		comTimeOut.ReadTotalTimeoutMultiplier = 3;
		comTimeOut.ReadTotalTimeoutConstant = 2;
		comTimeOut.WriteTotalTimeoutMultiplier = 3;
		comTimeOut.WriteTotalTimeoutConstant = 2;
		SetCommTimeouts(m_portHandle, &comTimeOut);		// set the time-out parameter into device control.
		m_portStatus = TRUE;
		return TRUE;
	}
	return FALSE;
}

BOOL SerialCtrl::OpenPort(CString baudRate, CString portName)
{
	DCB configSerial;
	configSerial.ByteSize = 8;
	configSerial.StopBits = ONESTOPBIT;
	configSerial.Parity = NOPARITY;
	switch (atoi((LPCSTR)(CStringA)baudRate))
	{
	case 110:
		configSerial.BaudRate = CBR_110;
		break;
	case 300:
		configSerial.BaudRate = CBR_300;
		break;
	case 600:
		configSerial.BaudRate = CBR_600;
		break;
	case 1200:
		configSerial.BaudRate = CBR_1200;
		break;
	case 2400:
		configSerial.BaudRate = CBR_2400;
		break;
	case 4800:
		configSerial.BaudRate = CBR_4800;
		break;
	case 9600:
		configSerial.BaudRate = CBR_9600;
		break;
	case 14400:
		configSerial.BaudRate = CBR_14400;
		break;
	case 19200:
		configSerial.BaudRate = CBR_19200;
		break;
	case 38400:
		configSerial.BaudRate = CBR_38400;
		break;
	case 56000:
		configSerial.BaudRate = CBR_56000;
		break;
	case 57600:
		configSerial.BaudRate = CBR_57600;
		break;
	case 115200:
		configSerial.BaudRate = CBR_115200;
		break;
	case 128000:
		configSerial.BaudRate = CBR_128000;
		break;
	case 256000:
		configSerial.BaudRate = CBR_256000;
		break;
	default:
		configSerial.BaudRate = CBR_115200;
		break;
	}

	return OpenPort(configSerial, portName);
}
BOOL SerialCtrl::Read(unsigned char* inputData, const unsigned int& sizeBuffer, unsigned long& length)
{
	if (ReadFile(m_portHandle,  // handle of file to read
		inputData,               // handle of file to read
		sizeBuffer,              // number of bytes to read
		&length,                 // pointer to number of bytes read
		NULL) == 0)              // pointer to structure for data
	{
		// AfxMessageBox("Reading of serial communication has problem.");
		return FALSE;
	}
	if (length > 0)
	{
		inputData[length] = NULL; // Assign end flag of message.
		return TRUE;
	}
	return TRUE;
}

BOOL SerialCtrl::Write(const char* outputData, const unsigned int& sizeBuffer, unsigned long& length)
{
	if (length > 0)
	{
		if (WriteFile(m_portHandle, // handle to file to write to
			outputData,              // pointer to data to write to file
			sizeBuffer,              // number of bytes to write
			&length, NULL) == 0)      // pointer to number of bytes written
		{
			AfxMessageBox((CString)"Reading of serial communication has problem.");
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL SerialCtrl::ClosePort(void)
{
	if (m_portStatus == TRUE)               // Port need to be open before.
	{
		m_portStatus = FALSE;                 // Update status
		if (CloseHandle(m_portHandle) == 0)    // Call this function to close port.
		{
			AfxMessageBox((CString)"Port Closing isn't successes.");
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}




const unsigned short MAX_MESSAGE = 300;

IMPLEMENT_DYNCREATE(SerialThread, CWinThread)
SerialThread::SerialThread() :m_serialIO(NULL)
{

}
SerialThread::~SerialThread()
{
	m_serialIO = NULL;
}


BOOL SerialThread::InitInstance()
{
	return TRUE;
}

int SerialThread::Run()
{
	// Check signal controlling and status to open serial communication.
	while (1)
	{

		while (m_serialIO->GetProcessActivateValue() == TRUE)
		{
			if (m_serialIO->m_SerialCtrl.GetPortStatus() == FALSE && m_serialIO->GetPortActivateValue() == TRUE)
			{

				if (m_serialIO->m_SerialCtrl.OpenPort(m_serialIO->m_DCB, m_serialIO->m_csPortName) == TRUE)
				{
					m_serialIO->OnEventOpen(TRUE);
				}
				else
				{
					m_serialIO->OnEventOpen(FALSE);
					m_serialIO->SetPortActivate(FALSE);
				}

			}
			else if (m_serialIO->GetPortActivateValue() == TRUE)
			{
				unsigned char message[MAX_MESSAGE] = { 0 };
				unsigned int lenBuff = MAX_MESSAGE;
				unsigned long lenMessage;
				if (m_serialIO->m_SerialCtrl.Read(message, lenBuff, lenMessage) == TRUE)
				{
					if (lenMessage > 0)
						m_serialIO->OnEventRead(message, lenMessage);
				}
				else
				{
					//m_serialIO->SetProcessActivate(FALSE);
				}

			}

			if (m_serialIO->GetSendActivateValue() == TRUE)
			{
				unsigned long nWritten;
				if (m_serialIO->m_SerialCtrl.Write(m_serialIO->m_sendBuffer, m_serialIO->m_sendSize, nWritten) == TRUE)
				{
					m_serialIO->OnEventWrite(nWritten);
				}
				else
				{
					m_serialIO->OnEventWrite(-1);
				}
				m_serialIO->SetSendActivate(FALSE);
			}
			if (m_serialIO->m_bClosePort == TRUE)
			{
				if (m_serialIO->m_SerialCtrl.ClosePort() == TRUE)
				{
					m_serialIO->OnEventClose(TRUE);
				}
				else
				{
					m_serialIO->OnEventClose(FALSE);
				}
				m_serialIO->m_bClosePort = FALSE;

			}
		}

	}
	return 0;
}
void SerialThread::ClosePort()
{

	m_serialIO->m_SerialCtrl.ClosePort();

}


CSerialIO::CSerialIO() :m_serialProcess(NULL), m_csPortName("COM3"), m_strBaudRate("115200"), m_sendSize(0)
{
	m_bProccessActivate = FALSE;
	m_bPortActivate = FALSE;
	m_bSendActivate = FALSE;
	m_bClosePort = FALSE;
	Init();

}
CSerialIO:: ~CSerialIO()
{
	UnInit();
}
BOOL CSerialIO::Init()
{
	m_serialProcess = (SerialThread*)AfxBeginThread(RUNTIME_CLASS(SerialThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	m_serialProcess->setOwner(this);

	m_DCB.ByteSize = 8;
	m_DCB.StopBits = ONESTOPBIT;
	m_DCB.Parity = NOPARITY;
	switch (atoi((LPCSTR)(CStringA)m_strBaudRate))
	{
	case 110:
		m_DCB.BaudRate = CBR_110;
		break;
	case 300:
		m_DCB.BaudRate = CBR_300;
		break;
	case 600:
		m_DCB.BaudRate = CBR_600;
		break;
	case 1200:
		m_DCB.BaudRate = CBR_1200;
		break;
	case 2400:
		m_DCB.BaudRate = CBR_2400;
		break;
	case 4800:
		m_DCB.BaudRate = CBR_4800;
		break;
	case 9600:
		m_DCB.BaudRate = CBR_9600;
		break;
	case 14400:
		m_DCB.BaudRate = CBR_14400;
		break;
	case 19200:
		m_DCB.BaudRate = CBR_19200;
		break;
	case 38400:
		m_DCB.BaudRate = CBR_38400;
		break;
	case 56000:
		m_DCB.BaudRate = CBR_56000;
		break;
	case 57600:
		m_DCB.BaudRate = CBR_57600;
		break;
	case 115200:
		m_DCB.BaudRate = CBR_115200;
		break;
	case 128000:
		m_DCB.BaudRate = CBR_128000;
		break;
	case 256000:
		m_DCB.BaudRate = CBR_256000;
		break;
	default:
		break;
	}

	m_DCB.BaudRate = CBR_115200;
	m_bProccessActivate = TRUE;
	m_serialProcess->ResumeThread();
	return TRUE;

}
void CSerialIO::UnInit()
{
	m_bPortActivate = FALSE;
	if (m_serialProcess)
	{
		m_serialProcess->ClosePort();
		m_serialProcess->SuspendThread();
	}
}
void CSerialIO::OpenPort(CString strPortName, CString strBaudRate)
{
	m_strBaudRate = strBaudRate;
	m_csPortName = strPortName;
	m_bPortActivate = TRUE;

}
void CSerialIO::OnEventOpen(BOOL bSuccess)
{
	if (bSuccess)
		printf("port open successfully\n");
	else
		printf("port open failed\n");
}

void CSerialIO::ClosePort()
{
	m_bClosePort = TRUE;
	SetPortActivate(FALSE);
}
void CSerialIO::OnEventClose(BOOL bSuccess)
{
	if (bSuccess)
		printf("port close successfully\n");
	else
		printf("port close failed\n");

}
void CSerialIO::OnEventRead(unsigned char* inPacket, int inLength)
{

	return;
}
void CSerialIO::Write(char* outPacket, int outLength)
{
	if (outLength <= MAX_SEND_BUFFER)
	{
		memcpy(m_sendBuffer, outPacket, outLength);
		m_sendSize = outLength;
		SetSendActivate(TRUE);

	}
	else
	{
		printf("buffer over flow");
		OnEventWrite(-1);
	}
	return;
}

void CSerialIO::OnEventWrite(int nWritten)
{
	if (nWritten < 0)
	{
		printf("write error\n");
	}
	else
	{
		printf("%d bytes send\n", nWritten);
	}
}

BOOL CSerialIO::GetSendActivateValue(void)
{
	return m_bSendActivate;
}

void CSerialIO::SetSendActivate(BOOL bSendActivate)
{
	m_bSendActivate = bSendActivate;
}

BOOL CSerialIO::GetPortActivateValue(void)
{
	return m_bPortActivate;
}

void CSerialIO::SetPortActivate(BOOL bPortActivate)
{
	m_bPortActivate = bPortActivate;
}

BOOL CSerialIO::GetProcessActivateValue(void)
{
	return m_bProccessActivate;
}

void CSerialIO::SetProcessActivate(BOOL bProcessActivate)
{
	m_bProccessActivate = bProcessActivate;
}
