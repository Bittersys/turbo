var 
    i,j: integer;
    r: real;
begin
    r:=0;
    for i:=1 to 1000 do
        for j:=1 to 1000 do
            r:=r+1;
    writeln(r);
end.