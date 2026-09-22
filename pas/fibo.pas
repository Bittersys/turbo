var 
    i, n, x1, x2, fibo:integer;
begin
    i:= 0;
    x1:=1;
    x2:= 0;

    write('Numero:');
    read(n);

    while i < n do begin
        fibo:= x1 + x2;
        i := i + 1;
        writeln(i, fibo);

        x2:= x1;
        x1:= fibo;
    end;

end.