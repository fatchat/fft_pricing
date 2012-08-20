#!/opt/local/bin/perl -w

use strict;
use Getopt::Std;

my %opts = ();
getopts("hc:", \%opts);

sub help
{
    print "usage: $0 [-h(elp)] [-c config_file]\n";
    exit(0);
}

help() if exists $opts{h};

sub genMiddle
{
    my $configFile = shift;
    my $middle = "";
    if (defined $configFile) {
	if (! -e $configFile) {
	    print "Error: $configFile does not exist.\n";
	    exit (1);
	}
	open INPUT, "$configFile"
	    or die "Could not open $configFile for reading, quitting.\n";

	while (my $line = <INPUT>) {
	    next if ($line =~ /^#/);
	    my ($type, $name, $prompt, $default) = split ' ', $line;
	    if (defined $default) {
		$middle .= qq^
    const $type $name = get_value_from_config_file<$type>("$prompt: ", $default);^;
	    }
	}
    }
    return $middle;
}

my $middle = genMiddle($opts{c});

my $top = <<TOP;
#include <iostream>

#include "csutils/utils.h"
#include "csutils/globals.h"
#include "csutils/get_opt.h"
#include "csutils/configfile.h"

void showHelp(const char* progname)
{
    std::cout << "usage: " << progname
	      << " [-h(elp)]"
	      << " [-v(erbose)]"
	      << " [-c <config file name>]"
	      << "\\n"
      ;
}

int main(int argc, char* argv[])
{
    Getopt getopt;
    getopt.addOption("help", Option::NO_ARG, 'h');
    getopt.addOption("verbose", Option::NO_ARG, 'v');
    getopt.addOption("configfile", Option::REQ_ARG, 'c');
    if (getopt.processOpts(argc, argv)) {
        showHelp(argv[0]);
        return 1;
    }
    if (getopt.getOption('h').is_set()) {
        showHelp(argv[0]);
        return 0;
    }
    const bool verbose = getopt.getOption('v').is_set();

    const std::string cfgFile(getopt.getOption('c').is_set() ?
			      getopt.getOption('c').arg() :
			      "configfiles/config.");
    ConfigFile::get_mutable_instance().init(cfgFile);

TOP
    my $bottom = <<BOTTOM;

    return 0;
}
BOTTOM

    print "${top}${middle}${bottom}";
