#!/usr/bin/perl -w

use strict;
use Math::Complex;

my @A;
my @B;

while (my $line = <>) {
    if ($line =~ /A:/) {
	@A = split " ", $line;
	shift @A;
	shift @A;
    }
    if ($line =~ /B:/) {
	@B = split " ", $line;
	shift @B;
	shift @B;
    }
}

my @CA;
my @CB;
make_compl(\@A, \@CA);
make_compl(\@B, \@CB);

my $l0 = 1;
my $l1 = 0;

my $bl = Math::Complex->new(0,0);
for (my $i = 0; $i < 3; ++$i) {
    for (my $j = 0; $j < 3; ++$j) {
	my $k = 3 * $i + $j;
	$bl += $CA[$k] * exp (2 * pi * i / 3 * ($i * $l0 + $j * $l1));
    }
}
print $bl . "\n";


sub make_compl
{
    my $A = shift;
    my $CA = shift;
    for (my $i = 0; $i < 9; ++$i) {
	$A->[$i] =~ s/\(//;
	$A->[$i] =~ s/\)//;
	my ($real, $imag) = split ",", $A[$i];
	$CA->[$i] = Math::Complex->new($real, $imag);
    }
}

