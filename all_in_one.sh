#!/bin/bash
output_file="codigo.txt"
> "$output_file"  # Limpiar el archivo de salida antes de empezar

for file in *.cpp *.h; do
  if [ -f "$file" ]; then  # Asegurarse de que sea un archivo regular
    # Verificar si el nombre del archivo ya está en codigo.txt
    if ! grep -q "// ${file}" "$output_file"; then
      echo "// ${file}" >> "$output_file"
      cat "$file" >> "$output_file"
      echo -e "\n" >> "$output_file"  # Agregar una línea en blanco entre archivos
    fi
  fi
done
