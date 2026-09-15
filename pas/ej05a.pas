var
    i,tmp: integer;
    arr: array[1..5] of integer;

begin
  { Cargar valores de ejemplo }
    tmp := 100;
    arr[1] := 34;
    arr[2] := 50;
    arr[3] := 7;
    arr[4] := 18;
    arr[5] := 131;
  
  { Mostrar el arreglo }
    for i:=1 to 5 do begin
        tmp := arr[i];
        arr[i] := arr[i] + 1;
        writeln(tmp, arr[i]);
    end;
end.