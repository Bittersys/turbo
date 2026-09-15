var
  limite, actual, anterior, temporal, i: integer;
begin
  limite := 8;
  anterior := 0;
  actual := 1;
  i := 1;

  writeln('Serie de Fibonacci:');
  writeln(anterior);
  writeln(actual);

  while i < limite do
  begin
    temporal := actual;
    actual := anterior + actual;
    anterior := temporal;
    
    writeln(actual);
    i := i + 1;
  end;
end.

