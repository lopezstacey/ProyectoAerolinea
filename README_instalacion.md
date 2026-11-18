# ProyectoAerolinea

Aplicacion de gestion de una aerolinea desarrollada en C++ con interfaz grafica SFML.

## Requisitos

- Windows.
- ZinjaI, usando la cadena de herramientas MinGW incluida con la instalacion del proyecto.
- SFML 2 para esa misma cadena de herramientas. La configuracion de ZinjaI busca sus archivos en `C:\Program Files (x86)\ZinjaI\mingw32-gcc6\sfml2`.

El proyecto ya configura las rutas de include y bibliotecas de SFML en `ProyectoAerolinea.zpr`. Enlaza `sfml-graphics`, `sfml-window`, `sfml-system` y `comdlg32`. Si SFML esta instalado en otra ubicacion, actualice las rutas de include y bibliotecas en las configuraciones Debug y Release de ZinjaI. Use una version de SFML compatible con la cadena de herramientas MinGW seleccionada.

## Abrir, compilar y ejecutar

1. Abra `ProyectoAerolinea.zpr` desde ZinjaI.
2. Seleccione la configuracion Debug o Release.
3. Use las opciones de ZinjaI para compilar y ejecutar el proyecto.

La configuracion genera `ProyectoAerolinea.exe` en la carpeta temporal de la configuracion (por defecto, `Debug` o `Release`). Si ejecuta el programa fuera de ZinjaI, hagalo desde la carpeta del proyecto para que encuentre los archivos de `resources`.

## Archivos necesarios para ejecutar

Mantenga junto al proyecto:

- La carpeta `resources`, que contiene imagenes y fuentes utilizadas por la interfaz.
- Las bibliotecas dinamicas de SFML incluidas (`sfml-graphics-2.dll`, `sfml-window-2.dll` y `sfml-system-2.dll`), accesibles desde la carpeta del ejecutable o desde el `PATH`.

Si Windows indica que falta una DLL al iniciar, instale o configure la version de SFML correspondiente a MinGW y asegurese de que sus DLL esten disponibles para el ejecutable.
