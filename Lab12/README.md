# Lab12 - Implementacja wymagań MPI

## Wykonane zadania

### 3.0 punkty

#### 1. Obliczanie liczby PI z wykorzystaniem broadcast i reduce
- **Plik**: `oblicz_PI.c`
- **Zmiany**:
  - Dodano MPI do programu obliczającego PI metodą Leibniza
  - Użyto `MPI_Bcast` do rozesłania liczby wyrazów do wszystkich procesów
  - Użyto `MPI_Reduce` do sumowania częściowych wyników z wszystkich procesów
  - Każdy proces oblicza część szeregu, a następnie wyniki są redukowane

#### 2. Pomiary wydajności
- **Programy**: `mat_vec_row_MPI.c` i `oblicz_PI.c`
- **Funkcjonalność**:
  - Programy mierzą czas wykonania dla różnych liczb procesów (1, 2, 4, 8)
  - Obliczają przyspieszenie (speedup) i efektywność (efficiency)
  - Dane wyjściowe są formatowane dla łatwego parsowania

#### 3. Obliczanie miar wydajności
- **Przyspieszenie (Speedup)**: S(p) = T(1) / T(p)
  - gdzie T(1) to czas sekwencyjny, T(p) to czas z p procesami
- **Efektywność (Efficiency)**: E(p) = S(p) / p
  - miara wykorzystania procesorów

#### 4. Wykresy wydajności
- **Skrypt**: `plot_performance.py`
- **Funkcjonalność**:
  - Generuje wykresy czasu wykonania, przyspieszenia i efektywności
  - Porównuje zmierzone wartości z idealnymi (liniowe przyspieszenie)
  - Tworzy pliki PNG: `performance_mat_vec.png` i `performance_pi.png`

### 4.5 punktów

#### Modyfikacja programu mnożenia macierz-wektor z komunikacją grupową
- **Plik**: `mat_vec_row_MPI.c`
- **Zmiany**:
  - Zastąpiono komunikację point-to-point (`MPI_Send`/`MPI_Recv`) komunikacją grupową:
    - `MPI_Scatterv` - rozesłanie wierszy macierzy do procesów
    - `MPI_Bcast` - rozesłanie wektora x do wszystkich procesów
    - `MPI_Gatherv` - zebranie wyników z procesów
  - Zaimplementowano dekompozycję kolumnową z użyciem `MPI_Reduce`

### 5.0 punktów

#### Wykorzystanie MPI_IN_PLACE
- **Lokalizacje**:
  1. **Allgather** (linia ~136): Użyto `MPI_IN_PLACE` dla rank 0, gdzie dane są już na miejscu
  2. **Reduce** (linia ~280): Użyto `MPI_IN_PLACE` dla rank 0 w dekompozycji kolumnowej, gdzie wynik jest zapisywany bezpośrednio w buforze wejściowym

## Uruchomienie

### Kompilacja
```bash
make
```

### Uruchomienie programów

#### Obliczanie PI
```bash
echo "10000000" | mpiexec -np 4 --oversubscribe ./oblicz_PI
```

#### Mnożenie macierz-wektor
```bash
mpiexec -np 4 --oversubscribe ./mat_vec_row_MPI [liczba_wątków]
```

### Testy wydajnościowe

1. Uruchom testy dla różnych liczb procesów:
```bash
./run_performance_tests.sh
```

2. Wygeneruj wykresy:
```bash
python3 plot_performance.py
```

Wymagane biblioteki Python:
- matplotlib
- numpy

## Struktura plików

- `oblicz_PI.c` - program obliczający PI z użyciem MPI (broadcast + reduce)
- `mat_vec_row_MPI.c` - program mnożenia macierz-wektor z komunikacją grupową
- `Makefile` - plik make do kompilacji
- `run_performance_tests.sh` - skrypt do uruchamiania testów wydajnościowych
- `plot_performance.py` - skrypt Python do generowania wykresów
- `performance_mat_vec.dat` - dane wydajnościowe dla mat_vec (generowane przez skrypt)
- `performance_pi.dat` - dane wydajnościowe dla oblicz_PI (generowane przez skrypt)

## Uwagi techniczne

1. **MPI_IN_PLACE**: Może być używany tylko w określonych kontekstach:
   - W `MPI_Reduce` - tylko dla procesu root (rank 0)
   - W `MPI_Allgather` - gdy bufor wejściowy i wyjściowy się pokrywają

2. **Dekompozycja kolumnowa**: Wymaga bardziej złożonej komunikacji, ponieważ macierz jest przechowywana w formacie row-major.

3. **OpenMP**: Program `mat_vec_row_MPI.c` wspiera równoległość OpenMP w funkcji `mat_vec` (można przekazać liczbę wątków jako argument).

## Format danych wydajnościowych

Programy wypisują dane w formacie:
```
#PERF_DATA: [liczba_procesów] [czas] [przyspieszenie] [efektywność]
```

Skrypt `run_performance_tests.sh` zbiera te dane do plików `.dat` dla późniejszej analizy.
