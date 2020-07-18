#include "Open3DTcpThread.h"

O3DS_TcpThread::O3DS_TcpThread(int port)
: m_Port(port)
, TcpServerSocket(nullptr)
, TcpSource(nullptr)
, m_Kill(false)
, m_Pause(false)
{
	m_semaphore = FGenericPlatformProcess::GetSynchEventFromPool(false);
	Thread = FRunnableThread::Create(this, TEXT("O3DS_TcpThread"), 0, TPri_BelowNormal);
}

O3DS_TcpThread::~O3DS_TcpThread()
{
	if (m_semaphore)
	{
		//Cleanup the FEvent
		FGenericPlatformProcess::ReturnSynchEventToPool(m_semaphore);
		m_semaphore = nullptr;
	}

	if (Thread)
	{
		//Cleanup the worker thread
		delete Thread;
		Thread = nullptr;
	}
}

bool O3DS_TcpThread::Init()
{
	//Init the Data 

	TcpServerSocket = FTcpSocketBuilder(TEXT("Open3DStreamServer"))
		.AsNonBlocking()
		.AsReusable()
		.BoundToPort(m_Port)
		.Listening(1)
		.Build();

	return true;
}


uint32 O3DS_TcpThread::Run()
{
	//Initial wait before starting

	FPlatformProcess::Sleep(0.03);

	while (!m_Kill)
	{
		if (m_Pause)
		{
			//FEvent->Wait(); will "sleep" the thread until it will get a signal "Trigger()"
			m_semaphore->Wait();

			if (m_Kill)
			{
				return 0;
			}
		}
		else
		{
			if (TcpSource == nullptr)
			{
				TcpSource = TcpServerSocket->Accept("Open3DStreamSource");
			}

			if (TcpSource)
			{

				if (TcpSource->Wait(ESocketWaitConditions::WaitForReadOrWrite, FTimespan::FromSeconds(1)))
				{
					m_mutex.Lock();
					DataDelegate.Execute(TcpSource);
					m_mutex.Unlock();
				}
			}
		}
	}

	return 0;
}

void O3DS_TcpThread::PauseThread()
{
	m_Pause = true;
}

void O3DS_TcpThread::ContinueThread()
{
	m_Pause = false;

	if (m_semaphore)
	{
		//Here is a FEvent signal "Trigger()" -> it will wake up the thread.
		m_semaphore->Trigger();
	}
}

void O3DS_TcpThread::Stop()
{
	m_Kill = true; //Thread kill condition "while (!m_Kill){...}"
	m_Pause = false;

	if (m_semaphore)
	{
		//We shall signal "Trigger" the FEvent (in case the Thread is sleeping it shall wake up!!)
		m_semaphore->Trigger();
	}
}

//Use this method to kill the thread!!
void O3DS_TcpThread::EnsureCompletion()
{
	Stop();

	if (Thread)
	{
		Thread->WaitForCompletion();
	}
}

bool O3DS_TcpThread::IsThreadPaused()
{
	return (bool)m_Pause;
}