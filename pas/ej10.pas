var
  numero, contador, resultado: integer;
begin
  numero := 9;
  resultado := 1;
  contador := 1;

  while contador <= numero do
  begin
    resultado := resultado * contador;
    contador := contador + 1;
  end;

  writeln('El factorial de ',numero,' es:');
  writeln(resultado);
end.
