#!/bin/bash 

# Auto checker script for Scilab Textbook Companion
# http://scilab.in/Textbook_Companion_Project

# Original author: Lavitha Pereira, Sachin

# This file is part of tbc-auto-checker.
# tbc-auto-checker is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
# tbc-auto-checker is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with tbc-auto-checker.  If not, see <http://www.gnu.org/licenses/>.

# NOTE: scilab version 5.4.0 or higher is recommended.


# Set your scilab path here
# export SCIHOME='/var/www/html/scilab_in_2015/uploads/auto_check/scilab/'
# export SCI_PATH="/var/www/html/scilab_in_2015/uploads/auto_check/scilab/bin/scilab-adv-cli"

if [ -d $2 ]; then
            rm -rf $2
fi
# mkdir -p $2
( umask 000 && mkdir -p $2 )
# chmod -R 777 $2
# set where to store graph images
#SCI_GRAPH_PATH="/var/www/html/scilab_in_2015/uploads/auto_check/$2/tbc_graphs"

echo "<br />Hello $USER, Welcome to automatic code check for Scilab textbook companion."
echo "<br />The Date & Time is:" `date`
# echo ""

#touch ./1202/output_1202.log;
#touch ./1202/output_graph_1202.log;
# exit 1
SAVEIFS=$IFS			# save value of IFS, if any
IFS=$(echo -en "\n\b\:\,")	# define Internal Field Seperator(for
# directory names with spaces)
# echo "k"

function scan_sce_for_errors {
	# Generate output log, error log of both text and graphical based output
	# also generate graphs(currently only plots graphs which has plot*(*) function)
	
	#unzip $1			# unzip file
	#wait
# 	echo 'f'
	ZIPFILE=$1
	ZIPFILE_NAME=$2
	#find /$ZIPFILE/DEPENDENCIES/ -iname '*.sci' -exec cp {} /PENDING/CH*/EX*/ \;
	
	
	# make a list of all .sce file(with complete path). Exclude
	# scanning 'DEPENDENCIES' DIR
	
	# That awk command says the field separator FS is set to /; this
	# affects the way it reads fields. The output field separator OFS
	# is set to "."; this affects the way it prints records. This is
	# intentionally done so that 'cut' will take "." as a field
	# Seperator during final sort operation. The next statement says
	# print the last column (NF is the number of fields in the record,
	# so it also happens to be the index of the last field) as well as
	# the whole record ($0 is the whole record); it will print them
	# with the OFS between them. Then the list is sorted, treating _
	# as the field separator - since we have the filename first in the
	# record, it will sort by that. Then the cut prints only fields 3
	# through the end, again treating "." as the field separator.
	
	SCE_FILE_LIST=$(find ${ZIPFILE} -type f -iname "*.sce" -o -iname "*.sci" ! \
	-path "*/DEPENDENCIES*" | \
	awk -vFS=/ -vOFS="." '{print $NF,$0}' | \
	sed 's/^[[:upper:]]*//g' | \
	sed 's/^[[:lower:]]*//g' | \
	sort -n -t _ -k1 -k2 | cut -d"." -f3-);
	

	
	SCE_FILE_COUNT=$(echo "${SCE_FILE_LIST}" | wc -l)
	echo -e "<br />Total number of .sce files(without counting DEPENDENCIES directory): ${SCE_FILE_COUNT}\n" >> ./${ZIPFILE_NAME}/output_${ZIPFILE_NAME}.log
	
	# make directory for storing graphs(each dir will be named after a book)
# 	mkdir -p ${SCI_GRAPH_PATH}/${ZIPFILE_NAME}
	( umask 000 && mkdir -p ${SCI_GRAPH_PATH}/${ZIPFILE_NAME} )
# 	chmod 777 ${SCI_GRAPH_PATH}/${ZIPFILE_NAME}
	
	for sce_file in ${SCE_FILE_LIST};
	do
		
		#CAT_FILE=$(egrep -r "plot\S[0-9]?[d]?[0-9]?[(]*[)]*" ${sce_file})
		CAT_FILE=$(egrep -r "subplot\S[0-9]?[d]?[0-9]?[(][)]*|clf\S[0-9]?[d]?[0-9]?[(][)]|plot\S[0-9]?[d]?[0-9]?[(][)]|gca\S[0-9]?[d]?[0-9]?[(][)]|gcf\S[0-9]?[d]?[0-9]?[(][)]*|figure|plot|plot2d|plot3d" ${sce_file})
		
		
		
		#echo ${CAT_FILE}
		if [ -z "${CAT_FILE}" ];
		then
			BASE_FILE_NAME=$(basename ${sce_file} .sce)
			
			echo "<br />--------- Text output file --------------"
			echo "<br />------------- ${sce_file}  --------------"
			
# 			echo "" >> ${sce_file}
# 			echo "" >> ${BASE_FILE_NAME}
			
			echo "exit();" >> ${sce_file} 
			sed -i '1s/^/mode(2);/' ${sce_file}
			sed -i '1s/^/errcatch(-1,"stop");/' ${sce_file}
			sed -i 's/xdel(winsid());//g' ${sce_file}
			sed -i 's/clc()//g' ${sce_file}
			sed -i 's/clc//g' ${sce_file}
			sed -i 's/clc,//g' ${sce_file} #sri
			sed -i 's/clc();//g' ${sce_file} #sri
			sed -i 's/close;//g' ${sce_file}
			sed -i 's/close();//g' ${sce_file} #sri
			sed -i 's/close()//g' ${sce_file} #sri
			sed -i 's/close//g' ${sce_file}
			sed -i 's/clear//g' ${sce_file}
			sed -i 's/clear()//g' ${sce_file} #sri
			sed -i 's/clear();//g' ${sce_file} #sri
			sed -i 's/clear\ all//g' ${sce_file} #sri
			sed -i 's/clear\ all;//g' ${sce_file} #sri
			sed -i 's/clear\ all,//g' ${sce_file} #sri
			sed -i 's/clear(),//g' ${sce_file} #sri
			sed -i 's/clf()//g' ${sce_file}
			sed -i 's/clf();//g' ${sce_file}
			sed -i 's/clf;//g' ${sce_file}
			
			# run command
			OUTPUT=`scilab-cli -nb -nwni -f ${sce_file}`;
# 			OUTPUT=$(${SCIHOME}/bin/scilab-cli -nb -nwni -f ${sce_file});
			echo "<br />"
			echo $OUTPUT
			if [[ "${OUTPUT}" =~ "!--error" ]];
			then
				echo "ERROR: ${sce_file}" >> ./${ZIPFILE_NAME}/error_${ZIPFILE_NAME}.log
				echo "${OUTPUT}" >> ./${ZIPFILE_NAME}/error_${ZIPFILE_NAME}.log
			else
				echo "################# ${sce_file} #####################" >> ./${ZIPFILE_NAME}/output_${ZIPFILE_NAME}.log
				echo "${OUTPUT}" >> ./${ZIPFILE_NAME}/output_${ZIPFILE_NAME}.log
			fi
			unset OUTPUT
			unset BASE_FILE_NAME
		else
			echo "<br />-------- Graph file -----------"
			echo "<br />--------${sce_file}------------"
# 			echo "" >> ${sce_file}
			BASE_FILE_NAME=$(basename ${sce_file} .sce)
			# change path for storing graph image file
			echo "xinit('${SCI_GRAPH_PATH}/${ZIPFILE}/${BASE_FILE_NAME}');xend();exit();" >> ${sce_file} 
# 			sed -i '1s/^/mode(2);errcatch(-1,"stop");driver("GIF");/' ${sce_file}
# 			sed -i 's/xdel(winsid());//g' ${sce_file}
# 			sed -i 's/clc()//g' ${sce_file}
# 			sed -i 's/close;//g' ${sce_file}
# 			sed -i 's/clf;//g' ${sce_file}
			# run command
			OUTPUT=`scilab-cli -f ${sce_file}`;
			echo "<br />"
			echo ${OUTPUT}
			if [[ "${OUTPUT}" =~ "error" ]];
			then
				echo "ERROR: ${sce_file}" >> ./${ZIPFILE_NAME}/error_graph_${ZIPFILE_NAME}.log
				echo "${OUTPUT}" >> ./${ZIPFILE_NAME}/error_graph_${ZIPFILE_NAME}.log
			else
				echo "###################### ${sce_file} ###################" >> ./${ZIPFILE_NAME}/output_graph_${ZIPFILE_NAME}.log
				echo "${OUTPUT}" >> ./${ZIPFILE_NAME}/output_graph_${ZIPFILE_NAME}.log
			fi
			unset OUTPUT
			unset BASE_FILE_NAME
		fi
	done
}

scan_sce_for_errors $1 $2
# echo 'ok'
IFS=$SAVEIFS	
# restore value of IFS

#----end of auto.sh----#


