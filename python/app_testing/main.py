import asyncio
import json
from contextlib import asynccontextmanager

order_data1 = {
    "symbol": "TEST1",
    "price": 150.25,
    "qty": 100,
    "order_side": "BUY",
    "order_status": "OPEN",
    "order_type": "LIMIT",
    "client_order_id": 12345,
    "order_id": 67890,
    "timestamp": 1693332765,
    "acc_id": 98765
}

order_data2 = {
    "symbol": "TEST1",
    "price": 150.25,
    "qty": 100,
    "order_side": "SELL",
    "order_status": "OPEN",
    "order_type": "LIMIT",
    "client_order_id": 12345,
    "order_id": 67890,
    "timestamp": 1693332765,
    "acc_id": 98765
}


@asynccontextmanager
async def open_connection(host, port):
    reader, writer = await asyncio.open_connection(host, port)
    try:
        yield reader, writer
    finally:
        writer.close()
        await writer.wait_closed()
        print("Connection closed.")


async def send_orders(host, port):
    try:
        async with open_connection(host, port) as (reader, writer):
            print(f"Connected to {host}:{port}")
            json_data1 = json.dumps(order_data1).encode('utf8')
            writer.write(json_data1)
            await writer.drain()
            print("Order 1 sent!")

            json_data2 = json.dumps(order_data2).encode('utf8')
            writer.write(json_data2)
            await writer.drain()
            print("Order 2 sent!")
            await asyncio.sleep(5)
    except Exception as e:
        print("Error:", e)


async def read_messages(host, port):
    try:
        async with open_connection(host, port) as (reader, writer):
            print(f"Connected to {host}:{port}")

            while True:
                message_data = await reader.readline()
                message = message_data.decode('utf-8').strip()
                if message:
                    print(f"Message from server: [{port}] {message}")
                else:
                    print(f"Server closed the connection: {port}")
                    break

    except Exception as e:
        print("Error:", e)

async def main():
    host = "localhost"
    send_port = 7020

    ports = [7020, 7021, 7010, 7011, 7012]
    tasks = []

    for port in ports:
        tasks.append(asyncio.create_task(read_messages(host, port)))


    tasks.append(asyncio.create_task(send_orders(host, send_port)))

    await asyncio.gather(*tasks)

if __name__ == "__main__":
    asyncio.run(main())
