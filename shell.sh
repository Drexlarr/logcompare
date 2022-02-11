#!/bin/bash

read_config(){
	lgwy=0
	ltbl=0

	while read line;
	do 
		if line == "[GWY_LOGS]"; then
			$lgwy = 1
		
		elif line == "[TABLES_BD]"; then
			$lgwy = 0
			$ltbl = 1
		fi

	
	done < $1
}

one(){
    echo -e "\nRecuerda colocar el archivo xml y el archivo de log en la carpeta 'Files'\n (El archivo no debe contener espacios)" && sleep 1

	read -p "Ingrese el nombre del archivo xml: " fxml

	if ! test -f "files/$fxml"; then	
		echo -e "Nombre de archivo invalido\n" 	
		return
	fi

	read -p "Ingrese el nombre del archivo log: " flog

	if ! test -f "files/$flog"; then
		echo -e "Nombre de archivo invalido\n" 	
		return
	fi

	sed '1,/<expectedresults>/d' "files/$fxml" > files/clear.xml
	sed -i 's/&nbsp;/ /g' files/clear.xml
	sed -i 's/ </</g' files/clear.xml
	sed -i "s,/ol,-ol," files/clear.xml

	ol=0
	olline=0
	clear="files/clear.xml"

	while read lline;
	do
		((olline+=1))
		if [[ $lline =~ "<ol start" ]]; then
			ol=1
		fi
		if [[ $ol == 1 && $lline == "<-ol>" ]]; then
			((olline+=1))
			sed -i "$olline"',$d' files/clear.xml
			break
		fi
	done < $clear

	if [[ $ol == 0 ]]; then
		sed -i "/-ol>/Q" files/clear.xml
	fi

	./recolectlog $flog


    echo "El reporte generado se encuentra en el archivo 'results.txt'" && sleep 2
}
 
two(){
	read -p "Ingresa el nombre del archivo de configuracion que desea cargar: " fconfig

	read_config $fconfig
	

}
 
show_menus() {
	clear
	echo "#####################"	
	echo " FIND - RECOLECTOR"
	echo "#####################"
	echo "                     "
	echo "1. Validar Flujo"
	echo "2. Recolectar evidencias"
	echo "3. Salir"
}

read_options(){
	local choice
	read -p "Elige una opcion: " choice
	case $choice in
		1) one ;;
		2) two ;;
		3) exit 0;;
		*) echo -e "${RED}Elige una opcion valida...${STD}" && sleep 1
	esac
}

 
while true
do
 
	show_menus
	read_options
done