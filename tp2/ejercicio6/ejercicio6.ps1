##########################################
# ejercicio6.ps1
# TP2 Ejercicio 6
# Integrantes:
#       Bouza Facundo 	DNI 41332191
#		Cabrera Gonzalo	DNI 37350600
#		Facal Ernesto 	DNI 38983722
#		Marson Tomás	DNI 40808276
#		Viescas Julián	DNI 40258471
# Primera reentrega.
###########################################

<#
.SYNOPSIS
    Este script se encargara de realizar un producto escalar o la transposicion de una matriz recibida en un archivo.
.DESCRIPTION
    Este script se encargara de realizar un producto escalar o la transposicion de una matriz recibida en un archivo.
    El script recibira el path del archivo de la matriz, y podra realizar el producto escalar (indicando el escalar a multiplicar)
    o realizara la transposicion de la matriz. No puede realizar ambas funciones a la vez.

.EXAMPLE
    Para realizar producto escalar:
    ./ejercicio6.ps1 -Entrada $pathMatriz -Producto 5
.EXAMPLE
    Para trasponer la matriz:
    ./ejercicio6.ps1 -Entrada $pathMatriz -Trasponer
#>

param (
    [ValidateScript({
        if( -Not ($_ | Test-Path) ){
            throw "El archivo de entrada `'$_`' es invalido o inexistente..."
        }
        return $true
    })]
    [Parameter(Mandatory)]
    [Parameter(ParameterSetName="set1")]
    [Parameter(ParameterSetName="set2")]
    [System.IO.FileInfo]
    $Entrada,
    [Parameter(ParameterSetName="set1")]
    [double]
    $Producto,
    [Parameter(ParameterSetName="set2")]
    [switch]
    $Trasponer
)

if($PSBoundParameters.Count -eq 1){
    Write-Host "Debe seleccionar una operacion a realizar con la matriz...`n
                Uso: ./ejercicio6.ps1 -Entrada <pathEntrada> [-Producto <int32>] [-Trasponer]`n
                -Entrada `t es el archivo que contiene la matriz a procesar `n
                -Producto `t entero con el que se va a realizar la operacion de producto escalar, excluyente con trasponer `n
                -Trasponer `t parametro a recibir si se quiere trasponer la matriz ingresada" -ForegroundColor Red
    exit
}

function Producto {
    param(
        [Parameter(Mandatory=$true)][double]$Escalar,
        [Parameter(Mandatory=$true)][System.IO.FileInfo]$Path
    )
    Write-Host "Realizando el producto..." -ForegroundColor Green
    $lista = Get-Content $Path
    $vec = @()
    $salida = @()
    $mat = @()

    foreach ($item in $lista){
        $numeros = $item.Split('|')
        foreach ($num in $numeros){
            $vec += "$([double]$num*$Escalar)"                  # Realizo el producto de cada número de la matriz por el escalar y lo guardo en un Array 'vec'
        }
        $vec += ';'                                             # Separador de fila
    }
    foreach ($item in $vec){
        if($item -ne ';'){
            $salida = "$($salida)" + "$($item)|"                # Creo el string que va a significar cada fila de la matriz con el separador entre numeros
        }
        else {
            $salida = $salida.Substring(0,($salida.Length)-1)   # Elimino el último caracter del string (es decir, elimino el último '|')
            $mat += $salida
            $salida = ""                                        # Vacío el string salida para poder crear la próxima fila para la matriz
        }
    }
    return $mat
    
}
function Trasponer {
    param([Parameter(Mandatory=$true)][System.IO.FileInfo]$Path)

    Write-Host "Trasponiendo la matriz..." -ForegroundColor Green
    $lista = Get-Content $Path
    $vec = @()
    $salida = @()
    $mat = @()

    foreach ($item in $lista){
        $numeros = $item.Split('|')
        foreach ($num in $numeros){
            $vec += "$($num)"                                           #Copio cada espacio de la matriz a $vec
        }
        $vec += ';'                                                     #Separador de fila
    }
    $columnas = $vec.IndexOf(';')                                       #Defino número de columnas como el index donde encuentro el ; (ya que ese caracter es mi "separador de fila")
    $filas = ($vec.Length/$columnas)-1                                  #La cantidad de filas será el lagro del vector dividido la cantidad de columnas. Le resto uno ya que una columna es del separador ';'
    for($j = 0; $j -lt $columnas; $j++){
        for($i = 0; $i -lt $filas; $i++){
            $salida = "$($salida)" + "$($vec[$j+$i*($columnas+1)])|"    #Hago la transposición de la matriz
        }
        $salida = $salida.Substring(0,($salida.Length)-1)
        $mat += $salida
        $salida = ""
    }
    return $mat
}

function GenerarArchivoSalida {
    Write-Host "Generando archivo..." -ForegroundColor Green
    $pathSalida = (Get-ChildItem $Entrada).Directory.ToString() + "/salida." + $Entrada.Name.ToString()
    $Matriz | Out-File -FilePath $pathSalida
    Write-Host "Archivo generado. Nuevo nombre: $pathSalida" -ForegroundColor Green
}

if ($Producto){
    $Matriz = Producto -Escalar $Producto -Path $Entrada
    GenerarArchivoSalida
}

if ($Trasponer){
    $Matriz = Trasponer -Path $Entrada
    GenerarArchivoSalida
}