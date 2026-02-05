# kompilator c
CCOMP = mpicc

# konsolidator
LOADER = mpicc

# program uruchomieniowy MPI
MPI_run = mpiexec -oversubscribe

# opcje optymalizacji:
# wersja do debugowania
OPT = -g -DDEBUG
# wersja zoptymalizowana do mierzenia czasu
# OPT = -O3

# pliki naglowkowe
#INC = -I../pomiar_czasu

# biblioteki
#LIB = -L../pomiar_czasu -lm
LIB = -lm

all: MPI_simple MPI_hostname MPI_relay MPI_packed_struct MPI_pipeline

run: MPI_simple
	$(MPI_run) -np 4 ./MPI_simple

run_hostname: MPI_hostname
	$(MPI_run) -np 4 ./MPI_hostname

run_relay: MPI_relay
	$(MPI_run) -np 5 ./MPI_relay

run_packed: MPI_packed_struct
	$(MPI_run) -np 2 ./MPI_packed_struct

run_pipeline: MPI_pipeline
	$(MPI_run) -np 4 ./MPI_pipeline

# zaleznosci i komendy
MPI_simple : MPI_simple.o
	$(LOADER) $(OPT) MPI_simple.o -o MPI_simple $(LIB)

MPI_hostname : MPI_hostname.o
	$(LOADER) $(OPT) MPI_hostname.o -o MPI_hostname $(LIB)

MPI_relay : MPI_relay.o
	$(LOADER) $(OPT) MPI_relay.o -o MPI_relay $(LIB)

MPI_packed_struct : MPI_packed_struct.o
	$(LOADER) $(OPT) MPI_packed_struct.o -o MPI_packed_struct $(LIB)

MPI_pipeline : MPI_pipeline.o
	$(LOADER) $(OPT) MPI_pipeline.o -o MPI_pipeline $(LIB)

# jak uzyskac plik moj_program.o ?
MPI_simple.o: MPI_simple.c 
	$(CCOMP) -c $(OPT) MPI_simple.c

MPI_hostname.o: MPI_hostname.c 
	$(CCOMP) -c $(OPT) MPI_hostname.c

MPI_relay.o: MPI_relay.c 
	$(CCOMP) -c $(OPT) MPI_relay.c

MPI_packed_struct.o: MPI_packed_struct.c 
	$(CCOMP) -c $(OPT) MPI_packed_struct.c

MPI_pipeline.o: MPI_pipeline.c 
	$(CCOMP) -c $(OPT) MPI_pipeline.c

clean:
	rm -f *.o MPI_simple MPI_hostname MPI_relay MPI_packed_struct MPI_pipeline
