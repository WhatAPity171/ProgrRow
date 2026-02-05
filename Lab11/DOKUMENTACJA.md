# Dokumentacja - Komunikacja MPI

## 1. Przesyłanie i odbieranie komunikatów (punkt-punkt)

Komunikacja punkt-punkt (point-to-point) w MPI to podstawowy mechanizm wymiany danych między dwoma procesami. Charakteryzuje się następującymi cechami:

### Charakterystyka komunikacji punkt-punkt:
- **Synchroniczność**: Operacje mogą być synchroniczne (blokujące) lub asynchroniczne (nieblokujące)
- **Kierunkowość**: Komunikat jest wysyłany z procesu źródłowego do procesu docelowego
- **Tagowanie**: Każdy komunikat może mieć tag, który pozwala na rozróżnienie różnych typów komunikatów
- **Buforowanie**: System MPI może buforować komunikaty, ale nie jest to gwarantowane

### Przykład komunikacji punkt-punkt:
```c
// Proces wysyłający
MPI_Send(&data, count, MPI_INT, dest, tag, MPI_COMM_WORLD);

// Proces odbierający
MPI_Recv(&data, count, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
```

## 2. Send, ISend, Recv, IRecv - Szczegółowa dyskusja

### MPI_Send (Synchroniczne wysyłanie)

**Charakterystyka:**
- Operacja **blokująca** - funkcja nie zwraca kontroli dopóki komunikat nie zostanie bezpiecznie wysłany
- Może być **synchroniczna** (czeka na odbiór) lub **buforowana** (zapisuje do bufora systemowego)
- Gwarantuje, że dane są bezpieczne przed powrotem z funkcji

**Prototyp:**
```c
int MPI_Send(void *buf, int count, MPI_Datatype datatype, 
             int dest, int tag, MPI_Comm comm);
```

**Zastosowanie:**
- Gdy potrzebujemy pewności, że dane zostały wysłane
- W prostych scenariuszach komunikacji
- Gdy nie potrzebujemy asynchroniczności

**Przykład:**
```c
int data = 42;
MPI_Send(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
```

### MPI_ISend (Asynchroniczne wysyłanie)

**Charakterystyka:**
- Operacja **nieblokująca** - funkcja zwraca natychmiast, nie czekając na wysłanie
- Zwraca **request handle**, który można użyć do sprawdzenia statusu
- Pozwala na **nakładanie komunikacji z obliczeniami** (overlap)

**Prototyp:**
```c
int MPI_ISend(void *buf, int count, MPI_Datatype datatype,
              int dest, int tag, MPI_Comm comm, MPI_Request *request);
```

**Zastosowanie:**
- Gdy chcemy nakładać komunikację z obliczeniami
- W scenariuszach, gdzie proces może wykonywać inne operacje podczas wysyłania
- Do optymalizacji wydajności

**Przykład:**
```c
int data = 42;
MPI_Request request;
MPI_ISend(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);
// Możemy wykonywać inne operacje...
MPI_Wait(&request, MPI_STATUS_IGNORE); // Czekamy na zakończenie
```

### MPI_Recv (Synchroniczne odbieranie)

**Charakterystyka:**
- Operacja **blokująca** - funkcja czeka na nadejście komunikatu
- Może używać **wildcards**: `MPI_ANY_SOURCE`, `MPI_ANY_TAG`
- Zwraca informacje o komunikacie w strukturze `MPI_Status`

**Prototyp:**
```c
int MPI_Recv(void *buf, int count, MPI_Datatype datatype,
             int source, int tag, MPI_Comm comm, MPI_Status *status);
```

**Zastosowanie:**
- Gdy potrzebujemy natychmiast otrzymać dane
- W prostych scenariuszach komunikacji
- Gdy nie potrzebujemy asynchroniczności

**Przykład:**
```c
int data;
MPI_Status status;
MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, 
         MPI_COMM_WORLD, &status);
printf("Otrzymano od procesu %d\n", status.MPI_SOURCE);
```

### MPI_IRecv (Asynchroniczne odbieranie)

**Charakterystyka:**
- Operacja **nieblokująca** - funkcja zwraca natychmiast
- Zwraca **request handle** do sprawdzenia statusu
- Pozwala na **nakładanie komunikacji z obliczeniami**

**Prototyp:**
```c
int MPI_IRecv(void *buf, int count, MPI_Datatype datatype,
              int source, int tag, MPI_Comm comm, MPI_Request *request);
```

**Zastosowanie:**
- Gdy chcemy nakładać komunikację z obliczeniami
- W scenariuszach, gdzie proces może wykonywać inne operacje podczas oczekiwania
- Do optymalizacji wydajności

**Przykład:**
```c
int data;
MPI_Request request;
MPI_IRecv(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
// Możemy wykonywać inne operacje...
MPI_Wait(&request, &status); // Czekamy na otrzymanie
```

### Porównanie operacji synchronicznych i asynchronicznych

| Cecha | Synchroniczne (Send/Recv) | Asynchroniczne (ISend/IRecv) |
|-------|---------------------------|------------------------------|
| Blokowanie | Tak | Nie |
| Zwracanie request | Nie | Tak |
| Nakładanie z obliczeniami | Nie | Tak |
| Złożoność | Prostsza | Bardziej złożona |
| Wydajność | Niższa | Wyższa (przy overlap) |

## 3. Działanie aplikacji z pamięcią dzieloną

### Pamięć dzielona vs. komunikacja MPI

**MPI (Message Passing Interface):**
- Każdy proces ma **własną przestrzeń adresową**
- Komunikacja odbywa się przez **wysyłanie i odbieranie komunikatów**
- Procesy mogą działać na **różnych maszynach** (distributed memory)
- **Brak bezpośredniego dostępu** do pamięci innych procesów

**Pamięć dzielona (Shared Memory):**
- Wiele procesów/wątków ma dostęp do **wspólnej przestrzeni pamięci**
- Komunikacja przez **bezpośredni dostęp do pamięci**
- Procesy muszą działać na **tej samej maszynie**
- Wymaga **synchronizacji** (semafory, mutexy, bariery)

### Kiedy używać pamięci dzielonej?

**Zalety:**
- Szybsza komunikacja (brak kopiowania danych)
- Prostszy model programowania dla niektórych algorytmów
- Efektywne dla małych danych

**Wady:**
- Ograniczone do jednej maszyny
- Wymaga synchronizacji (race conditions, deadlocks)
- Trudniejsze debugowanie

### Kiedy używać MPI?

**Zalety:**
- Działa na wielu maszynach (klaster, superkomputer)
- Skalowalność do tysięcy procesów
- Jasny model komunikacji
- Niezależne przestrzenie adresowe (bezpieczeństwo)

**Wady:**
- Overhead komunikacji (kopiowanie danych)
- Bardziej złożony kod dla niektórych algorytmów

### Hybrydowe podejście

Współczesne systemy HPC często używają **hybrydowego podejścia**:
- **MPI między węzłami** (komunikacja sieciowa)
- **OpenMP/pamięć dzielona wewnątrz węzła** (wielowątkowość)

### Przykład różnicy:

**MPI (Message Passing):**
```c
// Proces 0
int data = 42;
MPI_Send(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

// Proces 1
int received;
MPI_Recv(&received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
```

**Pamięć dzielona (Shared Memory):**
```c
// Wspólna pamięć
int *shared_data = mmap(...);

// Proces 0
*shared_data = 42;

// Proces 1
int value = *shared_data; // Bezpośredni dostęp
```

## 4. MPI_PACKED - Przesyłanie struktur

### Problem z przesyłaniem struktur

Struktury w C mogą mieć **padding** (wyrównanie bajtów), co powoduje, że:
- Rozmiar struktury może być większy niż suma pól
- Układ pamięci może różnić się między platformami
- Bezpośrednie przesyłanie struktury może być nieprzenośne

### Rozwiązanie: MPI_PACKED

`MPI_PACKED` pozwala na:
- **Pakowanie** poszczególnych pól struktury do bufora
- **Przesyłanie** pakowanych danych jako ciągły strumień bajtów
- **Rozpakowywanie** danych w procesie odbierającym

### Funkcje:
- `MPI_Pack()` - pakuje dane do bufora
- `MPI_Unpack()` - rozpakowuje dane z bufora
- `MPI_Pack_size()` - oblicza rozmiar potrzebnego bufora

### Przykład użycia:

```c
// Pakowanie
position = 0;
MPI_Pack(&struct.id, 1, MPI_INT, buffer, size, &position, comm);
MPI_Pack(&struct.value, 1, MPI_FLOAT, buffer, size, &position, comm);
MPI_Pack(struct.name, len, MPI_CHAR, buffer, size, &position, comm);

// Wysyłanie
MPI_Send(buffer, position, MPI_PACKED, dest, tag, comm);

// Odbieranie i rozpakowywanie
MPI_Recv(buffer, size, MPI_PACKED, source, tag, comm, &status);
position = 0;
MPI_Unpack(buffer, size, &position, &struct.id, 1, MPI_INT, comm);
MPI_Unpack(buffer, size, &position, &struct.value, 1, MPI_FLOAT, comm);
MPI_Unpack(buffer, size, &position, struct.name, len, MPI_CHAR, comm);
```

## 5. Przetwarzanie potokowe (Pipeline Processing) - SPMD

### SPMD (Single Program Multiple Data)

**Definicja:**
- Jeden program wykonywany przez wiele procesów
- Każdy proces działa na różnych danych
- Procesy mogą wykonywać różne części kodu w zależności od ranku

### Przetwarzanie potokowe (Pipeline)

**Charakterystyka:**
- Dane przepływają przez szereg procesów
- Każdy proces wykonuje **własne operacje** na danych
- Dane są przekazywane sekwencyjnie: P0 → P1 → P2 → ... → Pn

**Zastosowania:**
- Przetwarzanie sygnałów
- Filtrowanie danych
- Transformacje sekwencyjne
- Przetwarzanie obrazów

### Przykład potoku:

```
Proces 0: Inicjalizacja → Operacja A → Przekazanie
Proces 1: Odbieranie → Operacja B → Przekazanie
Proces 2: Odbieranie → Operacja C → Przekazanie
...
Proces N: Odbieranie → Operacja Finalna → Koniec
```

### Zalety:
- **Równoległość**: Różne etapy mogą być wykonywane jednocześnie
- **Modularność**: Każdy proces ma jasno określone zadanie
- **Skalowalność**: Można dodać więcej etapów

### Wady:
- **Latencja**: Cały potok musi przejść przez wszystkie etapy
- **Nierównomierne obciążenie**: Jeśli jeden etap jest wolniejszy, spowalnia cały potok
