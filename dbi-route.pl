#!/usr/bin/perl
#  Perl script for loading the startup config file. 
#   or for extracting a portion of the startup file as "set" commands. 
#   example : $0 /tmp/config.test.boot firewall name foobar-in rule 5 


use strict;
use lib "/opt/vyatta/share/perl5/"; 
use Vyatta::ConfigLoad; 
use DBI;

my $DB_NAME = "testdb";
my $DB_HOST = "localhost";
my $DB_USER = "ozuma";
my $DB_PASSWD = "7PX-uwVX";

my $dbh = DBI->connect("dbi:Pg:dbname=$DB_NAME;host=$DB_HOST", $DB_USER, $DB_PASSWD) or die "$!\n Error: failed to connect to DB.\n";
my $sth = $dbh->prepare("SELECT userid,settime,status FROM tableA WHERE name = ? AND param = ?");
$sth->execute("Yasunaga", 201);

while (my $ary_ref = $sth->fetchrow_arrayref) {
  my ($userid, $settime, $status) = @$ary_ref;
  ....
  ....
}

$dbh->disconnect;

my $usage= "Usage: $0 [config-file-path] [config node]\n"; 
my $conf_file = '/opt/vyatta/etc/config/config.boot'; 
my $template_dir= '/opt/vyatta/share/vyatta-cfg/templates/'; 

i# if $ARGV[0] matches a top level config keyword, assume that it 
# is not a config filename. 

my @toplevelnodes= split(' ', `cd $template_dir && ls`); 
if (defined $ARGV[0]) { 
   my @isanode= grep(/^$ARGV[0]$/i, @toplevelnodes); 
   if (! @isanode) { 
      $conf_file = $ARGV[0]; 
      shift @ARGV; 
   } 
} 


# get a list of all config statement in the startup config file 
my @all_nodes = Vyatta::ConfigLoad::getStartupConfigStatements($conf_file); 
if (scalar(@all_nodes) == 0) { 
  # no config statements 
  exit 1; 
} 

foreach (@all_nodes) { 
  my ($path_ref) = @$_; 
  my $elements = scalar(@$path_ref); 
  my @nodes = @$path_ref[0 .. ($elements - 1)] ; 
  my $path = ''; 
  foreach (@nodes) { 
    if (/[\s\!\@\#\$\%\^\&\*\(\)\+\=\~\`]/) { 
       $path .= "$_ "; 
    } else { 
       my $tmp = $_; 
       $tmp =~ s/'//g; 
       $path .= "$tmp "; 
    } 
  } 
  print "set $path\n"; 
} 
print "commit\n";
