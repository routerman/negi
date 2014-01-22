#!/bin/sh
make
count=`ps -ef | grep jubaclassifier | grep -v grep | wc -l`
if [ $count = 0 ];
then
   echo "jubaclassifier is dead."
   jubaclassifier --configpath template/config/jubatus/config-classifier.json &
   until [ $count = 0 ]; 
   do
      count=`ps -ef | grep jubaclassifier | grep -v grep | wc -l`
   done
else
   echo "jubaclassifier is alive."
fi



count=`ps -ef | grep jubarecommender | grep -v grep | wc -l`
if [ $count = 0 ];
then
   echo "jubarecommender is dead."
	jubarecommender --configpath template/config/jubatus/config-recommender.json --rpc-port 19199 &
   until [ $count = 0 ]; 
   do
      count=`ps -ef | grep jubarecommender | grep -v grep | wc -l`
   done
else
   echo "jubarecommender is alive."
fi

./negi template/config/pcap.conf



