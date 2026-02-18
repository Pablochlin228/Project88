#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <semaphore>
using namespace std;

// —оздаЄм семафор.
// <2> Ч это *максимальное количество ресурсов*, то есть сколько "пропусков" может быть одновременно.
// sem(2) Ч начальное количество пропусков. «начит, одновременно работать могут только 2 потока.
counting_semaphore<2> sem(2);

void task(int id) {
    // acquire() Ч попытка забрать один пропуск.
    // ≈сли пропуск есть Ч поток сразу пойдЄт дальше.
    // ≈сли пропуска нет Ч поток будет ждать, пока его освобод€т.
    sem.acquire();

    cout << "ѕоток " << id << " начал работу\n";

    // »митируем т€жЄлую работу Ч пусть поток Ђзан€тї 0.5 секунды.
     this_thread::sleep_for(chrono::milliseconds(500));

    cout << "ѕоток " << id << " закончил работу\n";

    // release() Ч возвращаем пропуск, даЄм возможность другому потоку начать работу.
    sem.release();
}

int main() {
    setlocale(0, "ru");
   vector<thread> threads;

    // «апускаем 5 потоков.
    // Ќо одновременно смогут работать только 2 Ч благодар€ семафору.
    for (int i = 1; i <= 5; ++i)
        threads.emplace_back(task, i);

    // ∆дЄм завершени€ всех потоков.
    for (auto& t : threads)
        t.join();

    return 0;
}