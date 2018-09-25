 -Para lograr el objetivo el estudiante debe desarrollar un programa en el lenguaje de programación C++, orientado al sistema operativo UNIX y utilizar todas las facilidades para la comunicación entre procesos (IPC) en ese ambiente.

 -Debe construirse un programa en C++ que creará un número determinado de procesos (fork) que le ayudarán a completar su tarea.

 -La función de este utilitario es la de contar el número de palabras distintas que sean marcas (delimitadas por "<" y">"), y la cantidad de veces que aparecen, en una cantidad determinada de archivos con formato XML, semejante a la primera tarea programada, pero esta vez con varios archivos.

 -La sintaxis del programa es la siguiente:

    	Contar [-t] Nombre_Archivo1 Nombre_Archivo2...

 -El proceso principal creará (fork) tantos procesos hijos como nombres de archivos se haya especificado, de tal manera que cada hijo se encargue de enviar los resultados (palabras y cantidad) de un archivo únicamente.

 -Además creará otro hijo (impresor) para producir la salida del programa.

 -Cada uno de los hijos abrirá el archivo correspondiente, asignado por el proceso principal, y comenzará a enviar la marca y las veces que aparece al proceso principal, utilizando para ello paso de mensajes.

 -El proceso principal recibirá los mensajes enviados por sus hijos, acumulará el número de veces que aparezca la misma marca en varios archivos y almacenará la palabra y su número total de apariciones en un área de datos compartida (shared memory) con el proceso impresor.

 -Una vez que el proceso principal completa todas las palabras que comienzan con una misma letra, permitirá al proceso impresor continuar (semáforo), tomar la información y hacer su trabajo.

 -El proceso impresor tomará las palabras almacenadas en la estructura de datos compartida y las enviará a la salida estándar (desde donde se podrán redireccionar).
 
 -Cuando el archivo designado a un hijo se acabe este debe informar al principal, utilizando un mensaje de tipo especial.
 
 -El programa deberá tratar apropiadamente todas las condiciones de error previsibles: archivos que no existen, número inválido de parámetros, opciones inválidas, etc.

 -El programa deberá acompañarse de un documento que detalle las pautas que emplearon para su resolución:
        *Explicación de algoritmos específicos.
        *Descripción de variables, constantes, funciones.
        *Problemas encontrados.
        *Comentarios sobre los resultados obtenidos.
        *Una documentación interna adecuada permite que la documentación externa sea mínima.
        *No es necesario adjuntar el código fuente en forma impresa
   
 -Los programas fuente deberán subirse al repositorio de SubVersion designado en el curso.
 -Deben enviar un correo electrónico a la cuenta "ci1310.g1 at gmail.com" indicando su ubicación exacta.
