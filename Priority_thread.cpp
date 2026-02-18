#include <windows.h>
#include <thread>
#include <iostream>
using namespace std;

void work(const char* name, int loops)
{
    for (int i = 0; i < loops; i++)
    {
        // имитируем нагрузку
        for (int j = 0; j < 1000000; j++) {}
        cout << name << " i =" << i << "\n";
    }
}

int main() {
    thread t1(work, "LOW", 10);
    thread t2(work, "HIGH", 5);

    // Получаем системные хэндлы
    HANDLE h1 = (HANDLE)t1.native_handle();
    HANDLE h2 = (HANDLE)t2.native_handle();

    // Устанавливаем приоритеты
    // Доступные уровни: THREAD_PRIORITY_LOWEST, BELOW_NORMAL, NORMAL,
    // ABOVE_NORMAL, HIGHEST, TIME_CRITICAL и др.
    SetThreadPriority(h1, THREAD_PRIORITY_LOWEST);
    SetThreadPriority(h2, THREAD_PRIORITY_TIME_CRITICAL);

    t1.join();
    t2.join();
}