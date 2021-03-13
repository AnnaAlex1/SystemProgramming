#! /bin/bash

#echo $0 $1 $2 $3 $4 ' ( virusesFile countriesFile numLines duplicatesAllowed  )'

virusesFile=$1
countriesFile=$2
numLines=$3
duplicatesAllowed=$4

echo $virusesFile
echo $countriesFile
echo $numLines
echo $duplicatesAllowed


#1.CHECK INPUT

#check number of inputs
if [ "$#" -ne "4" ]; then
	echo "Wrong number of arguments!"
	exit
fi


#check if files exist
if test -f "$virusesFile"; then
    echo "$virusesFile exists."
else
    echo "$virusesFile doesn't exist."
    exit
fi

if test -f "$countriesFile"; then
    echo "$countriesFile exists."
else
    echo "$countriesFile doesn't exist."
    exit
fi


#2.READ FILES

echo " "
#read viruses file
viruses=(`cat "$virusesFile"`)

: 'for vir in ${viruses[@]}; do
    echo "$vir"
done'

#read countries file
countries=(`cat "$countriesFile"`)

: 'for coun in ${countries[@]}; do
    echo "$coun"
done '



#dates
day_range=30
month_range=12
year_low=1900
year_high=2100
age_range=120
name_range=10
id_range=9999



#3.CREATE INPUT FILE

counter=0
while [ $counter -lt $numLines ]
do
    #choose citizenID
    let "id = $RANDOM % $id_range + 1"
    #echo "CitizenID length: " $id




    #choose firstname
    let "fn_length = $RANDOM % $name_range + 3"
    #echo "firstname length: " $fn_length

    first_n=$(tr -cd a-z </dev/urandom | head -c $fn_length)
    first_n=${first_n^}
    #echo $first_n




    #choose lastname
    let "ln_length = $RANDOM % $name_range + 3"
    #echo "lastname length: " $ln_length

    last_n=$(tr -cd a-z </dev/urandom | head -c $ln_length)
    last_n=${last_n^}
    #echo $last_n
    


    #choose age
    let "age = $RANDOM % $age_range + 1"
    #echo "Age is: " $age




    #choose country
    #echo "Size of array of countries: " "${#countries[@]}" 

    let "pos = $RANDOM % ${#countries[@]}"
    #echo $pos
    country=${countries[$pos]}
    #echo $country



    #choose virus
    #echo "Size of array of viruses: " "${#viruses[@]}" 
    let "pos = $RANDOM % ${#viruses[@]}"
    #echo $pos
    virus=${viruses[$pos]}
    #echo $virus

    #decide "YES" or "NO"
    let "ran = $RANDOM % 2"
    if [ $ran -eq 0 ]
    then
        vac="NO"
    else
        vac="YES"
    fi

    #echo $vac

    if [ $vac == "YES" ]
    then
        
        #calculate date
        let "day = $RANDOM % $day_range + 1"
        let "month = $RANDOM % $month_range + 1"
        let "year = $RANDOM % ($year_high - $year_low) + $year_low"
        date=$day-$month-$year
        #echo $date

    else
        date=""
    fi



    #echo "counter: $counter"

    full="$id $first_n $last_n $country $age $virus $vac $date"

    echo $full >> citizenRecordFile.txt
    echo ''; echo ''

    counter=$(( counter+1 ))
done



