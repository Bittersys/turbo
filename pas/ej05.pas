const
  N = 5; { Cantidad de elementos del arreglo }

var
  arr: array[1..N] of integer;
  i, j, aux: integer;

begin
  { Cargar valores de ejemplo }
  arr[1] := 34;
  arr[2] := 12;
  arr[3] := 50;
  arr[4] := 9;
  arr[5] := 22;

  { Algoritmo de la burbuja para ordenar de menor a mayor }
  for i := 1 to N - 1 do
  begin
    for j := 1 to N - i do
    begin
      if arr[j] > arr[j + 1] then
      begin
        aux := arr[j];
        arr[j] := arr[j + 1];
        arr[j + 1] := aux;
      end;
    end;
  end;

  { Mostrar el arreglo ordenado }
  writeln('Arreglo ordenado:');
  for i := 1 to N do
    write(arr[i], ' ');
  writeln;
end.