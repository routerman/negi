#!/bin/sh
#NEGIと機械学習プラットフォームJubatusのインストールを行うシェルスクリプト
#Postgresql9.1では文字列がうまく入らないため、8.4をインストールする。
#あらかじめgitをインストールしておき、root権限で実行する。
#
#git clone https://github.com/westlab/negi
#cd negi 
#./install.sh (Database user name) (Database name)

DBUSERNAME=$1
DBNAME=$2
if [ $# -ne 2 ]; then
  echo "指定された引数は$#個です。" 1>&2
  echo "実行するには2個の引数が必要です。" 1>&2
  exit 1
fi
apt-get update
apt-get install aptitude -y
echo "deb http://ftp.jp.debian.org/debian/ squeeze main" >> /etc/apt/sources.list
echo "deb http://download.jubat.us/apt binary/" >> /etc/apt/sources.list.d/jubatus.list
aptitude update
aptitude install git g++ libboost-dev libpqxx3-dev zlib1g-dev libpcap-dev libboost-all-dev postgresql-8.4 phppgadmin build-essential jubatus -y

sed -i -e s/ident/trust/ /etc/postgresql/8.4/main/pg_hba.conf
/etc/init.d/postgresql restart

psql -U postgres -c "create user $DBUSERNAME"
psql -U postgres -c "create database $DBNAME"
psql -U postgres -c "grant all privileges on database $DBNAME to $DBUSERNAME;"
psql -U $DBUSERNAME $DBNAME < ./template/script/negi.sql

make clean
make dep
make

#configuration pcap.conf

cat << END > ./template/config/pcap.conf
type pcap
filename test.pcap
dbname $DBNAME
dbuser $DBUSERNAME
dbpass 
dbhost localhost
gc_remove_time 600
END

#configuration eth1.conf
cat << END > ./template/config/eht0.conf
type ether
device eth0
dbname $DBNAME
dbuser $DBUSERNAME
dbpass 
dbhost localhost
gc_remove_time 600
END

. /opt/jubatus/profile


echo "オフラインモードで実行するにはtcpdump等でtest.pcapを用意し、./negi template/config/pcap.confを実行してください。"
echo "オンラインモードで実行するには./negi template/config/eth0.confを実行してください。"
