#!/usr/bin/perl
use strict;
use warnings;
use DBI;
use lib "/opt/vyatta/share/perl5/"; 
#use Vyatta::ConfigLoad; 

my $DB_NAME = "routermandb";
my $DB_HOST = "localhost";
my $DB_USER = "routerman";
my $DB_PASSWD = "routerman";

my $dbh = DBI->connect("dbi:Pg:dbname=$DB_NAME;host=$DB_HOST", $DB_USER, $DB_PASSWD) or die "$!\n Error: failed to connect to DB.\n";
print "connected!\n";

my $sth = $dbh->prepare("SELECT access_day FROM action_count WHERE class='Good'");
$sth->execute;

while (my $ary_ref = $sth->fetchrow_arrayref) {
	my $src_ip = @$ary_ref;
	print "SRC_IP=$src_ip is Good User\n";
}

#$dbh->commit;
$dbh->disconnect;

pod
my $usage= "Usage: $0 [config-file-path] [config node]\n"; 
my $conf_file = '/opt/vyatta/etc/config/config.boot'; 
my $template_dir= '/opt/vyatta/share/vyatta-cfg/templates/'; 

# if $ARGV[0] matches a top level config keyword, assume that it 
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
=cut
