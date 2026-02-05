# Laboratorium 11 - Programy MPI

## Opis programów

### 1. MPI_simple.c
Podstawowy program demonstracyjny komunikacji MPI - procesy wysyłają swoje rangi do procesu głównego.

### 2. MPI_hostname.c
Program do wysyłania nazwy hosta. Jeden proces (rank 0) zbiera informacje od innych procesów, które wysyłają swoje nazwy hostów.

**Uruchomienie:**
```bash
make run_hostname
# lub
mpiexec -np 4 ./MPI_hostname
```

### 3. MPI_relay.c
Program sztafeta - podział dostępnych procesów na 3 grupy:
- **Proces początkowy** (rank 0): inicjuje wiadomość
- **Procesy środkowe** (ranki 1 do size-2): przekazują i modyfikują wiadomość
- **Proces końcowy** (rank size-1): odbiera finalną wiadomość

**Uruchomienie:**
```bash
make run_relay
# lub
mpiexec -np 5 ./MPI_relay
```

### 4. MPI_packed_struct.c
Program demonstrujący przesyłanie struktury między procesami z wykorzystaniem `MPI_PACKED`. 
Struktura zawiera: int, float, char[].

**Uruchomienie:**
```bash
make run_packed
# lub
mpiexec -np 2 ./MPI_packed_struct
```

### 5. MPI_pipeline.c
Program realizujący mechanizm przetwarzania potokowego (SPMD - Single Program Multiple Data). 
Każdy proces wykonuje własne operacje na strukturze, która przepływa przez potok procesów.

**Uruchomienie:**
```bash
make run_pipeline
# lub
mpiexec -np 4 ./MPI_pipeline
```

## Kompilacja

### Kompilacja wszystkich programów:
```bash
make all
```

### Kompilacja pojedynczego programu:
```bash
make MPI_hostname
make MPI_relay
make MPI_packed_struct
make MPI_pipeline
```

### Uruchomienie programów:
```bash
make run          # MPI_simple
make run_hostname  # MPI_hostname
make run_relay     # MPI_relay
make run_packed    # MPI_packed_struct
make run_pipeline  # MPI_pipeline
```

### Czyszczenie:
```bash
make clean
```

## Dokumentacja

Szczegółowa dokumentacja znajduje się w pliku `DOKUMENTACJA.md` i zawiera:
- Dyskusję na temat przesyłania i odbierania komunikatów (punkt-punkt)
- Szczegółową dyskusję na temat Send, ISend, Recv, IRecv
- Dyskusję na temat działania aplikacji z pamięcią dzieloną
- Opis użycia MPI_PACKED do przesyłania struktur
- Opis przetwarzania potokowego (SPMD)

## Wymagania

- OpenMPI (lub inna implementacja MPI)
- Kompilator C (gcc)
- System Linux/Unix

## Struktura projektu

```
Lab11/
├── Makefile                 # Plik makefile do kompilacji
├── MPI_simple.c            # Podstawowy program MPI
├── MPI_hostname.c          # Program wysyłania nazwy hosta
├── MPI_relay.c             # Program sztafeta
├── MPI_packed_struct.c     # Przesyłanie struktury (MPI_PACKED)
├── MPI_pipeline.c          # Przetwarzanie potokowe (SPMD)
├── DOKUMENTACJA.md         # Szczegółowa dokumentacja
└── README.md               # Ten plik
```
