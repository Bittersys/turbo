var
    i: integer;
    x,y: real;
begin
    x:=3.14159;
    y:=2.5;

    writeln(x+y,x-y,x*y,x/y);
    x:= x + 3;
    y:= y + x;
    writeln(x,y);

    for i:=1 to 10 do
        x := x + 1;

    writeln(x,y);
end.