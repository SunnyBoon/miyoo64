# Paper Mario PAL v0.1.1-pal-alpha — Instalación (Español)

Para **Miyoo Mini Plus con OnionOS**. Port nativo ARM; **no es necesario compilar**. Este paquete añade una entrada en Ports, un icono nuevo de la aplicación y una carátula para Ports. El programa y las recetas no cambian.

## Instalación e importación en Ports

1. Cuando se publique, descargar **PaperMarioPAL-0.1.1-pal-alpha-OnionOS.zip** desde la versión preliminar **v0.1.1-pal-alpha** en SunnyBoon/miyoo64. SHA-256 del ZIP: **d65494e451820223279bc2fa442d1a165c861649bc0312a03ecb938e73430f31**. Conservar **NOTICE.md** y **THIRD-PARTY-LICENSES.txt** junto al ZIP; LICENSE y la documentación completa forman parte de los documentos del repositorio.
2. Salir del juego antes de actualizar. Extraer en la **raíz de la tarjeta SD**, fusionando las carpetas existentes **App/** y **Roms/**. Mantener exactamente **App/PaperMarioPAL/** y **Roms/PORTS/**. No sustituir ni borrar las carpetas App o Roms completas. El paquete no apunta a la instalación estadounidense original en App/PaperMario.
3. Aportar una ROM propia de **Paper Mario (Europe, PAL)** como **App/PaperMarioPAL/rom.z64**. Requisitos: **64 MiB (67.108.864 bytes), Z64 big-endian**, SHA-1 **2111d39265a317414d359e35a7d971c4dfa5f9e1**. No se incluyen ROM, recursos extraídos del juego, partidas guardadas ni cachés de ejecución. Cambiar el nombre de otra ROM no convierte su contenido.
4. Desde Apps, iniciar **Paper Mario PAL 0.1-pal-alpha**. Este sigue siendo el nombre real del paquete probado en hardware; la versión de la descarga es v0.1.1-pal-alpha.
5. Para Ports, utilizar la **función de importación de ports de la versión de OnionOS instalada** y elegir después **Paper Mario PAL** en Ports. El lanzador es **Roms/PORTS/Shortcuts/Paper Mario PAL.port**, su destino de importación **App/PaperMarioPAL/launch.sh**, y la carátula **Roms/PORTS/Imgs/Paper Mario PAL.png**. **Roms/PORTS/Games/** debe existir; se incluye como carpeta vacía. No colocar allí una segunda ROM ni otra instalación del juego. No se suministra ni sustituye ninguna configuración global de Emu.
6. Al iniciar el juego por primera vez, la aplicación comprueba la ROM y crea localmente los datos necesarios. Elegir **English, German, French o Spanish** dentro del juego. Apps y Ports abren la misma aplicación y comparten **App/PaperMarioPAL/data/**, la ROM, las partidas normales y la ubicación de estados de guardado de este build. La entrada Ports es un acceso al mismo juego, no una segunda instalación.
7. Una instalación alpha compatible utiliza su carpeta App/PaperMarioPAL existente. Fusionar los archivos del paquete y mantener **rom.z64** y **data/** en su sitio; ninguno está incluido en el ZIP. Esta actualización del paquete no exige migrar partidas manualmente ni borrar la caché. Salir de una sesión antes de iniciar desde el otro menú.

## Solución de problemas

- Aplicación ausente: comprobar la estructura y los seis archivos pm64, recipes.bin, launch.sh, config.json, icon.png, full_resolution. Mantener **pm64**, **launch.sh** y el lanzador **.port** ejecutables (0755); los demás archivos tienen modo 0644.
- Entrada Ports/carátula ausente: comprobar los nombres PAL exactos, la carpeta Games vacía y el lanzador de la aplicación; repetir la importación Ports de OnionOS. Si falta el soporte Ports/importación, consultar la documentación de la versión de OnionOS instalada. Este ZIP no instala el importador del sistema.
- ROM rechazada: comprobar ubicación, tamaño, orden de bytes Z64 y SHA-1. No añadir otra ROM bajo Roms/PORTS. No se ofrecen enlaces para descargar ROM.
- Primer inicio: esperar a que termine la creación local de datos y comprobar espacio libre con permiso de escritura. No mezclar programa, recetas, cachés o estados con el build del PR separado.
- Partidas diferentes: comprobar que los dos lanzadores siguen apuntando a la misma carpeta App. No mover ni sobrescribir partidas existentes para investigar el problema.

## Pruebas y limitaciones conocidas

El mantenedor informa de **6/6 PASS** para M14.51 en un Miyoo Mini Plus real: Apps/icono nuevo, importación Ports, carátula, inicio por Ports, partidas compartidas, guardado/carga y regreso a OnionOS. Fue una **prueba manual del usuario en hardware**, no una medición automática ni una partida completa. La base M14.39b sin cambios ya se había jugado desde el prólogo hasta el inicio del capítulo 1; sus anteriores pruebas separadas de estados de guardado no implican pruebas DualMenu exhaustivas nuevas. El PR más reciente usa otro build.

Fallos conocidos, sin corregir: uniones rectas y estrechas en algunos árboles grandes que dejan ver el fondo; dos lados en lugar de cuatro en el marco del enemigo ampliado por Goombario; una pieza de estrella parpadea incorrectamente en vez de permanecer visible, mientras su sombra sigue visible. Véanse KNOWN-ISSUES.md, NOTICE.md y ARTWORK-PROVENANCE.md. Según el mantenedor, ambas imágenes se generaron en ChatGPT a partir de texto; la incertidumbre restante sobre posibles derechos de terceros en personajes/logotipos ha sido aceptada expresamente por el responsable de la publicación (ARTWORK_PUBLISHER_DECISION: ACCEPTED), sin presentarla como una autorización externa. Véase ARTWORK-PUBLISHER-DECISION.md. Este documento prepara la publicación y no demuestra que v0.1.1-pal-alpha ya esté publicado.
