// Enteros no negativos de tamaño arbitrario y operaciones auxiliares de la práctica.
//
// Representación: un número es un std::string de dígitos decimales ('0'..'9'),
// con el dígito MÁS significativo primero ("1234" representa mil doscientos treinta y cuatro).
// Las operaciones nativas (int) se usan solo sobre dígitos individuales y acarreos.
#pragma once

#include <algorithm>
#include <random>
#include <string>
#include <utility>

namespace bn {

using Num = std::string;

// Elimina los ceros a la izquierda; el cero se representa como "0".
inline Num normalizar(Num a) {
    size_t i = a.find_first_not_of('0');
    if (i == Num::npos) return "0";
    return a.substr(i);
}

// Rellena con ceros a la izquierda hasta tener 'longitud' dígitos (no recorta si ya es más largo).
inline Num rellenar(const Num& a, size_t longitud) {
    if (a.size() >= longitud) return a;
    return Num(longitud - a.size(), '0') + a;
}

// A + B. Admite longitudes distintas; el resultado puede tener un dígito más que el operando mayor.
inline Num sumar(const Num& a, const Num& b) {
    Num r(std::max(a.size(), b.size()) + 1, '0');
    int acarreo = 0;
    size_t i = a.size(), j = b.size(), k = r.size();
    while (k > 0) {
        int s = acarreo;
        if (i > 0) s += a[--i] - '0';
        if (j > 0) s += b[--j] - '0';
        r[--k] = static_cast<char>('0' + s % 10);
        acarreo = s / 10;
    }
    return normalizar(r);
}

// A - B con la precondición A >= B. El resultado se normaliza (sin ceros a la izquierda).
inline Num restar(const Num& a, const Num& b) {
    Num r = a;
    int prestamo = 0;
    size_t j = b.size();
    for (size_t i = r.size(); i > 0; --i) {
        int d = (r[i - 1] - '0') - prestamo - (j > 0 ? b[--j] - '0' : 0);
        prestamo = d < 0;
        if (d < 0) d += 10;
        r[i - 1] = static_cast<char>('0' + d);
    }
    return normalizar(r);
}

// A · 10^k: añade k ceros a la derecha (el cero sigue siendo "0").
inline Num desplazar(const Num& a, size_t k) {
    if (normalizar(a) == "0") return "0";
    return a + Num(k, '0');
}

// Separa A en {alta, baja}, donde baja son los m dígitos menos significativos: A = alta·10^m + baja.
inline std::pair<Num, Num> partir(const Num& a, size_t m) {
    Num ext = rellenar(a, m + 1);
    size_t corte = ext.size() - m;
    return {ext.substr(0, corte), ext.substr(corte)};
}

// Número aleatorio de exactamente n dígitos (el primero distinto de cero), para pruebas y benchmark.
template <class Generador>
Num aleatorio(size_t n, Generador& gen) {
    std::uniform_int_distribution<int> dig(0, 9), primero(1, 9);
    Num r(n, '0');
    r[0] = static_cast<char>('0' + primero(gen));
    for (size_t i = 1; i < n; ++i) r[i] = static_cast<char>('0' + dig(gen));
    return r;
}

}  // namespace bn
