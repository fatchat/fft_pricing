#!/opt/local/bin/perl -w

use strict;
use Getopt::Std;

sub help
{
    print "usage: $0 [-h(elp)] -m <m-value> -n <n-value> -f <input file>\n";
    print "At least one value of m or n must be supplied.\n";
    exit(0);
}

my %opts = ();
getopts("m:n:hf:", \%opts);

help() if(exists $opts{h});

my $infile = $opts{f};
help() if not defined $infile;
my $mm = $opts{m};
my $nn = $opts{n};

my $outfile;
if (defined $mm) {
    $outfile = "${infile}.m.$mm";
}
elsif (defined $nn) {
    $outfile = "${infile}.n.$nn";
}
else {
    help();
}

open I, $infile or die "Could not open $infile for reading.\n";
open O, "> $outfile" or die "Could not open $outfile for writing.\n";
while (my $line = <I>) {
    my ($n, $m, $val) = split ' ', $line;
    last if(not defined $val);
    if (defined $mm and $m eq $mm) {
	print O "$n $val\n";
    }
    if (defined $nn and $n eq $nn) {
	print O "$m $val\n";
    }
}

