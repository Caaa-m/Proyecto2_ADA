"""Genera tiempos.png a partir de tiempos_base1.csv y tiempos_base64.csv (salida de benchmark).

Gráfica en escala de grises: las series se distinguen por marcador y tipo de línea.
"""
import csv

import matplotlib.pyplot as plt


def leer(archivo):
    """Devuelve (n, tiempos clásica, tiempos Karatsuba) de un CSV del benchmark."""
    with open(archivo, newline="") as f:
        filas = list(csv.DictReader(f))
    return ([int(r["n"]) for r in filas],
            [float(r["clasica_us"]) for r in filas],
            [float(r["karatsuba_us"]) for r in filas])


n, clasica, kara1 = leer("tiempos_base1.csv")
kara64 = leer("tiempos_base64.csv")[2]
series = [("Karatsuba (caso base 1)", kara1, dict(marker="s", linestyle="--", color="0.35", mfc="white")),
          ("Karatsuba (caso base 64)", kara64, dict(marker="^", linestyle=":", color="0.0", mfc="0.6"))]

plt.rcParams.update({"font.family": "serif", "font.size": 8})
fig, (tiempo, razon) = plt.subplots(1, 2, figsize=(7.0, 2.9))
tiempo.loglog(n, clasica, marker="o", linestyle="-", color="black", mfc="black", ms=4, label="Clásica Θ(n²)")
for nombre, datos, estilo in series:
    tiempo.loglog(n, datos, ms=4, label=nombre, **estilo)
    razon.semilogx(n, [c / k for c, k in zip(clasica, datos)], ms=4, label=nombre, **estilo)
tiempo.set(xlabel="dígitos (n)", ylabel="tiempo por multiplicación (µs)", title="(a) Tiempo")
razon.axhline(1, color="0.5", linestyle="-", linewidth=0.8)
razon.set(xlabel="dígitos (n)", ylabel="tiempo clásica / tiempo Karatsuba", title="(b) Razón (> 1: gana Karatsuba)",
          yscale="log")
for eje in (tiempo, razon):
    eje.set_xscale("log", base=2)
    eje.grid(True, which="major", color="0.85", linewidth=0.6)
    eje.legend(fontsize=7, frameon=False)
fig.tight_layout()
fig.savefig("tiempos.png", dpi=200)
