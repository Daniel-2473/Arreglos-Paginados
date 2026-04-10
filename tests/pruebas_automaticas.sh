#!/bin/bash

GENERATOR="" #Ruta del programa
SORTER=""#Ruta del programa
RESULTS_DIR="results" #Ruta donde guardar los resultados
DATA_DIR="test_data" #Ruta donde guardar los archivos que se crearon

mkdir -p "$RESULTS_DIR" #Crear directorios de resultados y archivos creados
mkdir -p "$DATA_DIR"

REPS=3 #Número de repeticiones

declare -A SIZE_LABELS
SIZE_LABELS=( ["SMALL"]="32mb" ["MEDIUM"]="64mb" ["LARGE"]="128mb" )
SIZES=("SMALL" "MEDIUM" "LARGE")

generate_files() { #Generar archivos con sorter
    echo "Generando archivos de prueba"
    for size in "${SIZES[@]}"; do
        echo "-> Generando ${SIZE_LABELS[$size]}..."
        "$GENERATOR" -size "$size" -output "$DATA_DIR/input_${SIZE_LABELS[$size]}.bin"
    done
    echo "Archivos generados."
}

run_test() { #Ejecuta una sola prueba, es decir, el programa con ciertos parametros
    local alg=$1 #Parametros
    local rep=$2
    local size=$3
    local label=${SIZE_LABELS[$size]}
    local pageSize=$4
    local pageCount=$5 #Parametros

    local total_time=0 #Iniciar variables
    local total_faults=0
    local total_hits=0 

    for i in $(seq 1 $REPS); do
        local output="$DATA_DIR/sorted_${alg}_${rep}_${label}_ps${pageSize}_pc${pageCount}_rep${i}.bin" #Archivo de salida
        local result 
        result=$("$SORTER" \ #Ejecutar el programa
            -input "$DATA_DIR/input_${label}.bin" \
            -output "$output" \
            -alg "$alg" \
            -pageSize "$pageSize" \
            -pageCount "$pageCount" \
            -algRemp "$rep" 2>&1)

        local time=$(echo "$result"   | grep "Tiempo transcurrido:" | grep -oP '[\d.]+(?=s)') #Obtiene los datos que imprime el programa
        local faults=$(echo "$result" | grep "Page faults:"         | grep -oP '\d+$')
        local hits=$(echo "$result"   | grep "Page hits:"           | grep -oP '\d+$')

        total_time=$(echo "$total_time + $time" | bc)  #Acomular resultados para luego sacar el promedio
        total_faults=$(echo "$total_faults + $faults" | bc)
        total_hits=$(echo "$total_hits + $hits" | bc)

        # Elimina el archivo sorted para ahorrar espacio
        rm -f "$output"
    done

    local avg_time=$(echo "scale=4; $total_time / $REPS" | bc) #Calcular el promedio
    local avg_faults=$(echo "$total_faults / $REPS" | bc)
    local avg_hits=$(echo "$total_hits / $REPS" | bc)

    echo "$alg,$rep,$label,$pageSize,$pageCount,$avg_time,$avg_faults,$avg_hits" >> "$RESULTS_DIR/summary.csv" #Guardar en el archivo de salida
    echo "   -> Tiempo promedio: ${avg_time}s | Faults: $avg_faults | Hits: $avg_hits" #Mostrar resultados en terminal
}


# Encabezado .csv
echo "Algorithm,Replacement,Size,PageSize,PageCount,AvgTime(s),AvgFaults,AvgHits" > "$RESULTS_DIR/summary.csv"

generate_files

echo "Prueba 1: Algoritmos vs Tamaño de archivo"

ALGORITHMS=("Quick" "Merge" "Heap" "Shell" "Intro") #Definir algortimos a usar
for size in "${SIZES[@]}"; do 
    for alg in "${ALGORITHMS[@]}"; do 
    	echo "-> $alg | LRU | $size" 
        run_test "$alg" "LRU" "$size" 4096 1536 
     done
 done

echo "Prueba 2: Algoritmos x Algoritmo de reemplazo"

for alg in "${ALGORITHMS[@]}"; do
    for rep in "LRU" "FIFO"; do
        echo "-> $alg | $rep | SMALL"
        run_test "$alg" "$rep" "SMALL" 4096 490
    done
done

echo "Prueba 3: pageSize vs pageCount (RAM constante ~64MB)"

 PAGE_COMBOS=(
     "256 65536"
     "1024 16384"
     "4096 4096"
     "16384 1024"
     "65536 256"
 )
 for combo in "${PAGE_COMBOS[@]}"; do
     ps=$(echo $combo | awk '{print $1}')
     pc=$(echo $combo | awk '{print $2}')
     echo "-> Quick | LRU | LARGE | pageSize=$ps | pageCount=$pc"
     run_test "Quick" "LRU" "LARGE" "$ps" "$pc"
 done

echo "Prueba 4: Impacto de RAM disponible"

 PAGE_COUNTS=("819" "2048" "4096" "6144" "8192")
 for size in "${SIZES[@]}"; do
     for pc in "${PAGE_COUNTS[@]}"; do
         echo "-> Quick | LRU | ${SIZE_LABELS[$size]} | pageCount=$pc"
         run_test "Quick" "LRU" "$size" 4096 "$pc"
     done
 done

echo "Pruebas completadas."
echo "Resultados en: $RESULTS_DIR/summary.csv"

