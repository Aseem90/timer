/******************************************************************************
* File Name:        main.cpp
* Description:      Driver code to test the Timer
* Notes:            
* Author:           Aseem Tiwari
* Date:             10/02/2021
******************************************************************************/

#include "timer.hpp"

// Event handler
void WhenTimerExpire()
{
    std::cout << "myTimer expired!! " << "\n";
}

// Driver code to test timer
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
    return 0;
}
