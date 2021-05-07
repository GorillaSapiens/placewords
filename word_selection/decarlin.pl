#!/usr/bin/perl

# George Carlin's Seven Dirty Words
@seven = (
      "shit",
      "piss",
      "fuck",
      "cunt",
      "cocksucker",
      "motherfucker",
      "tits" );

while (<>) {
   $score = 0;
   foreach $word (@seven) {
      if ($_ =~ /$word/) {
         $score++;
      }
   }
   if (!$score) {
      print $_;
   }
}
