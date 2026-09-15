var
    numero: Integer;
    resultado: Integer;

function Factorial(n: Integer): Integer;
begin
    if (n = 0) or (n = 1) then
        Factorial := 1
    else
        Factorial := n * Factorial(n - 1);
end;

begin
    numero := 5;
    WriteLn('El factorial de:',numero, ' es ',Factorial(numero));   
end.
