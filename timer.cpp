/******************************************************************************
* File Name:        timer.cpp
* Description:      
* Notes:            
* Author:           Aseem Tiwari
* Date:             10/02/2021
******************************************************************************/

#include <chrono>

Timer::Timer(uint64_t timeMs, const std::function<void()>& callback, bool autoReset)
{
    m_TimeInterval = timeMs;
    m_AutoReset = autoReset;
    m_Callback = std::cref(callback);
}

Timer::~Timer()
{
    this->Dispose();
}

void Timer::TimerThreadFunc()
{
    do
    {
        if (!m_Active.load())
        {
            return;
        }
        
        std::unique_lock<std::mutex> lock(m_MutexForCV);
        if(m_CondVar.wait_for(lock, std::chrono::milliseconds(m_TimeInterval))
                            == std::cv_status::timeout && m_Active)
        {
            m_Callback();
        }
        else
        {
            return;
        }

    }while(m_AutoReset);
}

void Timer::Start()
{
    if (!m_Callback)
    {
        return;
    }

    if (!m_TimerThread.joinable())
    {
        {
            m_Active = true;
        }
        m_TimerThread = std::thread(&Timer::TimerThreadFunc, this);
        m_TimerThread.detach();
    }
}

void Timer::Restart(uint64_t timeMs)
{
    if (timeMs != 0UL)
    {
        m_TimeInterval = timeMs;
    }

    this->Start();
}

void Timer::Stop()
{
    {
        std::lock_guard<std::mutex> lg(m_MutexForCV);
        m_Active = false;
    }
    
    m_CondVar.notify_one();
}

void Timer::Dispose()
{
    this->Stop();
    m_TimeInterval = 0UL;
    m_Callback = nullptr;
}
