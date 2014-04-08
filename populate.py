#!/usr/bin/python

import sqlite3

conn = sqlite3.connect("estabelecimentos.db")

cursor = conn.cursor()

cursor.execute(""" DROP TABLE IF EXISTS estabelecimentos""")
cursor.execute("""CREATE TABLE estabelecimentos(
					ID INTEGER PRIMARY KEY,
					name TEXT,
					coord_x REAL NOT NULL,
					coord_y REAL NOT NULL,
					information TEXT,
					rate REAL)""")
print("+ database created")

estabelecimentos = [(001, 'Posto Shell', 0, 300, 'Posto de gasolina e conveniencia', 5),
					(002, 'McDonalds', -50, 300, 'Fast-food gorduroso', 7),
					(003, 'BarDoZe', -200, 300, 'Melhor bar de Barao', 10),
					(004, 'BurgerKing', -600, 300, 'Fast-food mais gorduroso', 5),
					(005, 'Dogao', -250, 350, 'Melhor hot-dog do mundo', 8),
					(006, 'Subway', 100, 300, 'Fast-food saudavel', 6),
					(007, 'Eskina', -200, 350, 'Bar honesto', 7)]


cursor.executemany("INSERT INTO estabelecimentos VALUES (?,?,?,?,?,?)", estabelecimentos)

print("+ database populated")

conn.commit()

print("+ databes commited")