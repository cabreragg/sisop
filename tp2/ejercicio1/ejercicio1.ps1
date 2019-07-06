##########################################
# ejercicio1.ps1
# TP2 Ejercicio 1
# Integrantes:
#       Bouza Facundo 	DNI 41332191
#		Cabrera Gonzalo	DNI 37350600
#		Facal Ernesto 	DNI 38983722
#		Marson Tomás	DNI 40808276
#		Viescas Julián	DNI 40258471
# Primera reentrega.
###########################################


# 1. El script recibe por parametro un path, verifica que sea una ruta existente/valida e imprime por terminal
#    los nombres y tamaño de todos los archivos existentes en esa ruta (unicamente archivos, no directorios).
#    Si el path no fuese valido, arroja error y termina su ejecucion.

# 2. Al parametro path salida se le podrian agregar las siguientes validaciones:
#       a. Que sea de tipo System.IO.FileInfo: Esto evitaria que tenga caracteres especiales que no pueden formar
#          parte de un path valido.
#       b. Que sea mandatorio: No tendria sentido correr el script sin pasarle un parametro de entrada. Arrojaria
#          error siempre.
#       c. Agregar un validate script: En este ejemplo no tendria mucho sentido ya que validar el path es lo primero
#          que se realiza. Pero si se hiciesen operaciones antes de validar el parametro, al agregar el decorador
#          ValidateScript para testear el path ahorraria todo ese codigo y rebotaria paths invalidos apenas iniciado
#          el script.
#   
#    La definicion del parametro $pathSalida quedaria de la siguiente manera con todas estas validaciones:
#
#       [ValidateScript({
#         if( -Not ($_ | Test-Path) ){
#           throw "El Path ingresado `'$_`' es invalido o inexistente..."
#         }
#         return $true
#       })]
#       [Parameter(Mandatory)]
#       [System.IO.FileInfo]
#       $pathSalida    
#
# 3. El script entero se podria reemplazar por el cmdlet Get-ChildItem ejecutado dentro de este mismo script, pero extrayendo
#    de los archivos resultantes solo las propiedades Name y Length. La llamada quedaria de la siguiente manera:
#       Get-ChildItem "Path/A/Evaluar" -File | Select-Object -Property Name, Length
#
#    Ejemplo de ejecucion:
#       Get-ChildItem "/home/gonza/Desktop/TP2/Ejercicio 6" -File | Select-Object -Property Name, Length

Param($pathsalida)
$existe = Test-Path $pathsalida
if ($existe -eq $true) {
    $lista = Get-ChildItem -File
    foreach ($item in $lista) {
        Write-Host "$($item.Name) $($item.Length)"
    }
} else {
    Write-Error "El path no existe"
}

#EOF