#!/usr/bin/perl

# George Carlin's Seven Dirty Words
# and a few more
@dirty = (
      "shit",
      "piss",
      "fuck",
      "cunt",
      "cocksucker",
      "motherfucker",
      "tits",
      "asshole" );

while (<>) {
   $score = 0;
   foreach $word (@dirty) {
      if ($_ =~ /$word/) {
         $score++;
      }
   }
   if (!$score) {
      print $_;
   }
}
