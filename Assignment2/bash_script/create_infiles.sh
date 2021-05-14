#! /bin/bash


#echo $0 $1 $2 $3 ' ( inputFile input_dir numFilesPerDir )'

inputFile=$1
input_dir=$2
numFilesPerDir=$3

: 'echo $inputFile        #print arguments
echo "$input_dir"
echo "$numFilesPerDir"'


#1.CHECK INPUT

#check number of inputs
if [ "$#" -ne "3" ]; then
	echo "Wrong number of arguments!"
	exit
fi


#check if files exist
if test -f "$inputFile"; then
    echo "File $inputFile found!"
else
    echo "File $inputFile doesn't exist."
    exit
fi


#check number of files per Directory
if [ $numFilesPerDir -lt 1 ]
then
    echo "Please give a positive integer!"
    exit  
fi



#check if directory already exists
if [ -d "$input_dir" ]
then
    echo "Sorry, $input_dir already exists."
    exit
fi

#Create dir
mkdir $input_dir
echo "Input Folder created!"


########################## 
#2.READ CITIZENRECORDSFILE

#citizens=(`cat "$inputFile"`)

mapfile -t citizens <$inputFile

N=4         #to get fourth word -> country


declare -i num_of_countries=0
declare -A file_number

for (( i = 0 ; i < ${#citizens[@]} ; i++))
do
    #echo "${citizens[$i]}" | cut -d " " -f $N

    country=`echo "${citizens[$i]}" | cut -d " " -f $N`         #fourth element of each line
    #echo $country

    fullpath="$input_dir/$country"
    #echo $fullpath

    if [ ! -d "$fullpath" ]                         #if country folder doesn't exist
    then                            #NEW COUNTRY    
        mkdir $fullpath                 # create Country Folder
        num_of_countries+=1

        for (( j = 0 ; j < $numFilesPerDir ; j++))  #Fill with files
        do
            filename="$fullpath/$country-$j.txt"
            touch $filename
            #echo $filename
        done

        file_number[$country]=0             #initialization of array that keeps number of file

    fi 
    
done



echo "${#citizens[@]}"
echo "Number of Countries: " $num_of_countries

#echo "${file_number[@]}"



for (( i = 0 ; i < ${#citizens[@]} ; i++))      #FOR EVERY RECORD
do
    
    #current_rec=`echo "${citizens[$i]}" | cut -d " " -f 1-3, 5-`
    country=`echo "${citizens[$i]}" | cut -d " " -f $N`
    #echo $country
    current_rec=`echo ${citizens[$i]} | cut -d " " -f 1-3,5-`
    #echo $current_rec

    
    #echo "Number of file to be put: " ${file_number[$country]}
    #echo "FILENAME: " "$input_dir/$country/$country-${file_number[$country]}.txt"

    echo $current_rec >> "$input_dir/$country/$country-${file_number[$country]}.txt"

    ((file_number[$country]=(file_number[$country]+1) % $numFilesPerDir ))    #calculate number of next file to be used



done

