import socket
import time
import mysql.connector

from threading import Thread
from datetime import datetime
from zoneinfo import ZoneInfo

HOST = "0.0.0.0"
PORT = 11001

# Wait for MySQL to be ready
for attempt in range(60):
    try:
        db = mysql.connector.connect(
            host="gp_database",
            user="root",
            password="root",
            database="GP_Database"
        )
        break
    except Exception as e:
        print(f"Attempt {attempt+1}: Waiting for MySQL... {e}")
        time.sleep(1)

cursor = db.cursor()

# Load existing points from DB instead of starting at 0
cursor.execute("SELECT points FROM scoreboard WHERE user='python_user1'")
points = cursor.fetchone()[0]
print(f"Resuming from points: {points}")

def update_db():
    global points

    while True:

        points += 10

        current_time = datetime.now()

        cursor.execute("""
        UPDATE scoreboard
        SET points=%s,
            datetime_stamp=%s
        WHERE user='python_user1'
        """, (points, current_time))

        db.commit()

        print(
            f"python_user1 updated | "
            f"{current_time}"
        )

        time.sleep(30)

Thread(target=update_db, daemon=True).start()

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

server.bind((HOST, PORT))
server.listen(5)

print("Python Server 1 Running")

while True:
    client, address = server.accept()

    try:
        request = client.recv(1024).decode().strip().lower()

        if request == "reset":
            points = 0
            current_time = datetime.now()
            cursor.execute("""
            UPDATE scoreboard
            SET points=%s,
                datetime_stamp=%s
            WHERE user='python_user1'
            """, (points, current_time))
            db.commit()
            response = f"python_user1 score reset to 0 at {current_time}"
        else:
            cursor.execute("""
            SELECT points
            FROM scoreboard
            WHERE user='python_user1'
            """)
            result = cursor.fetchone()
            response = f"python_user1 score: {result[0] if result else 0}"

        client.sendall(response.encode())
    except Exception as e:
        client.sendall(f"ERROR: {e}".encode())
    finally:
        client.close()
