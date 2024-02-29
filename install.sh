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

attempts=0

transletions="https://raw.githubusercontent.com/rompelhd/Etree/main/locales/"

echo -e "\n${greenColour}>=======>   >=>                                           >>                  >=>              >=>                     >=>                >=>  >=>"
echo -e ">=>         >=>                                          >>=>                 >=>              >=>                     >=>                >=>  >=>"
echo -e ">=>       >=>>==> >> >==>   >==>      >==>              >> >=>     >=>  >=> >=>>==>    >=>     >=> >==>>==>   >===>  >=>>==>    >=> >=>   >=>  >=>"
echo -e ">=====>     >=>    >=>    >>   >=>  >>   >=>           >=>  >=>    >=>  >=>   >=>    >=>  >=>  >=>  >=>  >=> >=>       >=>    >=>   >=>   >=>  >=>"
echo -e ">=>         >=>    >=>    >>===>>=> >>===>>=>         >=====>>=>   >=>  >=>   >=>   >=>    >=> >=>  >=>  >=>   >==>    >=>   >=>    >=>   >=>  >=>"
echo -e ">=>         >=>    >=>    >>        >>               >=>      >=>  >=>  >=>   >=>    >=>  >=>  >=>  >=>  >=>     >=>   >=>    >=>   >=>   >=>  >=>"
echo -e ">=======>    >=>  >==>     >====>    >====>         >=>        >=>   >==>=>    >=>     >=>     >=> >==>  >=> >=> >=>    >=>    >==>>>==> >==> >==>\n"
echo -e "                                                                                                                                       by rompelhd${endColour}\n"

ARCHITECTURE=$(uname -m)

if ! which curl >/dev/null 2>&1; then
    echo "Curl is not installed"
fi

if [ "$ARCHITECTURE" == "x86_64" ]; then
    binary="https://github.com/rompelhd/Etree/releases/download/Etree_x86_64/etree"
    arch="x86_64"
elif [ "$ARCHITECTURE" == "armhf" ]; then
    binary="https://github.com/rompelhd/Etree/releases/download/Etree_arm/etree"
    arch="armhf"
elif [ "$ARCHITECTURE" == "arm64" ]; then
    binary="https://github.com/rompelhd/Etree/releases/download/Etree_arm64/etree"
    arch="arm64"
elif [ "$ARCHITECTURE" == "i686" ]; then
    binary="https://github.com/rompelhd/Etree/releases/download/Etree_i686/etree"
    arch="i686"
else
    echo "Unsupported arch"
    exit 1
fi

if [ -n "$binary" ]; then
    echo -e "${blueColour}Downloading etree ${greenColour}$arch \n${greenColour}"
    curl -O -J -L "$binary"
    chmod +x etree
    mv etree /bin/
else
    echo "Error downloading"
fi

while [ $attempts -lt 3 ]; do
    echo -ne "${endColour}\n${blueColour}Which language would you like to download?${turquoiseColour} Type ${greenColour}'es'${blueColour}${turquoiseColour} or ${greenColour}'en${greenColour}'${turquoiseColour}: "
    read LANGUAGE

    case $LANGUAGE in
        "es")
            FILENAME="es.json"
            break
            ;;
        "en")
            FILENAME="en.json"
            break
            ;;
        *)
            echo "\nInvalid language. Please select 'es' for Spanish or 'en' for English."
            attempts=$((attempts+1))
            ;;
    esac
done

if [ $attempts -eq 3 ]; then
    echo "You have exceeded the maximum number of attempts. Exiting..."
    exit 1
fi

transletionsf="$transletions$FILENAME"

echo -e "\n${blueColour}Downloading file ${greenColour}$FILENAME ${blueColour}...\n${greenColour}"
curl -O -J -L $transletionsf

if [ -d "$Transletions_DIR" ]; then
    echo -e "\n${blueColour}The directory ${greenColour} $Transletions_DIR${blueColour} already exists."
    echo -e "\n${blueColour}Copying translations\n"
    mv $FILENAME $Transletions_DIR
else
    mkdir -p "$Transletions_DIR"
    if [ $? -eq 0 ]; then
        echo -e "\n${blueColour}Directory created in ${greenColour}$Transletions_DIR"
        echo -e "\n${blueColour}Copying translations\n"
        mv $FILENAME $Transletions_DIR
    else
        echo -e "\nError creating directory in $Transletions_DIR\n"
        exit 1
    fi
fi

echo -e "${greenColour}>=======>"
echo -e ">=>        >>            >>         >=>"
echo -e ">=>           >==>>==>       >===>  >=>"
echo -e ">=====>   >=>  >=>  >=> >=> >=>     >=>>=>"
echo -e ">=>       >=>  >=>  >=> >=>   >==>  >=>  >=>"
echo -e ">=>       >=>  >=>  >=> >=>     >=> >>   >=>"
echo -e ">=>       >=> >==>  >=> >=> >=> >=> >=>  >=> \n"
