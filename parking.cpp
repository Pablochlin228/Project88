#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <fstream>
#include <mutex>
#include <semaphore>

//1
class Car
{
	std::string model;
	int stateNumber;
public:
	Car(std::string m, int n)
	{
		model = m;
		stateNumber = n;
	}
	void Arrive()
	{
		std::cout << "Car " << model  << " with state number"  << stateNumber
		<< " arrived and trying to find a free parking space!" << std::endl;
	}
	void Park()
	{
		std::cout << "Car " << model << " with state number" << stateNumber
		<< " parked successfully!" << std::endl;
	}
	void Leave()
	{
		std::cout << "Car " << model << " with state number" << stateNumber
		<< " left from parking space!" << std::endl;
	}
};

std::counting_semaphore<3> parking(3);

void Parking(Car car)
{
	using namespace std::chrono;
	car.Arrive();

	parking.acquire();

	car.Park();
	std::this_thread::sleep_for(seconds(3));
	car.Leave();

	parking.release();
}

//2
std::counting_semaphore<2> control(2);
std::mutex m;

void Worker(int id)
{
	std::cout << "Thread " << id << "is trying to write his line in file!" << std::endl;

	control.acquire();
	m.lock();
	std:: ofstream out("log.txt", std::ios::app);
	if (out.is_open())
	{
		out << "Thread " << id << "write information" << std::endl;
		out.close();
	}
	else
	{
		std::cout << "Could not open the file" << std::endl;
	}
	m.unlock();
	control.release();
}

//3
int data[20] = { 0 };
int nextIndex = 0;
std::mutex dataMutex;

void FillArray(int id)
{
	while (true)
	{
		control.acquire();

		dataMutex.lock();

		if (nextIndex >= 20)
		{
			control.release();
			break;
		}

		int currantIndex = nextIndex;
		data[currantIndex] = id;
		nextIndex++;

		dataMutex.unlock();

		control.release();
	}
}

int main()
{
	//1
	std::thread car1(Parking, Car("Toyota", 35643));
	std::thread car2(Parking, Car("Cadilac", 03223));
	std::thread car3(Parking, Car("Niva", 00456));
	std::thread car4(Parking, Car("BMV", 45034));
	std::thread car5(Parking, Car("Lexus", 11045));
	std::thread car6(Parking, Car("Audi", 44504));

	car1.join();
	car2.join();
	car3.join();
	car4.join();
	car5.join();
	car6.join();

	std::cout << "Parking is free now!!!" << std::endl;

	//2
	std::vector<std::thread> writers;
	for (int i = 1; i <= 6; ++i) {
		writers.emplace_back(Worker, i);
	}

	for (auto& t : writers) t.join();

	std::cout << "Task is done!!!" << std::endl;

	//3
	std::vector<std::thread> numbers;
	for (int i = 1; i <= 5; ++i) {
		numbers.emplace_back(FillArray, i);
	}

	for (auto& t : numbers) t.join();

	for (int i = 0; i < 20; i++)
	{
		std::cout << data[i] << std::endl;
	}

}

//Есть глобальный массив из 20 элементов(например int data[20];), изначально заполненный нулями.
//
//Есть несколько потоков(например 5).
//
//Каждый поток должен :
//получить «разрешение» от семафора(например на 2 пропуска),
//найти первый нулевой элемент массива,
//записать туда номер потока(или любое своё значение),
//освободить разрешение семафора.
//
//Работа продолжается, пока весь массив не будет заполнен.
//
//После завершения работы программа выводит массив.
//
//Важно:
//Необходимо предотвратить ситуацию, когда два потока одновременно пытаются записать в один и тот же индекс.