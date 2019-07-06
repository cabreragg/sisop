##########################################
# ejercicio4.ps1
# TP2 Ejercicio 4
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
    Este script permite filtrar un directorio por archivos que contengan un string determinado y generar un zip con ellos.
.DESCRIPTION
    El script recibe tres parametros:
        -Un primer parametro con la ruta del directorio que contiene los archivos log de errores.
        -Un segundo parametro con la ruta del directorio donde se quiere guardar el zip.
        -Un tercer parametro con la cadena que se quiere buscar.
    Se buscara en cada archivo que se encuentre en el directorio del primer parametro todos los logs que contengan la cadena del
    tercer parametro al menos una vez y se comprimiran en un .zip guardandose este en el
    el directorio del segundo parametro con el nombre especificado al final de la ruta.
    
.EXAMPLE
    ./ejercicio4.ps1 "/home/user/carpeta/con/logs" "/home/user/ruta/archivo.zip" "CadenaBuscada"
.EXAMPLE
    Si se quiere filtrar y comprimir el directorio actual por "hola" y guardarlo en la misma ruta:
    ./ejercicio4.ps1 "./" "./archivoSalida" "hola"

#>
Param(
    [ValidateScript({
    if( -Not ($_ | Test-Path) ){
        throw "El directorio `'$_`' es invalido o inexistente..."
        exit
    }
    return $true
    })] 
    $pathlogs,
    [ValidateScript({
    if( -Not ("$_\.." | Test-Path) ){
        throw "El directorio `'$_`' es invalido o inexistente..."
        exit
    }
    return $true
    })]
    $pathsalida,
    $cadena
)
if ( -Not $pathlogs){
    throw "Recuerde ingresa como primer parametro el directorio de entrada de logs"
    exit
}
elseif( -Not "$pathsalida*"){
    throw "Recuerde ingresar como segundo parametro el directorio de salida seguido del nombre del zip"
    exit
}
elseif( Test-Path "$pathsalida"){
    throw "Recuerde que se debe especificar el nombre con el que se guardara el archivo comprimido en el segundo parametro"
    exit
}
elseif( -Not $cadena){
    throw "Recuerde ingresar como tercer parametro la cadena por la cual filtrar"
    exit
}
Get-ChildItem -path $pathlogs -recurse | 
 Select-String -pattern "$cadena" -list  |
  Sort-Object -Property Filename | 
   Compress-Archive -Update -DestinationPath $pathsalida -CompressionLevel Optimal
if ( -Not ("$pathsalida*.zip" | Test-Path) ){
    Write-Output "No se encontro ningun archivo con esa cadena"
}
else{
    Write-Output "Compresion de archivos satisfactoria"
}

