-- Create the database
CREATE DATABASE IF NOT EXISTS ItemBasedCF_DB;
USE ItemBasedCF_DB;

-- 1. Users Table
CREATE TABLE IF NOT EXISTS Users (
    user_id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) NOT NULL
);

-- 2. Items Table (Movies/Products)
CREATE TABLE IF NOT EXISTS Items (
    item_id INT PRIMARY KEY AUTO_INCREMENT,
    title VARCHAR(100) NOT NULL,
    genre VARCHAR(50)
);

-- 3. Interactions Table (Ratings)
CREATE TABLE IF NOT EXISTS Interactions (
    interaction_id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT,
    item_id INT,
    rating FLOAT,
    FOREIGN KEY (user_id) REFERENCES Users(user_id),
    FOREIGN KEY (item_id) REFERENCES Items(item_id)
);

-- Insert Dummy Data

-- Users
INSERT INTO Users (username) VALUES ('Alice'), ('Bob'), ('Charlie'), ('David'), ('Eve');

-- Items (Movies)
INSERT INTO Items (title, genre) VALUES 
('The Matrix', 'Sci-Fi'),
('Inception', 'Sci-Fi'),
('Titanic', 'Romance'),
('The Notebook', 'Romance'),
('Die Hard', 'Action'),
('Mad Max', 'Action');

-- Interactions
-- Alice likes Sci-Fi
INSERT INTO Interactions (user_id, item_id, rating) VALUES 
(1, 1, 5.0), -- Matrix
(1, 2, 4.5), -- Inception
(1, 5, 2.0); -- Die Hard

-- Bob likes Sci-Fi too
INSERT INTO Interactions (user_id, item_id, rating) VALUES 
(2, 1, 4.0), -- Matrix
(2, 2, 5.0), -- Inception
(2, 6, 3.0); -- Mad Max

-- Charlie likes Romance
INSERT INTO Interactions (user_id, item_id, rating) VALUES 
(3, 3, 5.0), -- Titanic
(3, 4, 4.5), -- Notebook
(3, 1, 2.0); -- Matrix

-- David likes Action
INSERT INTO Interactions (user_id, item_id, rating) VALUES 
(4, 5, 5.0), -- Die Hard
(4, 6, 4.5), -- Mad Max
(4, 2, 3.0); -- Inception

-- Eve (New user, few ratings)
INSERT INTO Interactions (user_id, item_id, rating) VALUES 
(5, 1, 3.0);

