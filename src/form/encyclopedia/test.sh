#!/bin/sh
#set -x

for dir in component project race ship special_ops tech
do
	echo "Changing directory into $dir.....";
	cd $dir;

	for file in $(ls *.html)
	do
		sed 1,'$'s/'image.archspace.co.kr'/'$IMAGE_SERVER_URL'/g $file > $file.new;
		rm $file;
		mv $file.new $file;
	done

	echo "Coming back.....";
	cd ../;
done

