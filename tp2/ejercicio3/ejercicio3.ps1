##########################################
# ejercicio3.ps1
# TP2 Ejercicio 3
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
Este script se encarga de resumir un archivo .csv de multas por patente y año en una ruta especificada.
.DESCRIPTION
Este script se encarga de leer un archivo .csv desde una ruta pasada por parametro -Entrada, resumir los campos de dicho archivo
por Patente y Año, y por ultimo escribir dichos datos en un nuevo archivo .csv en la ruta especificada por parametro -Salida.

El formato de los campos del archivo .csv de entrada es "Patente","ValorMulta","Fecha" 
y el formato en el archivo .csv de salida sera "Patente","Año","TotalMultas".
.EXAMPLE
./ejercicio3.ps1 -Entrada "ruta/archivo/a/resumir" -Salida "ruta/archivo/datos/resumidos"
#>

param (
[Parameter(Mandatory=$true)][string]$Entrada,
[Parameter(Mandatory=$true)][string]$Salida
)

$csv_agrupado = @()

$valid_e = Test-Path -Path $Entrada
if ($valid_e -eq $false) {
    Write-Error "La ruta del archivo de entrada no existe"
    exit 
}

 $valid_s = Test-Path -Path "$($Salida)\.."
 if ($valid_s -eq $false) {
     Write-Error "La ruta de salida no es valida"
     exit 
 }

$csv = Import-Csv -Path $Entrada | Sort-Object -Property "Patente"
foreach ($item in $csv) {
    $item.Fecha = $item.Fecha.Substring(6,4)
}
$csv | Group-Object -Property "Patente","Fecha" | ForEach-Object {
     $csv_agrupado += [pscustomobject]@{
         Patente = $_.Name
         Año = $_.Fecha
         TotalMultas = ($_.Group | Measure-Object -Property ValorMulta -Sum).Sum
     }
}

$csv_agrupado | Export-Csv -Path $Salida -NoTypeInformation

#EOF
