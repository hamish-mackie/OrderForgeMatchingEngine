Highlights

Hashed Linked List
Trade Producer
Scaled Int

Where to start?
main.cpp contains a demo app which outlines the interface
exchange/order_book.h should be the next stop
exchange/trade_producer.h handles order matching
exchange/book_level.h holds the hashed linked list data structure

Getting a demo app running is straight forward 
```
mkdir build-release && cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
./demo_run
```





