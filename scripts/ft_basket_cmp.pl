#!/usr/bin/perl -w 

use strict;

my $configFilePrefix = "configfiles/config.basket_cmp";

my $var = "S_0";
my $initialVal = 90;
my $stepVal = 1;
my $lastVal = 110;

my %cfgVars = ();
open I, "$configFilePrefix" or die "could not open $configFilePrefix\n";
while (my $line = <I>) {
    my ($key, $value) = split ' ', $line;
    if (defined $value) {  $cfgVars{$key} = $value;  }
}
close I;

my $runCmdBase = "bin/ft_basket_cmp";

for (my $i = $initialVal; $i < $lastVal; $i += $stepVal) {
    my $cfgFile = "${configFilePrefix}.$i";
    open O, "> $cfgFile" or die "Could not open $cfgFile for writing\n";
    foreach my $key (keys %cfgVars) {
	if ($key eq $var) {   print O "$key $i\n";	}
	else {	    print O "$key $cfgVars{$key}\n";	}
    }
    close O;
    my $runCmd = "$runCmdBase -c $cfgFile";
    open E, "$runCmd |" or die "could not exec $runCmd\n";
    my $input = <E>;
    close E;
    unlink $cfgFile;
    my (undef, $basket, $ft) = split ' ', $input;
    print "$i $basket $ft\n";
}
