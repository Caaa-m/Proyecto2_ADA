// Validación de la multiplicación clásica y de Karatsuba.
// Karatsuba se compara contra la clásica y ambas contra referencias independientes
// (aritmética nativa para números pequeños y una forma cerrada para (10^k - 1)^2).
#include <iostream>
#include <random>
#include <string>

#include "multiplicacion.hpp"

using namespace bn;

namespace {

long total = 0, fallos = 0;

void comprobar(bool ok, const std::string& caso) {
    ++total;
    if (!ok) {
        ++fallos;
        if (fallos <= 20) std::cerr << "FALLO: " << caso << '\n';
    }
}

// Karatsuba y clásica deben dar 'esperado' (si se conoce) y coincidir entre sí.
void comprobarProducto(const Num& a, const Num& b, const Num& esperado = "") {
    Num k = karatsuba(a, b), c = multiplicacionClasica(a, b);
    std::string caso = a.substr(0, 40) + " * " + b.substr(0, 40);
    comprobar(k == c, "karatsuba != clasica: " + caso);
    if (!esperado.empty()) comprobar(k == esperado, "resultado incorrecto: " + caso);
}

void pruebasAuxiliares() {
    comprobar(sumar("999", "1") == "1000", "sumar con acarreo final");
    comprobar(sumar("5", "12345") == "12350", "sumar longitudes distintas");
    comprobar(sumar("0", "0") == "0", "sumar ceros");
    comprobar(restar("1000", "1") == "999", "restar con préstamos");
    comprobar(restar("12345", "12345") == "0", "restar iguales");
    comprobar(restar("00500", "7") == "493", "restar con ceros a la izquierda");
    comprobar(desplazar("12", 3) == "12000", "desplazar");
    comprobar(desplazar("0", 5) == "0", "desplazar cero");
    auto [alta, baja] = partir("123456", 2);
    comprobar(alta == "1234" && baja == "56", "partir");
    comprobar(normalizar("000") == "0" && normalizar("0012") == "12", "normalizar");
    comprobar(rellenar("7", 4) == "0007", "rellenar");
}

void casosDeBorde() {
    for (int a = 0; a <= 9; ++a)  // un dígito
        for (int b = 0; b <= 9; ++b) comprobarProducto(std::to_string(a), std::to_string(b), std::to_string(a * b));
    comprobarProducto("0000000123", "00456", "56088");                 // ceros a la izquierda
    comprobarProducto("0000", "000000", "0");                          // ceros puros
    comprobarProducto("1000000001", "200030004", "200030004200030004");  // ceros intercalados
    comprobarProducto("100200300", "400500600", "40130280270180000");
    comprobarProducto("12345", "678", "8369910");                       // longitudes impares
    comprobarProducto("1234567", "7654321", "9449772114007");
    comprobarProducto("9", "123456789012345678901234567890", "1111111101111111110111111111010");  // muy distintas
    comprobarProducto("1", "98765432109876543210", "98765432109876543210");
    comprobarProducto("0", "98765432109876543210", "0");
}

void exhaustivoPequenos() {  // todos los pares 0..499 contra la aritmética nativa (solo como oráculo)
    for (int a = 0; a < 500; ++a)
        for (int b = 0; b < 500; ++b)
            comprobar(karatsuba(std::to_string(a), std::to_string(b)) == std::to_string(a * b),
                      std::to_string(a) + " * " + std::to_string(b));
}

void formaCerrada() {  // (10^k - 1)^2 = 9...9 8 0...0 1 (k-1 nueves y k-1 ceros)
    for (size_t k = 1; k <= 1000; ++k) {
        Num nueves(k, '9');
        Num esperado = Num(k - 1, '9') + "8" + Num(k - 1, '0') + "1";
        comprobar(karatsuba(nueves, nueves) == esperado, "(10^" + std::to_string(k) + " - 1)^2 karatsuba");
        comprobar(multiplicacionClasica(nueves, nueves) == esperado, "(10^" + std::to_string(k) + " - 1)^2 clasica");
    }
}

void aleatorios() {
    std::mt19937 gen(2026);
    std::uniform_int_distribution<int> longitud(1, 300);
    for (int i = 0; i < 300; ++i) comprobarProducto(aleatorio(longitud(gen), gen), aleatorio(longitud(gen), gen));
    comprobarProducto(aleatorio(512, gen), aleatorio(512, gen));  // productos grandes
    comprobarProducto(aleatorio(1000, gen), aleatorio(777, gen));
    comprobarProducto(aleatorio(2000, gen), aleatorio(2000, gen));
}

}  // namespace

int main() {
    pruebasAuxiliares();
    casosDeBorde();
    exhaustivoPequenos();
    formaCerrada();
    aleatorios();
    std::cout << "UMBRAL_BASE = " << UMBRAL_BASE << ": " << total - fallos << " de " << total
              << " comprobaciones correctas\n";
    return fallos == 0 ? 0 : 1;
}
