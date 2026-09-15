const
  FILAS = 3;
  COLUMNAS = 3;

var
  tablero: array[0..2, 2..4] of integer; { Índices no basados en 1 para estresar el cálculo }
  i, j, producto: integer;

begin
  { Cargar valores calculados dinámicamente }
  for i := 0 to FILAS - 1 do
  begin
    for j := 2 to COLUMNAS + 1 do
    begin
      tablero[i, j] := i * j;
    end;
  end;

  { Mostrar la matriz y verificar posiciones clave }
  writeln('Matriz Bidimensional Calculada:');
  
  { Imprimir fila 0: 0*2, 0*3, 0*4 -> Debe ser 0 0 0 }
  write(tablero[0, 2], ' ', tablero[0, 3], ' ', tablero[0, 4]);
  writeln;

  { Imprimir fila 1: 1*2, 1*3, 1*4 -> Debe ser 2 3 4 }
  write(tablero[1, 2], ' ', tablero[1, 3], ' ', tablero[1, 4]);
  writeln;

  { Imprimir fila 2: 2*2, 2*3, 2*4 -> Debe ser 4 6 8 }
  write(tablero[2, 2], ' ', tablero[2, 3], ' ', tablero[2, 4]);
  writeln;
end.
