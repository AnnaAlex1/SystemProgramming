#! /bin/bash


file="citizenRecordsFile.txt"

if [ -f "$file" ] ; then
    rm "$file"
fi

#echo $0 $1 $2 $3 $4 ' ( virusesFile countriesFile numLines duplicatesAllowed  )'

virusesFile=$1
countriesFile=$2
numLines=$3
duplicatesAllowed=$4

: 'echo $virusesFile        #print arguments
echo "$countriesFile"
echo "$numLines"
echo "$duplicatesAllowed"'


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

#read viruses file
viruses=(`cat "$virusesFile"`)
#mapfile -t viruses <$virusesFile

: 'for vir in ${viruses[@]}; do
    echo "$vir"
done'

#read countries file
countries=(`cat "$countriesFile"`)
#mapfile -t countries <$countriesFile
: 'for coun in ${countries[@]}; do
    echo "$coun"
done '


#########################   FUNCTIONS TO BE USED



table="abcdefghijklmnopqrstuvwxyz"


function create_citizen() {

    #choose citizenID
    if [ $duplicatesAllowed == 0 ]
    then
        id="${ids_table[$counter]}"
    else
        let "id = $RANDOM % $id_range + 1"
    fi
    
    #echo "CitizenID length: " $id



    #choose firstname
    let "fn_length = $RANDOM % $name_range + 3"
    #echo "firstname length: " $fn_length

    first_n=$(shuf -zer -n$fn_length {a..z} | tr -d '\0')

    first_n=${first_n^}
    #echo "$first_n"




    #choose lastname
    let "ln_length = $RANDOM % $name_range + 3"
    last_n=$(shuf -zer -n$ln_length {a..z} | tr -d '\0')

    last_n=${last_n^}
    #echo "$last_n"
    


    #choose age
    let "age = $RANDOM % $age_range + 1"
    #echo "Age is: " $age




    #choose country
    let "pos = $RANDOM % ${#countries[@]}"
    #echo "$pos"
    country=${countries[$pos]}
    #echo "$country"


    new_citizen="$id $first_n $last_n $country $age"
    #echo "$new_citizen"



}



function create_rest() {


    #choose virus
    #echo "Size of array of viruses: " "${#viruses[@]}" 
    let "pos = $RANDOM % ${#viruses[@]}"
    #echo "$pos"
    virus=${viruses[$pos]}
    #echo "$virus"

    #decide "YES" or "NO"
    let "ran = $RANDOM % 2"
    if [ $ran -eq 0 ]
    then
        vac="NO"
    else
        vac="YES"
    fi

    #echo "$vac"
    let "give_date = $RANDOM % 10"      #probability for providing a date 

    if ([ $vac == "YES" ] && [ $give_date -ge 1 ]) || ([ $vac == "NO" ] && [ $give_date -lt 1 ])     
    # probability of 90% for YES, probability of 10% for NO
    then
        
        #calculate date
        let "day = $RANDOM % $day_range + 1"
        let "month = $RANDOM % $month_range + 1"
        let "year = $RANDOM % ($year_high - $year_low) + $year_low"
        date=$day-$month-$year
        #echo "$date"

    else
        date=""
    fi



    #echo "counter: $counter"

    vir_vacc="$virus $vac $date"
    #echo "$vir_vacc"


}

########################################################3







#dates
day_range=30
month_range=12
year_low=1900
year_high=2100
age_range=120
name_range=10
id_range=9999

ids_table=()
#filling the table with numbers from 0 to 9999
if [ $duplicatesAllowed == 0 ]  #case: duplicates not allowed
then
    declare -i counter=0
    while [ $counter -le $id_range ]     # 9999 is the maximum number for ID
    do
        ids_table[${#ids_table[@]}]=$counter
        counter+=1

    done

    #sort table
    ids_table=($(shuf -e "${ids_table[@]}"))
    #echo "${ids_table[@]}"

fi




#3.CREATE INPUT FILE

my_citizens=()
declare -i counter=0

while [ $counter -lt $numLines ]
do


    if [ $duplicatesAllowed == 1 ]   #CASE: DUPLICATES ALLOWED
    then

        let "dupl_prob = $RANDOM % 10"

        if [ $dupl_prob -ge 1 ] || [ $counter -eq 0 ]         # create a new citizen with probability of 90%
        then
            #echo "Case: Duplicates allowed, but this is a new Citizen"
            create_citizen
            create_rest

            full="$new_citizen $vir_vacc"

            my_citizens[${#my_citizens[@]}]=$new_citizen
            

        else                            # choose an already existing citizen with probability of 10%
            #echo "Case: Duplicates allowed and choosing an already existing citizen"

            let "array_pos = $RANDOM % ${#my_citizens[@]}"
            #echo "$array_pos"

            create_rest

            full="${my_citizens[$array_pos]} $vir_vacc"
            #echo "${#my_citizens[@]}"
        fi


    else                            #CASE: NO DUPLICATES ALLOWED

        if [ $counter -gt 9999 ]
        then break
        fi

        create_citizen $counter
        create_rest
        full="$new_citizen $vir_vacc"

    fi

    echo $full >> citizenRecordsFile.txt

    counter+=1

done


#echo "${my_citizens[@]}"


