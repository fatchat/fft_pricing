#!/opt/local/bin/perl -w

use strict;
use Getopt::Std;

my %opts = ();
getopts("h", \%opts);

sub help
{
    print "usage: $0 [-h(elp)]\n";
    exit(0);
}

help() if exists $opts{h};

