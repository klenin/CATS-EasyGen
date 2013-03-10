#!/usr/bin/perl

use CATS::FormalInput;

local $/;
open FILE, "format.fmt";
$format_description = <FILE>;
close FILE;
my $input_filename = "input.txt";

my $error = CATS::FormalInput::ValidatorValidate(
    $input_filename, $format_description);

if ($error) {
   my $message = CATS::FormalInput::ValidatorErrorGetErrorMessage($error);
   my $line = CATS::FormalInput::ValidatorErrorGetErrorLine($error);
   my $pos = CATS::FormalInput::ValidatorErrorGetErrorPos($error);
   print "($line:$pos) $message\n";
} else {
    print "OK\n";
}
