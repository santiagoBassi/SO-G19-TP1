# TP1 - Sistemas Operativos 72.11 - Grupo 9

## Instrucciones de compilación

Todo el código fue testeado y compilado en el entorno de Docker provisto por la cátedra. Para compilar el proyecto ejecutar dentro del contenedor:

```bash
make clean
make
```

## Instrucciones de uso

### Ejecución app

Para ejecutar la aplicación, ejecutar dentro del contenedor:

```bash
./app file1 file2 ... fileN
```

### Ejecución slave 

Para ejecutar el slave, ejecutar dentro del contenedor:

```bash
./slave
```

Ingresar archivos por entrada estandar. Se lee hasta EOF o string vacio.

### Ejecución app + proceso vista (con pipe)

Para ejecutar la aplicación con el proceso vista usando un pipe, ejecutar dentro del contenedor:

```bash
./app file1 file2 ... fileN | ./view
```

### Ejecución app + proceso vista (sin pipe)

Para ejecutar la aplicación y proceso vista en dos terminales diferentes. Dentro del contenedor, ejecutar en una terminal:

```bash
./app file1 file2 ... fileN
```

Dentro del contenedor, en otra terminal:

```bash
./view
/md5view
```
(entrada estandar) o

```bash
./view /md5view
```
(como parametro). /md5view es el nombre compartido.
