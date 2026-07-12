# 2603-ITT440

## 1. Introduction

The DUO Project is a distributed systems project developed for ITT440 to demonstrate the integration of networking, database management, and containerization in a practical application. The system implements multiple TCP socket servers written in Python and C, a client application, and a shared MySQL database. The project illustrates how distributed services can be deployed and coordinated using Docker Compose in a single environment.

## 2. Objectives

The objectives of this project are to:
- design and implement a distributed client-server system,
- demonstrate communication between clients and backend servers through TCP sockets,
- store and retrieve shared data from a MySQL database,
- deploy the application using Docker containers, and
- evaluate the interaction between different programming languages and services.

## 3. Software and Technologies Used

The project was developed using the following technologies:
- Python 3.9 and Python 3.11
- C programming language
- MySQL 8.0
- Docker and Docker Compose
- TCP socket programming
- MySQL Connector for Python
- MariaDB development library for C

## 4. System Architecture

The system follows a layered client-server architecture with a central database. Clients send requests to backend servers, which process the request and update or retrieve data from the MySQL database.

### Architecture Diagram

```text
[Client] -> [Python Server] -> [MySQL Database]
[Client] -> [C Server]     -> [MySQL Database]
```

### Workflow
1. A client sends a request such as `view` or `reset` to a server.
2. The server connects to the MySQL database.
3. The database stores or returns the current score.
4. The server sends a response back to the client.

## 5. Project Structure

```text
DUO Project/
├── .env
├── GP_Compose.yaml
├── GP_Client/
│   ├── client.py
│   └── Dockerfile
├── GP_C_Server_1/
│   ├── Docker
│   └── server.c
├── GP_C_Server_2/
│   ├── Docker
│   └── server.c
├── GP_C_Server_3/
│   ├── Docker
│   └── server.c
├── GP_Database/
│   └── init.sql
├── GP_Python_Server_1/
│   ├── Docker
│   ├── requirements.txt
│   └── server.py
├── GP_Python_Server_2/
│   ├── Docker
│   ├── requirements.txt
│   └── server.py
├── GP_Python_Server_3/
│   ├── Docker
│   ├── requirements.txt
│   └── server.py
```

## 6. Database Design

The database is implemented using MySQL and contains a single table named `scoreboard`.

### Database Schema

```sql
CREATE DATABASE IF NOT EXISTS GP_Database;
USE GP_Database;

CREATE TABLE IF NOT EXISTS scoreboard (
    user VARCHAR(50) PRIMARY KEY,
    points INT NOT NULL DEFAULT 0,
    datetime_stamp DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
);
```

### Purpose of the Table
- stores the points for each user,
- records the latest update timestamp, and
- allows multiple servers to access shared data consistently.

## 7. Docker Compose Configuration

Docker Compose is used to orchestrate the complete system. It defines the database container, multiple Python and C servers, and client containers in a common network.

### Example Configuration

```yaml
services:
  gp_database:
    image: mysql:8.0
    container_name: gp_database
    environment:
      MYSQL_ROOT_PASSWORD: root
    ports:
      - "3306:3306"
    volumes:
      - Database_Data:/var/lib/mysql
      - ./GP_Database/init.sql:/docker-entrypoint-initdb.d/init.sql
```

The compose file also creates:
- 3 Python socket servers on ports `11001`, `11002`, and `11003`
- 3 C socket servers on ports `12001`, `12002`, and `12003`
- 6 client containers that connect to the servers
- a shared Docker network named `gp_network`

## 8. Implementation

### 8.1 Docker Container Deployment

Each component is deployed in an isolated Docker container. The database container is started first, followed by the Python and C servers and the client containers. This separation enables modular development and simpler testing.

```yaml
services:
  gp_python_server_1:
    build:
      context: ./GP_Python_Server_1
      dockerfile: Docker
    ports:
      - "11001:11001"
```

### 8.2 Python Socket Server

The Python servers listen for incoming client requests and interact with the MySQL database. A background thread periodically updates the score and stores the latest values in the database.

```python
import socket
import mysql.connector

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(("0.0.0.0", 11001))
server.listen(5)
```

### 8.3 C Socket Server

The C servers provide the same functionality using the C language and the MySQL C API. They accept requests from clients, query the database, and return the current score.

```c
#include <mysql/mysql.h>

MYSQL *conn;
conn = mysql_init(NULL);
mysql_real_connect(conn, "gp_database", "root", "root", "GP_Database", 0, NULL, 0);
```

## 9. Docker Files

### Python Server Dockerfile

```dockerfile
FROM python:3.9

WORKDIR /app

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY server.py .

CMD ["python","server.py"]
```

### C Server Dockerfile

```dockerfile
FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y libmariadb-dev && rm -rf /var/lib/apt/lists/*

COPY server.c .

RUN gcc server.c -o server -lmariadb -lpthread

CMD ["./server"]
```

### Client Dockerfile

```dockerfile
FROM python:3.11-slim

WORKDIR /app
COPY client.py /app/client.py

CMD ["python", "client.py"]
```

## 10. Steps to Run the Project

1. Ensure Docker and Docker Compose are installed and running.
2. Open the project directory.
3. Start the full system:

```bash
docker compose -f GP_Compose.yaml up --build -d
```

4. Verify that the containers are running:

```bash
docker compose -f GP_Compose.yaml ps
```

5. Test the application using a client container:

```bash
docker compose -f GP_Compose.yaml run --rm gp_client_python_1 python client.py gp_python_server_1 11001 view
```

6. To reset the score:

```bash
docker compose -f GP_Compose.yaml run --rm gp_client_python_1 python client.py gp_python_server_1 11001 reset
```

## 11. Testing and Results

The system was tested by running client requests against both Python and C servers. The expected results were:
- the server returned the current score,
- the score could be reset successfully, and
- the database recorded the updated values.

Example output:

```text
python_user1 score: 0
```

## 12. Problems Faced

Several challenges were encountered during the implementation:
- the MySQL container required time to initialize before the servers could connect,
- Docker networking had to be configured correctly using container hostnames, and
- the server applications needed consistent request handling to respond properly.

These issues were addressed by implementing retry logic and ensuring the correct service dependencies and network configuration.

## 13. Conclusion

The DUO Project successfully demonstrates a distributed application built using Docker, Python, C, TCP sockets, and MySQL. The project shows how multiple services can work together in a coordinated environment and how shared data can be accessed and updated through a central database.
