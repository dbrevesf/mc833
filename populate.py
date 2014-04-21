#!/usr/bin/python

import sqlite3

conn = sqlite3.connect("estabelecimentos.db")

cursor = conn.cursor()

cursor.execute(""" DROP TABLE IF EXISTS estabelecimentos""")
cursor.execute("""CREATE TABLE estabelecimentos(
					ID INTEGER PRIMARY KEY AUTOINCREMENT,
					name TEXT,
					coord_x REAL NOT NULL,
					coord_y REAL NOT NULL,
					information TEXT,
                    category TEXT)""")
                    
cursor.execute(""" DROP TABLE IF EXISTS notas""")					
cursor.execute("""CREATE TABLE notas(
					voteId INTEGER PRIMARY KEY AUTOINCREMENT,
					estabId INTEGER, 
					rate REAL,
					FOREIGN KEY (estabId) REFERENCES estabelecimentos(ID))""")
					
print("+ database created")

estabelecimentos = [(None, 'Posto Shell', 10, 300, 'Posto de gasolina e conveniencia', 'conveniencia'),
					(None, 'McDonalds', 10, 290, 'Fast-food gorduroso', 'fastfood'),
					(None, 'BarDoZe', 10, 250, 'Melhor bar de Barao', 'bar'),
					(None, 'BurgerKing', 10, 100, 'Fast-food mais gorduroso', 'fastfood'),
					(None, 'Dogao', 0, 200, 'Melhor hot-dog do mundo','fastfood'),
					(None, 'Subway', 20, 350, 'Fast-food saudavel', 'fastfood'),
					(None, 'Eskina', 0, 205, 'Bar honesto', 'bar')]
notas = [(None, 1, 5),
		 (None, 2, 5),
		 (None, 3, 5),
		 (None, 4, 5),
		 (None, 5, 5),
		 (None, 6, 5),
		 (None, 7, 5)]


cursor.executemany("INSERT INTO estabelecimentos VALUES (?,?,?,?,?,?)", estabelecimentos)
cursor.executemany("INSERT INTO notas VALUES (?,?,?)", notas)

print("+ database populated")

conn.commit()

print("+ databes commited")
