#!/usr/bin/perl

while (<>) {
   s/[\x0a\x0d]//g;
   s/ .*//g;
   $tmp = $_;
   $tmp =~ s/[^a-z]//g;
   if ($tmp eq $_) {
      print "$tmp\n";
   }
}
