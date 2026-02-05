#!/usr/bin/env python3
"""
Skrypt do generowania wykresów przyspieszenia i efektywności
"""

import matplotlib.pyplot as plt
import numpy as np
import sys

def read_performance_data(filename):
    """Czyta dane z pliku wydajnościowego (mat_vec lub calka)"""
    processes = []
    times = []
    speedups = []
    efficiencies = []
    
    try:
        with open(filename, 'r') as f:
            for line in f:
                line = line.strip()
                # Pomijaj puste linie, komentarze i nagłówki
                if not line or line.startswith('#') or line.startswith('==='):
                    continue
                parts = line.split()
                if len(parts) >= 4:
                    try:
                        processes.append(int(parts[0]))
                        times.append(float(parts[1]))
                        speedups.append(float(parts[2]))
                        efficiencies.append(float(parts[3]))
                    except ValueError:
                        # Pomijaj linie, które nie zawierają liczb
                        continue
    except FileNotFoundError:
        print(f"Plik {filename} nie istnieje. Uruchom najpierw run_performance_tests.sh")
        return None, None, None, None
    
    return processes, times, speedups, efficiencies

def plot_combined_performance(mat_vec_data, calka_data):
    """Tworzy wykresy porównawcze dla mat_vec (MPI) i calka_omp (OpenMP)"""
    
    # Obsługa None values
    if mat_vec_data[0] is None:
        mat_procs, mat_times, mat_speedups, mat_effs = [], [], [], []
    else:
        mat_procs, mat_times, mat_speedups, mat_effs = mat_vec_data
    
    if calka_data[0] is None:
        calka_threads, calka_times, calka_speedups, calka_effs = [], [], [], []
    else:
        calka_threads, calka_times, calka_speedups, calka_effs = calka_data
    
    if not mat_procs and not calka_threads:
        print("Brak danych do wyświetlenia")
        return
    
    # Wykres 1: Czas wykonania
    fig1, ax1 = plt.subplots(figsize=(10, 6))
    if mat_procs:
        ax1.plot(mat_procs, mat_times, 'bo-', linewidth=2, markersize=8, label='mat_vec (MPI)')
    if calka_threads:
        ax1.plot(calka_threads, calka_times, 'rs-', linewidth=2, markersize=8, label='calka_omp (OpenMP)')
    ax1.set_xlabel('Liczba procesów/wątków')
    ax1.set_ylabel('Czas wykonania [s]')
    ax1.set_title('Czas wykonania - porównanie')
    ax1.legend()
    ax1.grid(True)
    if mat_procs:
        ax1.set_xticks(mat_procs)
    elif calka_threads:
        ax1.set_xticks(calka_threads)
    plt.tight_layout()
    plt.savefig('performance_time.png', dpi=300)
    print("Wykres czasu wykonania zapisany w performance_time.png")
    
    # Wykres 2: Przyspieszenie
    fig2, ax2 = plt.subplots(figsize=(10, 6))
    if mat_procs:
        ax2.plot(mat_procs, mat_speedups, 'bo-', linewidth=2, markersize=8, label='mat_vec (MPI) - zmierzony')
        ax2.plot(mat_procs, mat_procs, 'b--', linewidth=1.5, alpha=0.5, label='mat_vec - idealny')
    if calka_threads:
        ax2.plot(calka_threads, calka_speedups, 'rs-', linewidth=2, markersize=8, label='calka_omp (OpenMP) - zmierzony')
        ax2.plot(calka_threads, calka_threads, 'r--', linewidth=1.5, alpha=0.5, label='calka_omp - idealny')
    ax2.set_xlabel('Liczba procesów/wątków')
    ax2.set_ylabel('Przyspieszenie')
    ax2.set_title('Przyspieszenie - porównanie')
    ax2.legend()
    ax2.grid(True)
    if mat_procs:
        ax2.set_xticks(mat_procs)
    elif calka_threads:
        ax2.set_xticks(calka_threads)
    plt.tight_layout()
    plt.savefig('performance_speedup.png', dpi=300)
    print("Wykres przyspieszenia zapisany w performance_speedup.png")
    
    # Wykres 3: Efektywność
    fig3, ax3 = plt.subplots(figsize=(10, 6))
    if mat_procs:
        ax3.plot(mat_procs, mat_effs, 'bo-', linewidth=2, markersize=8, label='mat_vec (MPI) - zmierzony')
    if calka_threads:
        ax3.plot(calka_threads, calka_effs, 'rs-', linewidth=2, markersize=8, label='calka_omp (OpenMP) - zmierzony')
    ax3.axhline(y=1.0, color='g', linestyle='--', linewidth=2, label='Idealny')
    ax3.set_xlabel('Liczba procesów/wątków')
    ax3.set_ylabel('Efektywność')
    ax3.set_title('Efektywność - porównanie')
    ax3.legend()
    ax3.grid(True)
    ax3.set_ylim([0, 1.2])
    if mat_procs:
        ax3.set_xticks(mat_procs)
    elif calka_threads:
        ax3.set_xticks(calka_threads)
    plt.tight_layout()
    plt.savefig('performance_efficiency.png', dpi=300)
    print("Wykres efektywności zapisany w performance_efficiency.png")
    
    # Wykres 4: Wszystkie miary razem (2x2)
    fig4, axes = plt.subplots(2, 2, figsize=(14, 10))
    
    # Czas wykonania
    if mat_procs:
        axes[0, 0].plot(mat_procs, mat_times, 'bo-', linewidth=2, markersize=8, label='mat_vec (MPI)')
    if calka_threads:
        axes[0, 0].plot(calka_threads, calka_times, 'rs-', linewidth=2, markersize=8, label='calka_omp (OpenMP)')
    axes[0, 0].set_xlabel('Liczba procesów/wątków')
    axes[0, 0].set_ylabel('Czas wykonania [s]')
    axes[0, 0].set_title('Czas wykonania')
    axes[0, 0].legend()
    axes[0, 0].grid(True)
    
    # Przyspieszenie
    if mat_procs:
        axes[0, 1].plot(mat_procs, mat_speedups, 'bo-', linewidth=2, markersize=8, label='mat_vec (MPI)')
        axes[0, 1].plot(mat_procs, mat_procs, 'b--', linewidth=1.5, alpha=0.5, label='mat_vec idealny')
    if calka_threads:
        axes[0, 1].plot(calka_threads, calka_speedups, 'rs-', linewidth=2, markersize=8, label='calka_omp (OpenMP)')
        axes[0, 1].plot(calka_threads, calka_threads, 'r--', linewidth=1.5, alpha=0.5, label='calka_omp idealny')
    axes[0, 1].set_xlabel('Liczba procesów/wątków')
    axes[0, 1].set_ylabel('Przyspieszenie')
    axes[0, 1].set_title('Przyspieszenie')
    axes[0, 1].legend()
    axes[0, 1].grid(True)
    
    # Efektywność
    if mat_procs:
        axes[1, 0].plot(mat_procs, mat_effs, 'bo-', linewidth=2, markersize=8, label='mat_vec (MPI)')
    if calka_threads:
        axes[1, 0].plot(calka_threads, calka_effs, 'rs-', linewidth=2, markersize=8, label='calka_omp (OpenMP)')
    axes[1, 0].axhline(y=1.0, color='g', linestyle='--', linewidth=2, label='Idealny')
    axes[1, 0].set_xlabel('Liczba procesów/wątków')
    axes[1, 0].set_ylabel('Efektywność')
    axes[1, 0].set_title('Efektywność')
    axes[1, 0].legend()
    axes[1, 0].grid(True)
    axes[1, 0].set_ylim([0, 1.2])
    
    # Porównanie przyspieszenia i efektywności (mat_vec)
    if mat_procs:
        ax4 = axes[1, 1]
        ax4.plot(mat_procs, mat_speedups, 'bo-', linewidth=2, markersize=8, label='Przyspieszenie mat_vec')
        ax4_twin = ax4.twinx()
        ax4_twin.plot(mat_procs, mat_effs, 'mo-', linewidth=2, markersize=8, label='Efektywność mat_vec')
        ax4.set_xlabel('Liczba procesów')
        ax4.set_ylabel('Przyspieszenie', color='b')
        ax4_twin.set_ylabel('Efektywność', color='m')
        ax4.set_title('mat_vec: Przyspieszenie i efektywność')
        ax4.grid(True)
        ax4.tick_params(axis='y', labelcolor='b')
        ax4_twin.tick_params(axis='y', labelcolor='m')
        # Dodaj legendy
        lines1, labels1 = ax4.get_legend_handles_labels()
        lines2, labels2 = ax4_twin.get_legend_handles_labels()
        ax4.legend(lines1 + lines2, labels1 + labels2, loc='upper left')
    
    plt.tight_layout()
    plt.savefig('performance_combined.png', dpi=300)
    print("Wykresy kombinowane zapisane w performance_combined.png")

if __name__ == '__main__':
    # Wczytaj dane dla mat_vec (MPI)
    mat_vec_data = read_performance_data('performance_mat_vec.dat')
    
    # Wczytaj dane dla calka_omp (OpenMP)
    calka_data = read_performance_data('performance_calka.dat')
    
    # Utwórz wykresy porównawcze
    plot_combined_performance(mat_vec_data, calka_data)
