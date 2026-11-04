# Paper Mario PAL v0.1-pal-alpha — Instalación (Español)

Para **Miyoo Mini Plus con OnionOS**. Este port nativo ARM está listo para instalar: **no es necesario compilar**. Necesita su propia copia europea PAL sin modificar de Paper Mario. No se incluyen ROM ni recursos extraídos del juego, y no se ofrece ninguna descarga de ROM.

## Descargar e instalar

1. En la [página de versiones de SunnyBoon/miyoo64](https://github.com/SunnyBoon/miyoo64/releases), seleccione **v0.1-pal-alpha** y descargue **PaperMarioPAL-0.1-pal-alpha-OnionOS.zip**. Compruebe su SHA-256 con el archivo SHA256SUMS adjunto: `763c582d37bff03363a14b2d04fe0f3726f44354f3e1098bef6c83302ca4f794`. Descargue también `LICENSE`, `NOTICE.md` y `THIRD-PARTY-LICENSES.txt` y consérvelos junto al ZIP.
2. Extraiga el contenido del ZIP en la **raíz de la tarjeta SD**, manteniendo `App/PaperMarioPAL/`. Los seis archivos deben quedar juntos: `pm64`, `recipes.bin`, `launch.sh`, `config.json`, `icon.png`, `full_resolution`. Evite una carpeta exterior adicional. Si `App/PaperMarioPAL/` ya existe, haga una copia de seguridad y revise la instalación antes de sustituir nada.
3. Coloque su propia ROM **Paper Mario (Europe, PAL)** como **`App/PaperMarioPAL/rom.z64`**. Debe ser un archivo **Z64 big-endian de 64 MiB (67.108.864 bytes)** con SHA-1 **`2111d39265a317414d359e35a7d971c4dfa5f9e1`**. No sirven las versiones estadounidense/japonesa, los archivos modificados ni un orden de bytes incorrecto. Cambiar el nombre no convierte el contenido.
4. Vuelva a colocar la tarjeta SD en el Miyoo Mini Plus, abra la lista de aplicaciones de OnionOS e inicie **Paper Mario PAL 0.1-pal-alpha**.
5. En el primer inicio, la aplicación comprueba la ROM y genera los datos necesarios localmente en **`App/PaperMarioPAL/data/`**. Espere a que termine; no necesita extraer recursos manualmente ni compilar. Mantenga la ROM en la tarjeta para las lecturas durante el juego. No copie cachés ni estados de guardado antiguos en esta carpeta.
6. En la selección de idioma del juego, elija **English, German, French o Spanish** (inglés, alemán, francés o español). Siga las indicaciones que muestra el juego. Use los guardados normales de esta versión y estados nuevos; no se admite migrar estados entre versiones.

## Solución de problemas

- **La aplicación no aparece o no se inicia:** compruebe la estructura exacta y los seis archivos. `pm64` y `launch.sh` deben seguir siendo ejecutables; el ZIP indica 0755 para ellos y 0644 para los demás. Conserve los permisos si el programa de extracción lo solicita. Use el ZIP completo con sus archivos correspondientes.
- **La ROM no se encuentra o se rechaza:** compruebe la ubicación exacta de `rom.z64`, el tamaño, el formato big-endian y el SHA-1 indicado. Evite una extensión doble. Use su propia copia compatible; aquí no se proporciona ningún enlace de descarga.
- **El primer inicio tarda más:** espere a que termine la generación local de datos. La tarjeta SD debe tener espacio libre y permitir escritura. No interrumpa una escritura ni añada una caché de otra versión.
- **No puede guardar/cargar, falta audio o persiste un problema al iniciar:** confirme el dispositivo/OnionOS, verifique el ZIP y mantenga juntos el programa y las recetas originales. Proteja sus guardados. Anote el error visible y los pasos para solicitar ayuda; no sustituya archivos por los de otra versión ni comparta ROM, guardados o datos privados.

## Pruebas y problemas conocidos

El **ZIP exacto para usuarios superó 5/5 comprobaciones manuales en hardware comunicadas por el usuario**: inicio desde esta carpeta, cuatro idiomas, menú principal/nueva partida, introducción/primer combate, guardado/carga normal y estados de guardado/carga. Su base M14.39b también se jugó de forma estable durante el prólogo hasta el comienzo del capítulo 1. Es una **alpha**, no una validación del juego completo; las escenas posteriores y todas las habilidades especiales no se han probado exhaustivamente. Los resultados solo cubren el dispositivo y sistema indicados.

Problemas gráficos conocidos, aún sin corregir:

- Algunos árboles grandes muestran uniones estrechas y rectas entre segmentos por las que se ve el fondo.
- El marco de la representación ampliada de un enemigo por Goombario está incompleto: dos lados en lugar de cuatro.
- Un fragmento de estrella parpadea por error aunque debería permanecer visible de forma estática; su sombra sigue visible mientras el fragmento desaparece.

La [PR independiente nº 7](https://github.com/mr-finster/miyoo64/pull/7) tiene su propia validación de hardware **8/8** y otras fuentes, programa y recetas. No sustituye el resultado **5/5** de este ZIP. No mezcle instalaciones, estados ni cachés. Consulte KNOWN-ISSUES.md y NOTICE.md.
