#!/usr/bin/perl
#CPAN
#cpan -i DBI
#capn -i DBD::Pg
use strict;
use warnings;
use DBI;

my ($dbh, $sth);
my ($data_source, $user_name, $sql);

$data_source = 'dbi:Pg:dbname=routermandb';
$user_name = 'routerman';
$dbh = DBI->connect($data_source, $user_name) || die $dbh->errstr;

$sql = "SELECT src_ip FROM action_count WHERE host='202.255.47.226' order by score limit 5";
$sth = $dbh->prepare($sql) || die $dbh->errstr;
$sth->execute() || die $sth->errstr;

my $src_ip;
while ( $src_ip = $sth->fetchrow_array()) {
   print "$src_ip\n";
   system "ip rule add from $src_ip table 100 prio 200";
   #system "ip route add dev eth0 src $src_ip table 100";
}
die $sth->errstr if $sth->err;

#system "ip route get ADDRESS";
system "route";
$sth->finish();
$dbh->disconnect();
