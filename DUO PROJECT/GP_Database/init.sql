CREATE DATABASE IF NOT EXISTS GP_Database;
USE GP_Database;

CREATE TABLE IF NOT EXISTS scoreboard (
    user VARCHAR(50) PRIMARY KEY,
    points INT NOT NULL DEFAULT 0,
    datetime_stamp DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
);

INSERT INTO scoreboard VALUES
('python_user1',0,NOW()),
('python_user2',0,NOW()),
('python_user3',0,NOW()),
('c_user1',0,NOW()),
('c_user2',0,NOW()),
('c_user3',0,NOW());
