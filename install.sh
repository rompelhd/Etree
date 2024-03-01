#!/bin/bash

greenColour="\e[0;32m\033[1m"
endColour="\033[0m\e[0m"
redColour="\e[0;31m\033[1m"
orangeColour="\033[38;5;202m"
blueColour="\e[0;34m\033[1m"
yellowColour="\e[0;33m\033[1m"
purpleColour="\e[0;35m\033[1m"
turquoiseColour="\e[0;36m\033[1m"
grayColour="\e[0;37m\033[1m"

Transletions_DIR="$HOME/.config/etree/locales"
Confile_DIR="$HOME/.config/etree/"

versionS=$(curl -s https://raw.githubusercontent.com/rompelhd/Etree/main/version)

transletions="https://raw.githubusercontent.com/rompelhd/Etree/main/locales/"

system_lang=$(echo $LANG | cut -d'_' -f1)

ARCHITECTURE=$(uname -m)

commands=("curl" "tee")

archi=("x86_64" "aarch64" "i686" "armhf")

declare -A languages
languages["es"]="es.json"
languages["en"]="en.json"

FILENAME=${languages[$system_lang]}

echo -e "\n${greenColour}>=======>   >=>                                           >>                  >=>              >=>                     >=>                >=>  >=>"
echo -e ">=>         >=>                                          >>=>                 >=>              >=>                     >=>                >=>  >=>"
echo -e ">=>       >=>>==> >> >==>   >==>      >==>              >> >=>     >=>  >=> >=>>==>    >=>     >=> >==>>==>   >===>  >=>>==>    >=> >=>   >=>  >=>"
echo -e ">=====>     >=>    >=>    >>   >=>  >>   >=>           >=>  >=>    >=>  >=>   >=>    >=>  >=>  >=>  >=>  >=> >=>       >=>    >=>   >=>   >=>  >=>"
echo -e ">=>         >=>    >=>    >>===>>=> >>===>>=>         >=====>>=>   >=>  >=>   >=>   >=>    >=> >=>  >=>  >=>   >==>    >=>   >=>    >=>   >=>  >=>"
echo -e ">=>         >=>    >=>    >>        >>               >=>      >=>  >=>  >=>   >=>    >=>  >=>  >=>  >=>  >=>     >=>   >=>    >=>   >=>   >=>  >=>"
echo -e ">=======>    >=>  >==>     >====>    >====>         >=>        >=>   >==>=>    >=>     >=>     >=> >==>  >=> >=> >=>    >=>    >==>>>==> >==> >==>\n"
echo -e "                                                                                                                                       by rompelhd${endColour}\n"

for command in "${commands[@]}"; do
    if ! which $command >/dev/null 2>&1; then
        echo "$command is not installed"
    fi
done

if [[ " ${archi[@]} " =~ " $ARCHITECTURE " ]]; then
    binary="https://github.com/rompelhd/Etree/releases/download/$versionS/etree-$ARCHITECTURE-unknown-linux"
else
    echo "Error Setting a binary"
fi

if [ -n "$binary" ]; then
    echo -e "${blueColour}Downloading ${greenColour}Etree${purpleColour} $ARCHITECTURE \n${greenColour}"
    curl -O -J -L -# "$binary"
    mv etree-$ARCHITECTURE-unknown-linux /bin/etree
    chmod +x /bin/etree
else
    echo "Error downloading"
fi

if [ -z "$FILENAME" ]; then
    FILENAME="en.json"
    echo "Default language, language not found in environment variable $LANG"
fi

transletionsf="$transletions$FILENAME"

echo -e "\n${blueColour}Downloading${greenColour} Translation ${purpleColour}$FILENAME\n${greenColour}"
curl -O -J -L -# $transletionsf | sed -r 's/.{50}$//'

if [ -d "$Transletions_DIR" ]; then
    echo -e "\n${blueColour}The directory${greenColour} $Transletions_DIR${purpleColour} already exists.\n"
    mv $FILENAME $Transletions_DIR
else
    mkdir -p "$Transletions_DIR"
    if [ $? -eq 0 ]; then
        echo -e "\n${blueColour}Directory created in ${greenColour}$Transletions_DIR\n"
        mv $FILENAME $Transletions_DIR
    else
        echo -e "\nError creating directory in $Transletions_DIR\n"
        exit 1
    fi
fi

if [ -n "$FILENAME" ] && [ ! -f "$Confile_DIR/etree.conf" ]; then
    LANGUAGE=$(basename "$FILENAME" .json | cut -d '.' -f 1)
elif [ -n "$FILENAME" ]; then
    rm $Confile_DIR/etree.conf
    LANGUAGE=$(basename "$FILENAME" .json | cut -d '.' -f 1)
fi

echo -e "${blueColour}Creating ${greenColour}etree.conf\n"
echo "#Etree Config" | tee -a $Confile_DIR/etree.conf > /dev/null
echo "languageCode=$LANGUAGE" | tee -a $Confile_DIR/etree.conf > /dev/null

echo -e "${greenColour}>=======>"
echo -e ">=>        >>            >>         >=>"
echo -e ">=>           >==>>==>       >===>  >=>"
echo -e ">=====>   >=>  >=>  >=> >=> >=>     >=>>=>"
echo -e ">=>       >=>  >=>  >=> >=>   >==>  >=>  >=>"
echo -e ">=>       >=>  >=>  >=> >=>     >=> >>   >=>"
echo -e ">=>       >=> >==>  >=> >=> >=> >=> >=>  >=> \n"
