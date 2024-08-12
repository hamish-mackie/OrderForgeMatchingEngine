# OrderForge
OrderForge is a fast C++ based order matching engine.
[OrderForge is free to use for educational, non-commercial and some small businesses](https://polyformproject.org/licenses/small-business/1.0.0)
https://polyformproject.org/licenses/small-business/1.0.0

## Benchmark Results
The engine can **insert/remove/match** up to:
- Logging Enabled:  **2.4 million orders/second**.
- Logging Disabled:  **2.7 million orders/second**.

---

## Highlights

### Hashed Linked List
- **Hashed linked list** is an efficient data structure for an exchange matching engine.
- **Operations**:
    - `push_back(insert order)`: O(1)
    - `remove(cancel order)`: O(1)
- The underlying linked list simplifies iteration, while mapping the values by `OrderId` provides O(1) lookup.
- Hashed Linked List nodes use a custom pool allocator to avoid costly allocations

### Logging
- The specialized logging system offloads all formatting and logging to another thread.
- The main engine thread only needs to copy around **100 bytes** for each order log.
- This setup significantly speeds up the process compared to formatting and logging in the main thread.

### Trade Producer

### Scaled Int

---

## Where to Start?

1. `main.cpp` contains a demo app which outlines the interface.
2. `exchange/order_book.h` should be the next stop.
3. `exchange/trade_producer.h` handles order matching.
4. `exchange/book_level.h` holds the hashed linked list data structure.

---

## Running the Demo App

Getting the demo app running is straightforward:

```bash
mkdir build-release && cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
./demo_run

## License
https://polyformproject.org/licenses/small-business/1.0.0