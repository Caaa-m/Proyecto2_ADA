// Multiplicación clásica Θ(n²) y algoritmo de Karatsuba Θ(n^log2(3)).
#pragma once

#include <vector>

#include "bignum.hpp"

// Caso base de Karatsuba: si los operandos tienen UMBRAL_BASE dígitos o menos se multiplican
// directamente. 1 = Karatsuba puro. Se puede cambiar al compilar: -DUMBRAL_BASE=64
#ifndef UMBRAL_BASE
#define UMBRAL_BASE 1
#endif

namespace bn {

// Método escolar: acumula el producto de cada par de dígitos en su columna y luego
// propaga los acarreos en una sola pasada. Θ(|A|·|B|).
inline Num multiplicacionClasica(const Num& a, const Num& b) {
    size_t n = a.size(), m = b.size();
    std::vector<int> col(n + m, 0);  // col[k]: suma de productos que caen en la posición k (desde la derecha)
    for (size_t i = 0; i < n; ++i) {
        int da = a[n - 1 - i] - '0';
        if (da == 0) continue;
        for (size_t j = 0; j < m; ++j) col[i + j] += da * (b[m - 1 - j] - '0');
    }
    Num r(n + m, '0');
    int acarreo = 0;
    for (size_t k = 0; k < n + m; ++k) {
        int v = col[k] + acarreo;
        r[n + m - 1 - k] = static_cast<char>('0' + v % 10);
        acarreo = v / 10;
    }
    return normalizar(r);
}

namespace detalle {

// Núcleo recursivo. Los operandos pueden llegar con longitudes distintas (las sumas de P3
// pueden tener m+1 dígitos), así que primero se igualan con ceros a la izquierda.
inline Num karatsubaRec(Num x, Num y) {
    size_t n = std::max(x.size(), y.size());
    x = rellenar(x, n);
    y = rellenar(y, n);
    if (n <= static_cast<size_t>(UMBRAL_BASE)) return multiplicacionClasica(x, y);  // caso base

    size_t m = n / 2;  // la mitad baja tiene m dígitos
    auto [x1, x0] = partir(x, m);
    auto [y1, y0] = partir(y, m);

    Num p1 = karatsubaRec(x1, y1);                          // x1·y1
    Num p2 = karatsubaRec(x0, y0);                          // x0·y0
    Num p3 = karatsubaRec(sumar(x1, x0), sumar(y1, y0));    // (x1+x0)(y1+y0)
    Num central = restar(restar(p3, p1), p2);               // (P3 - P1) - P2: nunca negativo

    // x·y = P1·10^(2m) + (P3 - P1 - P2)·10^m + P2
    return sumar(sumar(desplazar(p1, 2 * m), desplazar(central, m)), p2);
}

}  // namespace detalle

// Karatsuba: normaliza, rellena ambos factores hasta la misma potencia de 2 y llama al núcleo.
inline Num karatsuba(const Num& a, const Num& b) {
    Num x = normalizar(a), y = normalizar(b);
    size_t potencia = 1;
    while (potencia < std::max(x.size(), y.size())) potencia *= 2;
    return normalizar(detalle::karatsubaRec(rellenar(x, potencia), rellenar(y, potencia)));
}

}  // namespace bn
