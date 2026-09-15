var
    i, suma : integer;
begin
    i:=1;
    suma := 0;
    
    repeat
        suma := suma + i;
        i := i + 1;
    until i > 5;
    
    if suma > 100 then
        writeln(i,suma)
    else
        writeln(i,0)

end.
