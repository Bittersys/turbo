var
    i, suma : integer;
begin
    suma := 0;
    
    for i := 1 to 5 do
        suma := suma + i;
    
    if suma > 100 then
        writeln(i,suma)
    else
        writeln(i,0)

end.
