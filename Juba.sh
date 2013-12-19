#!/bin/sh

count=`ps -ef | grep jubaclassifier | grep -v grep | wc -l`
#count=`ps cax | grep mysqld | wc -l`
if [ $count = 0 ]; then
   echo "jubaclassifier is dead."
   jubaclassifier -f /opt/jubatus/share/jubatus/example/config/classifier/pa.jason &
else
   echo "jubaclassifier is alive."
fi

count=`ps -ef | grep jubarecommender | grep -v grep | wc -l`
#count=`ps cax | grep mysqld | wc -l`
if [ $count = 0 ]; then
   echo "jubarecommender is dead."
#	jubarecommender --configpath npb_similar_player.json 
else
    echo "jubarecommender is alive."
fi

./negi template/config/eth0.conf



