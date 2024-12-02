import socket
import orjson
import json

order_data1 = {
    "symbol": "TEST1",
    "price": 150.25,
    "qty": 100,
    "remaining_qty": 50,
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
    "remaining_qty": 50,
    "order_side": "SELL",
    "order_status": "OPEN",
    "order_type": "LIMIT",
    "client_order_id": 12345,
    "order_id": 67890,
    "timestamp": 1693332765,
    "acc_id": 98765
}


def send_order():


    host = "localhost"
    port = 7002

    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
            client_socket.connect((host, port))
            print(f"Connected to {host}:{port}")
            json_data = json.dumps(order_data1).encode('utf8')
            client_socket.sendall(json_data)
            print("Order sent!")

            response = client_socket.recv(1024)
            print("Response from server:", response.decode('utf-8'))

            json_data = json.dumps(order_data2).encode('utf8')
            client_socket.sendall(json_data)
            print("Order sent!")

            response = client_socket.recv(1024)
            print("Response from server:", response.decode('utf-8'))

    except Exception as e:
        print("Error:", e)


if __name__ == "__main__":
    send_order()
