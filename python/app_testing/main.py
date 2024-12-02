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


async def send_orders(writer):
    json_data1 = json.dumps(order_data1).encode('utf8')
    writer.write(json_data1)
    await writer.drain()
    print("Order 1 sent!")

    json_data2 = json.dumps(order_data2).encode('utf8')
    writer.write(json_data2)
    await writer.drain()
    print("Order 2 sent!")


async def read_messages(reader):
    while True:
        message_data = await reader.readline()
        message = message_data.decode('utf-8').strip()
        if message:
            print(f"Message from server: {message}")
        else:
            print("Server closed the connection.")
            break


async def main():
    host = "localhost"
    port = 7002

    try:
        async with open_connection(host, port) as (reader, writer):
            print(f"Connected to {host}:{port}")

            send_task = asyncio.create_task(send_orders(writer))
            read_task = asyncio.create_task(read_messages(reader))

            await send_task
            await read_task

    except Exception as e:
        print("Error:", e)

if __name__ == "__main__":
    asyncio.run(main())
