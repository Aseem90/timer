#include <iostream>
#include <functional>
#include <thread>
#include <chrono>
#include <atomic>
#include <condition_variable>

class Timer
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
    std::atomic_bool m_Active{false};
    bool m_AutoReset = false;
    std::function<void()> m_Callback = nullptr;
    uint64_t m_TimeInterval = 0U;
    std::condition_variable m_CondVar;
    std::mutex m_MutexForCV;
};

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

// void Timer::TimerThreadFunc()
// {
//     do
//     {
//         if (!m_Active.load())
//         {
//             return;
//         }

//         std::this_thread::sleep_for(std::chrono::milliseconds(m_TimeInterval));

//         if (!m_Active.load())
//         {
//             return;
//         }

//         // std::cout << "Thread Id - " << std::this_thread::get_id() << "\n";
        
//         m_Callback();
//     }while(m_AutoReset);
// }

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

        // std::unique_lock<std::mutex> lock(m_MutexForCV);
        // if(m_CondVar.wait_for(lock, std::chrono::milliseconds(m_TimeInterval),[this]
        // {
        //     return (m_Active == false);
        // }))
        // {
        //     return;
        // }
        
        // m_Callback();
    }while(m_AutoReset);
}

void Timer::Start()
{
    std::cout << "Starting Timer" << "\n";
    if (!m_Callback)
    {
        return;
    }

    if (!m_TimerThread.joinable())
    {
        m_Active = true;
        m_TimerThread = std::thread(&Timer::TimerThreadFunc, this);
        m_TimerThread.detach();
    }
}

void Timer::Restart(uint64_t timeMs)
{
    std::cout << "Restarting Timer" << "\n";
    if (timeMs != 0UL)
    {
        m_TimeInterval = timeMs;
    }

    this->Start();
}

void Timer::Stop()
{
    std::cout << "Stopping Timer" << "\n";
    std::lock_guard<std::mutex> lg(m_MutexForCV);
    m_Active = false;
    m_CondVar.notify_one();
}

void Timer::Dispose()
{
    this->Stop();
    m_TimeInterval = 0UL;
    m_Callback = nullptr;
}

void WhenTimerExpire()
{
    std::cout << "myTimer expired!! " << "\n";
}

int main()
{
    Timer myTimer(3000UL, &WhenTimerExpire, true);
    myTimer.Start();

    // std::this_thread::sleep_for(std::chrono::seconds(121));

    // Test2: Stop the timer after 7 secs but main thread should still be running. 
    std::this_thread::sleep_for(std::chrono::seconds(13));
    myTimer.Stop();
    myTimer.Restart(5000UL);

    std::this_thread::sleep_for(std::chrono::milliseconds(25005));
    std::cout << "Main exiting now..." << "\n";
    return 0;
}