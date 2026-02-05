# Nazwa kompilatora C
CC = gcc

# Flagi kompilacji
# -Wall: włącza wszystkie ostrzeżenia (dobra praktyka)
# -Wextra: włącza dodatkowe ostrzeżenia
# -fopenmp: włącza wsparcie dla OpenMP
# -O3: wysoki poziom optymalizacji (często używany w kodzie równoległym)
# -g: dodaje informacje debugowania (opcjonalne, przydatne przy debugowaniu)
CFLAGS = -Wall -Wextra -fopenmp -O3 -g

# Flagi dla linkera (matematyczna biblioteka)
LDFLAGS = -lm

# Nazwa pliku źródłowego
SRC = plik3.c

# Nazwa pliku wykonywalnego
EXEC = plik3

# Cel domyślny - buduje program
all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC) $(LDFLAGS)

# Cel 'clean' - usuwa plik wykonywalny i wszelkie inne generowane pliki
clean:
	rm -f $(EXEC)