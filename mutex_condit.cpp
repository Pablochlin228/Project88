// parking_cv_fixed.cpp
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <condition_variable>

int free_slots = 3;                 // количество свободных мест
std::mutex mtx;                     // защищает доступ к free_slots и синхронизацию вывода
std::condition_variable cv;         // очередь ожидания для машин

void car(int id)
{
    using namespace std::chrono;

    // "Подъехали" и сразу пытаемся занять место
    {
        std::lock_guard<std::mutex> lg(mtx);
        std::cout << "Машина " << id << " подъехала и ждёт место...\n";
    }

    // Ждём, пока появится свободное место
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return free_slots > 0; }); // защищаемся от ложных пробуждений
        --free_slots; // занимаем место
        std::cout << "Машина " << id << " припарковалась. (свободно: " << free_slots << ")\n";
        // мьютекс освобождаем сразу после изменения состояния;
        // стоять на парковке не нужно под защитой мьютекса.
    }

    // Стоим на парковке фиксированное время
    std::this_thread::sleep_for(seconds(1));

    // Уезжаем: освобождаем место и будим одного ожидающего
    {
        std::lock_guard<std::mutex> lock(mtx);
        ++free_slots;
        std::cout << "Машина " << id << " уехала. (свободно: " << free_slots << ")\n";
    }
    cv.notify_one(); // разбудим одну машину из очереди
}

int main() {
    setlocale(0, "ru");
    std::vector<std::thread> cars;
    const int total_cars = 6;

    for (int i = 1; i <= total_cars; ++i)
        cars.emplace_back(car, i);

    for (auto& t : cars) t.join();

    std::cout << "Парковка свободна. Все машины обработаны.\n";
    return 0;
}
