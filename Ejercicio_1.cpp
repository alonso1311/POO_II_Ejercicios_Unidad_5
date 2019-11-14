#include<iostream>
#include<thread>
#include<random>
#include<vector>
#include<future>

using namespace std;

random_device rd;

void Ejercicio_1() {
    const size_t n = 8;
    auto nt = thread::hardware_concurrency();

    auto range = n/nt;

    cout << n << endl;
    cout << nt << endl;
    cout << range << endl;

    vector<int> v(n);

    auto filler = [&v](size_t start, size_t stop) {
        for(auto i = start; i < stop; i++) 
            v[i] = rd() % 10000;
    };

    vector<thread> vt(nt);

    for(size_t i= 0; i < nt; i++) 
        vt[i] = thread(filler, range*i, range*(i+1));
    
    for(auto &t : vt) 
        t.join();

    vector<promise<int>> v_prm(nt);
    vector<future<int>> ftr;

    for(auto& prm : v_prm)
        ftr.emplace_back(prm.get_future());


    auto summarizer = [&v](promise<int>& prm, size_t start, size_t stop) {
        auto total = 0;
        for(auto i = start; i < stop; i++) 
            total += v[i];

        prm.set_value(total);
    };

    vector<thread> vt2(nt);
    for(size_t i = 0; i < nt; i++) 
        vt[i] = thread(summarizer, ref(v_prm[i]), range*i, range*(i+1));

    for(size_t i = 0; i < nt; i++) 
        vt[i].join();
    
    auto total = accumulate(begin(ftr), end(ftr), 0, [](int acc, future<int>& v_ftr){
            return acc + v_ftr.get();
    });

    cout << "Total: " << total;
}

int main() {
    Ejercicio_1();
    return 0;
}
