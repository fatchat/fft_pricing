#!/opt/local/bin/perl -w

use strict;
use Env;
use IPC::Open2;
use Getopt::Std;

my %opts = ();
getopts("hvg", \%opts);

sub help
{
    print "usage: $0 [-h(elp)] [-v(erbose)] [-g(nuplot output)]\n";
    exit(0);
}

help() if (exists $opts{h});
my $verbose = exists $opts{v};
my $gnuplot = exists $opts{g};

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
    'stock' => 50,
    'strike' => 40,
    'time' => 10,
    'vol' => 0.2,
    'alpha' => 1,
    'rate' => 0.1,
    'steps' => 400,
    'dpsi' => 0.2,
    'searchFor' => 91
    );

my $configFilename = "config.fteuro";
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
my $pid = open2(\*Reader, \*Writer, "$ENV{'PWD'}/bin/ft");

print Writer $params{'stock'} . "\n";
print Writer $params{'strike'} . "\n";
print Writer $params{'rate'} . "\n";
print Writer $params{'vol'} . "\n";
print Writer $params{'time'} . "\n";
print Writer $params{'alpha'} . "\n";
print Writer $params{'steps'} . "\n";
print Writer $params{'dpsi'} . "\n";
print Writer $params{'searchFor'} . "\n";

if ($gnuplot) {
    my $strike_filename = "gnuplot.strikes";
    my $logstrike_filename = "gnuplot.logstrikes";
    open STRIKE, "> $strike_filename"
	or die "Could not open $strike_filename for writing.\n";
    open LOGSTRIKE, "> $logstrike_filename"
	or die "Could not open $logstrike_filename for writing.\n";
    while (my $line = <Reader>) {
	next if ($line =~ /^\#/);
	my (undef, $strike, $log_strike, undef, undef, $fft_price)
	    = split ' ', $line;
	if (defined $fft_price) {
	    print STRIKE "$strike $fft_price\n";
	    print LOGSTRIKE "$log_strike $fft_price\n";
	}
    }
}    
else {
    while (my $line = <Reader>) {
	print $line;
    }
}


waitpid($pid, 0);
