##########################################
# ejercicio2.ps1
# TP2 Ejercicio 2
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
    Este script se encarga de mostrar los archivos duplicados de manera recursiva dado un directorio.
.DESCRIPTION
    Este script se encargara de mostrar los archivos duplicados de manera recursiva dado un directorio, entendiendose como duplicados 
    aquellos que comparten tamaño y peso, mostrando un ejemplo de duplicado por cada grupo de duplicado.

.EXAMPLE
    ./ejercicio2.ps1 "/home/user/dir"
#>



Param($pathsalida)
$existe = Test-Path $pathsalida
if ($existe -eq $true) {
    $arraylist = Get-ChildItem -Path $pathsalida -recurse -File 
    $hashNombres = @{ };
    foreach ($item in $arraylist) {
        if ($hashNombres[$item.Name] -eq $null) {   #no existe clave con este nombre
            $hashTam = @{ };
            [void]$hashTam.Add($item.Length, 1);
            [void]$hashNombres.Add($item.Name, $hashTam);  
        }
        else {      #existe clave con este nombre
            $hashTam = $hashNombres[$item.Name];
            if ($hashTam[$item.length] -eq $null) {             #no existe clave con este tamaño
                [void]$hashTam.Add($item.length, 1);
                # [void]$hashNombres.Remove($item.Name);
                # [void]$hashNombres.Add($item.Name, $hashTam);
                $hashNombres[$item.Name] = $hashTam;
            }
            else {          #existe clave con este tamaño
                # $value = ++ $hashTam[$item.length];
                # [void]$hashTam.Remove($item.length);
                # [void]$hashTam.Add($item.length, $value);
                $hashTam[$item.length] = ++ $hashTam[$item.length];
                # [void]$hashNombres.Remove($item.Name);
                # [void]$hashNombres.Add($item.Name, $hashTam);
                $hashNombres[$item.Name] = $hashTam
            }
        }
    }
    foreach ($keyname in $hashNombres.Keys) {
        foreach ($itemTam in $hashNombres[$keyname]) {
            foreach($keylength in $itemTam.Keys){
                # $keylength;
                foreach($value in $itemTam[$keylength]){
                    if($value -gt 1){
                        #Write-Output "$($keyname)$(" con tamaño ")$($keylength)$(" y repeticiones ")$($value)$(" esta duplicado")"
			Write-Output "$($keyname)"
                    }
                }
            }
        }
    }
}
else {
    Write-Error "El path no existe"
}
