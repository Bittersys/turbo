var
    global_x : integer;

procedure calcular_doble(valor : integer);
var
    local_resultado : integer;
begin
    local_resultado := valor * 2;
    if local_resultado < 100 then
        writeln('El resultado es',local_resultado);
end;

begin
    global_x := 21;
    calcular_doble(global_x);
end.
