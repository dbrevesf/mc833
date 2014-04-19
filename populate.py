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
                    category TEXT,
					rate REAL)""")
print("+ database created")

estabelecimentos = [(001, 'Posto Shell', 0, 300, 'Posto de gasolina e conveniencia', 'conveniencia', 5),
					(002, 'McDonalds', -50, 300, 'Fast-food gorduroso', 'fastfood', 7),
					(003, 'BarDoZe', -200, 300, 'Melhor bar de Barao', 'bar', 10),
					(004, 'BurgerKing', -600, 300, 'Fast-food mais gorduroso', 'fastfood', 5),
					(005, 'Dogao', -250, 350, 'Melhor hot-dog do mundo','fastfood', 8),
					(006, 'Subway', 100, 300, 'Fast-food saudavel', 'fastfood', 6),
					(007, 'Eskina', -200, 350, 'Bar honesto', 'bar', 7)]


cursor.executemany("INSERT INTO estabelecimentos VALUES (?,?,?,?,?,?,?)", estabelecimentos)

print("+ database populated")

conn.commit()

print("+ databes commited")
