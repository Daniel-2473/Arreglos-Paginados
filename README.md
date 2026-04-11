# Arreglos-Paginados

A continuación se presentan las instrucciones para compilar y ejecutar cada programa.

# Requisitos
- g++ (GCC)
- Sistema Linux o Windows con MinGW/WSL

## Notas para Windows

- Si utiliza Windows, puede compilar usando MinGW o WSL.
- El ejecutable generado será `sorter.exe` o `generator.exe`.
- Se recomienda usar rutas con "/" en lugar de "\". Ya que Windows utiliza "\", no "/" como Linux.

- Para ejecutar:

.\sorter.exe -input ... 

## Compilar Generator

En la carpeta donde se encuentren los archivos ejecutar en terminal:
```bash
g++ -O3 main.cpp -o generator
```
## Compilar Sorter

En la carpeta donde se encuentren los archivos ejecutar en terminal:
```bash
g++ -O3 main.cpp PagedArray.cpp -o sorter
```
## Ejecutar Generator

En la carpeta donde se generó el ejecutable ejecutar en terminal:
```bash
./generator -size <SIZE> -output <OUTPUT FILE PATH>
```
<SIZE> puede escoger: "SMALL", "MEDIUM" o "LARGE"
<OUTPUT FILE PATH> puede ser cualquier directorio válido.

## Ejecutar Sorter

En la carpeta donde se generó el ejecutable ejecutar en terminal:
```bash
./sorter -input <INPUT FILE PATH> -output <OUTPUT FILE PATH> -alg
<ALGORITMO> -pageSize <PAGE-SIZE> -pageCount <PAGE-COUNT>
```
<INPUT FILE PATH> y <OUTPUT FILE PATH> pueden ser cualquier directorio válido, siempre que especifiquen el archivo a utilizar.

<ALGORITMO> puede escoger: "QUICK", "MERGE", "INTRO", "HEAP" o "SHELL"

<PAGE-SIZE> y <PAGE-COUNT> pueden ser cualquier número entero positivo.

Se recomienda ejecutar el programa utilizando QUICK, MERGE o INTRO, estos algoritmos son los que mejor tiempo de ejecución poseen.

### Cambiar el algoritmo de reemplazo

El programa utiliza como algoritmo de reemplazo predeterminado LRU, esto puede cambiar al usar el siguiente comando en terminal:
```bash
./sorter -input <INPUT FILE PATH> -output <OUTPUT FILE PATH> -alg <ALGORITMO> -pageSize <PAGE-SIZE> -pageCount <PAGE-COUNT> -algRemp <Algoritmo-Reemplazo>
```
<Algoritmo-Reemplazo> puede ser "FIFO" o "LRU".

# Pruebas

## Ejecutar .sh

Ejecutar el siguiente comando en la carpeta tests en terminal:
```bash
./pruebas_automaticas.sh
```
## Compilar archivo de prueba de algoritmos en memoria

Ejecutar el siguiente comando en la carpeta tests en terminal:
```bash
g++ -O3 main.cpp -o pruebas
```
## Ejecutar archivo de prueba de algoritmos en memoria

Ejecutar el siguiente comando en la carpeta donde se generó el ejecutable en terminal:
```bash
./pruebas
```
