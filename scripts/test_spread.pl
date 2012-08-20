#!/opt/local/bin/perl -w

use strict;
use Env;
use IPC::Open2;
use Getopt::Std;

my %opts = ();
getopts("hv", \%opts);

sub help
{
    print "usage: $0 [-h(elp)] [-v(erbose)]\n";
    exit(0);
}

help() if (exists $opts{h});
my $verbose = exists $opts{v};

sub readParamsFromConfigFile
{
    my $configFilename = shift;
    my $params = shift;
    open CFG, $configFilename
	or die "Could not open $configFilename for reading.\n";
    while (my $line = <CFG>) {
	chop $line;
	my ($key, $value) = split ('=', $line);
	if (defined $key and defined $value) {
	    $params->{$key} = $value;
	}
    }
}

my %params = (
    'stock_1' => 50,
    'stock_2' => 60,
    'strike' => 40,
    'rate' => 0.1,
    'vol_1' => 0.2,
    'vol_2' => 0.2,
    'rho' => 0.5,
    'time' => 5,
    'e1' => -2.5,
    'e2' => 1,
    'steps' => 400,
    );

my $configFilename = "config.spreads";
if (-e $configFilename) {
    readParamsFromConfigFile($configFilename, \%params);
}
else {
    print "Could not find $configFilename, using default values.\n";
}

if ($verbose) {
    for my $key (keys %params) {
	print "$key ==> $params{$key}\n";
    }
}

local (*Reader, *Writer);
my $pid = open2(\*Reader, \*Writer, "$ENV{'PWD'}/bin/spread_test");

print Writer $params{'stock_1'} . "\n";
print Writer $params{'stock_2'} . "\n";
print Writer $params{'strike'} . "\n";
print Writer $params{'rate'} . "\n";
print Writer $params{'vol_1'} . "\n";
print Writer $params{'vol_2'} . "\n";
print Writer $params{'rho'} . "\n";
print Writer $params{'time'} . "\n";
print Writer $params{'e1'} . "\n";
print Writer $params{'e2'} . "\n";
print Writer $params{'steps'} . "\n";

while (my $line = <Reader>) {
    print $line;
}

waitpid($pid, 0);
