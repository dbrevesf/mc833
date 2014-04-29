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
					(None, 'Eskina', 0, 205, 'Bar honesto', 'bar'),
                    (None, 'Dalben', 0 , 0, 'Supermercado', 'mercado'),
					(None, 'PadariaAlema', 150, 10, 'Melhor padaria de Campinas', 'padaria'),
					(None, 'PepeLoko', 900, 0, 'Fast-food mexicano gorduroso e caro', 'fastfood'),
					(None, 'Posto Ipiranga', 900, 10, 'Conveniencia Cara', 'conveniencia'),
					(None, 'Bardana', 850, 0, 'Por quilo muito gostoso','restaurante'),
					(None, 'Terraco', 1000, 50, 'Por quilo saudavel', 'restaurante'),
					(None, 'TemperoManero', 200, 750, 'Restaurante digno', 'restaurante'),
                    (None, 'Bagda', 200, 780, 'Esfiha aberta', 'restaurante'),
					(None, 'Temakeria', 700, 10, 'Fast-food gorduroso', 'fastfood'),
					(None, 'Aulus', 1000, 1000, 'A la carte', 'restaurante'),
					(None, 'SaporePizzaria', 210, 900, 'Pizza e Batata Recheada', 'restaurante'),
					(None, 'CasaDaMoqueca', 500, 600, 'Melhor A parmegianna do MUNDO','restaurante'),
					(None, 'PadariaRomana', 500, 1000, 'Padaria carissima', 'padaria'),
					(None, 'DiCapri', 500, 950, 'Atendimento de primeira', 'bar'),
                    (None, 'Cinetica', 700, 0, 'Academia mainstream', 'academia'),
					(None, 'GoldenFitness', 500, 700, 'Academia dos playba', 'academia'),
					(None, 'BarDaCoxinha', 40, 1000, 'Especializado em Coxinha', 'bar'),
					(None, 'LaCoxinha', 200, 500, 'Fast-food gordinho', 'fastfood'),
					(None, 'LaSalamandra', 200, 450, 'Mexicano respeitavel','restaurante'),
                    (None, 'BarDaMarcia', 300, 800, 'Butecao', 'bar'),
					(None, 'CasaSaoJorge', 300, 850, 'Bar Dancante', 'bar'),
					(None, 'Atlex', 300, 880, 'Academia Top', 'academia'),
					(None, 'Ruda', 280, 850, 'Bar Dancante','bar')
                    ]
notas = [(None, 1, 5),
		 (None, 2, 5),
		 (None, 3, 5),
		 (None, 4, 5),
		 (None, 5, 5),
		 (None, 6, 5),
		 (None, 7, 5),
         (None, 8, 5),
		 (None, 9, 5),
		 (None, 10, 5),
		 (None, 11, 5),
		 (None, 12, 5),
		 (None, 13, 5),
		 (None, 14, 5),
         (None, 15, 5),
		 (None, 16, 5),
		 (None, 17, 5),
		 (None, 18, 5),
		 (None, 19, 5),
		 (None, 20, 5),
		 (None, 21, 5),
         (None, 22, 5),
		 (None, 23, 5),
		 (None, 24, 5),
		 (None, 25, 5),
         (None, 26, 5),
		 (None, 27, 5),
		 (None, 28, 5),
		 (None, 29, 5),
		 (None, 30, 5)
		 ]


cursor.executemany("INSERT INTO estabelecimentos VALUES (?,?,?,?,?,?)", estabelecimentos)
cursor.executemany("INSERT INTO notas VALUES (?,?,?)", notas)

print("+ database populated")

conn.commit()

print("+ databes commited")
