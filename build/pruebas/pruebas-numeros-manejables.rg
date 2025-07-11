//Problema del tablero de ajedrez:
//si en la primera casilla ponemos un grano,
//y duplicamos la cantidad de granos en la siguiente,
//y así hasta rellenar el tablero,
//¿cuántos granos tendremos?
granos = 1
i = 0
while i < 64 then
   puts("En el escaque &{i}")
   puts(" hay &{granos}\n")
   granos *= 2
   i++
end
