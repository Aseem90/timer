/******************************************************************************
* File Name:        timer.hpp
* Description:      
* Notes:            
* Author:           Aseem Tiwari
* Date:             10/02/2021
******************************************************************************/

#include <condition_variable>
#include <functional>
#include <thread>

class Timer final
{
public:
    Timer(uint64_t timeMs, const std::function<void()>& callback, bool autoReset);

    ~Timer();

    void Start();

    void Stop();

    void Restart(uint64_t timeMs = 0UL);

    void Dispose();

private:
    std::thread m_TimerThread;
    void TimerThreadFunc();
    bool m_Active = false;
    bool m_AutoReset = false;
    std::function<void()> m_Callback = nullptr;
    uint64_t m_TimeInterval = 0U;
    std::condition_variable m_CondVar;
    std::mutex m_MutexForCV;
};
