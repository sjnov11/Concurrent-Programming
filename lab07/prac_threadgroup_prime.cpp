#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#define NUM_THREAD_IN_POOL  4

bool IsPrime(int n) {
    if (n < 2) {
        return false;
    }

    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

void Print(int start, int end, int count, int seq_count) {
    std::cout << "(" << seq_count << ")" <<  "number of primes in " << start << " ~ " << end << " is " << count << std::endl;

}

void ThreadFunc(int start, int end, int seq_count, boost::asio::io_service* io2) {
   long cnt_prime = 0;
    for (int i = start; i < end; i++) {
        if(IsPrime(i)) {
            cnt_prime++;
        }
    }

    io2->post(boost::bind(
                Print, start, end, cnt_prime, seq_count));
  
}

int main(void) {
    boost::asio::io_service io1, io2;
    boost::thread_group threadpool;
    boost::thread_group printthread;
    boost::asio::io_service::work* work1 = new boost::asio::io_service::work(io1);
    boost::asio::io_service::work* work2 = new boost::asio::io_service::work(io2);

    for (int i = 0; i < NUM_THREAD_IN_POOL - 1; i++) {
        threadpool.create_thread(boost::bind(
                    &boost::asio::io_service::run, &io1));
    }

    printthread.create_thread(boost::bind(
            &boost::asio::io_service::run, &io2));

    int range_start, range_end, seq_count;
    seq_count = 0;
    while (1) {
        std::cin >> range_start;
        std::cin >> range_end;

        if (range_start == -1) break;
        io1.post(boost::bind(
                   ThreadFunc, range_start, range_end, seq_count++, &io2));
    }

    delete work1;
    //io1.stop();
    threadpool.join_all();
    io1.stop();

    delete work2;
    printthread.join_all();
   //delete work2;


    //io1.stop();
    io2.stop();
}
