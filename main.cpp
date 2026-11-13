#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <syncstream>
#include <thread>

using namespace std;

void execute_task(const string& name, int duration_seconds) {
    this_thread::sleep_for(chrono::seconds(duration_seconds));
    osyncstream(cout) << name << '\n';
}

void slow(const string& name) {
    execute_task(name, 7);
}

void quick(const string& name) {
    execute_task(name, 1);
}

void worker1(promise<void>& p_A, promise<void>& p_D2) {
    quick("A");
    p_A.set_value();

    slow("D2");
    p_D2.set_value();

    quick("F2");
}

void worker2() {
    slow("B");
}

void worker3(future<void>& f_A, future<void>& f_B, future<void>& f_D2) {
    slow("C");

    f_A.get();
    f_B.get();
    quick("D1");

    f_D2.get();
    quick("F1");
}

int main() {
    const auto start_time = chrono::steady_clock::now();

    promise<void> p_A;
    future<void> f_A = p_A.get_future();

    promise<void> p_D2;
    future<void> f_D2 = p_D2.get_future();

    future<void> f_worker1 = async(launch::async, worker1, ref(p_A), ref(p_D2));
    future<void> f_B = async(launch::async, worker2);

    worker3(f_A, f_B, f_D2);

    f_worker1.get();

    const auto end_time = chrono::steady_clock::now();
    const auto elapsed = chrono::duration<double>(end_time - start_time).count();

    osyncstream(cout)
        << "Execution time: " << elapsed << " s\n"
        << "Work is done!\n";

    return 0;
}
