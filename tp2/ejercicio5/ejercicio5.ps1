#############################################################################
# ejercicio5.ps1
# TP2 Ejercicio 5
# Integrantes:
#       Bouza Facundo 	DNI 41332191
#		Cabrera Gonzalo	DNI 37350600
#		Facal Ernesto 	DNI 38983722
#		Marson Tomás	DNI 40808276
#		Viescas Julián	DNI 40258471
# Primera reentrega.
#############################################################################

<#
.SYNOPSIS
    Este script se encarga de mostrar cada un tiempo determinado la cantidad de procesos activos
    o la cantidad archivos presentes en cierto directorio.
.DESCRIPTION
    Este script se encargara de mostrar cada determinado tiempo (3 segundos), dependiendo de los parametros indicados,
    la cantidad de procesos activos o la cantidad de archivos presentes en un directorio recibido por parametro.
    Los parametros son excluyentes, solo se mostrara una de las dos salidas posibles al mismo tiempo.
    Si se desean ver los procesos activos, no se podra tambien indicar la cantidad de archivos en cierto directorio.
    Para poder obtener ambas salidas habra que ejecutar el script al menos 2 veces, una vez con cada uno de los parametros posibles.

.EXAMPLE
    Para ver los procesos activos:
    ./ejercicio5.ps1 -Procesos
.EXAMPLE
    Para ver los archivos presentes en un directorio en particular:
    ./ejercicio5.ps1 -Archivos -Directorio "/home/user/dir"
#>

param(
    [Parameter(ParameterSetName="set1")]
    [switch]
    $Procesos,

    [Parameter(ParameterSetName="set2")]
    [switch]
    $Archivos,

    [ValidateScript({
        if( -Not ($_ | Test-Path) ){
            throw "El directorio `'$_`' es invalido o inexistente..."
        }
        return $true
    })]
    [Parameter(ParameterSetName="set2", Mandatory=$true)]
    [System.IO.FileInfo]
    $Directorio
)

if ($PSBoundParameters.Count -eq 0) {
    Write-Host "Error en la llamada! `n
                Uso: ./ejercicio5.ps1 [-Procesos] [-Archivos] -Directorio <path> `n
                -Procesos `t muestra los procesos activos en el sistema, excluyente con -Archivos, solo se puede recibir uno de los dos parametros `n
                -Archivos `t muestra los archivos presentes en un directorio especificado, excluyente con -Procesos `n
                -Directorio `t si se ingreso el parametro -Archivos, se debera indicar mandatoriamente el path a un directorio valido" 
    return
}

if ($Procesos) {
    while ($true){
        $process = Get-Process

        Write-Host $process.count

        Start-Sleep -s 3
    }
}

if ($Archivos) { 
    while ($true){
        $files = Get-ChildItem -Path $Directorio

        Write-Host $files.count

        Start-Sleep -s 3
    }
}

#EOF