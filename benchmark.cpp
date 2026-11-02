// Benchmark: tiempo de la multiplicación clásica y de Karatsuba para 2..4096 dígitos.
// Salida CSV por stdout (n,clasica_us,karatsuba_us,razon); el punto de cruce se informa por stderr.
#include <chrono>
#include <iomanip>
#include <iostream>

#include "multiplicacion.hpp"

using namespace bn;
using Reloj = std::chrono::steady_clock;

namespace {

constexpr double MIN_US = 100000;  // cada tanda debe durar al menos 100 ms
constexpr int TANDAS = 5;          // tandas por tamaño; se reporta la más rápida
volatile char sumidero;            // consume los resultados para que no se elimine el cálculo

// Microsegundos por multiplicación: duplica las repeticiones hasta superar MIN_US y promedia.
// Cambia el último dígito de A en cada vuelta para que la llamada no sea invariante del bucle.
template <class Algoritmo>
double microsPorOperacion(Algoritmo multiplicar, Num a, const Num& b) {
    for (size_t reps = 1;; reps *= 2) {
        auto inicio = Reloj::now();
        for (size_t i = 0; i < reps; ++i) {
            a.back() = static_cast<char>('1' + i % 9);
            sumidero = multiplicar(a, b).back();
        }
        double us = std::chrono::duration<double, std::micro>(Reloj::now() - inicio).count();
        if (us >= MIN_US) return us / reps;
    }
}

// Mínimo de TANDAS mediciones: es el estimador menos sensible al ruido del sistema.
template <class Algoritmo>
double mejorTiempo(Algoritmo multiplicar, const Num& a, const Num& b) {
    double mejor = microsPorOperacion(multiplicar, a, b);
    for (int t = 1; t < TANDAS; ++t) mejor = std::min(mejor, microsPorOperacion(multiplicar, a, b));
    return mejor;
}

}  // namespace

int main() {
    std::mt19937 gen(42);
    size_t cruce = 0;
    std::cout << "n,clasica_us,karatsuba_us,razon\n" << std::fixed << std::setprecision(4);
    for (size_t n = 2; n <= 4096; n *= 2) {
        Num a = aleatorio(n, gen), b = aleatorio(n, gen);
        if (karatsuba(a, b) != multiplicacionClasica(a, b)) {
            std::cerr << "Resultados distintos para n=" << n << '\n';
            return 1;
        }
        double clasica = mejorTiempo(multiplicacionClasica, a, b);
        double kara = mejorTiempo(karatsuba, a, b);
        std::cout << n << ',' << clasica << ',' << kara << ',' << clasica / kara << '\n';
        cruce = kara >= clasica ? 0 : (cruce ? cruce : n);
    }
    if (cruce) std::cerr << "Punto de cruce: Karatsuba supera a la clásica desde n = " << cruce << '\n';
    else std::cerr << "Karatsuba no supera a la clásica en el rango medido\n";
}
