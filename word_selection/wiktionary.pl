#!/usr/bin/perl

$nnn = 0;
$rrr = 0;

@parts = (
      "Noun", "Verb", "Adverb", "Adjective", "Pronoun",
      "Preposition"
      );

@bad = (
      "Translingual", "plural_of", "homophones", "Proper noun",
      "Abreviation of", "Initialism of", "letter-case"
      );

while (<>) {
   s/[\x0a\x0d]//g;
   $word = $_;
   `rm -f /tmp/b.txt /tmp/a.txt`;
   `wget https://en.wiktionary.org/wiki/$_ -o /tmp/a.txt -O /tmp/b.txt`;
   $is_english = 0;
   $has_part = 0;
   $has_bad = "";
   open FILE, "/tmp/b.txt";
   @cont = <FILE>;
   foreach $line (@cont) {
      if ($line =~ /<span class=\"toctext\">English<\/span>/ ||
          $line =~ /<span class=\"mw-headline\" id="English">English<\/span>/) {
         $is_english++;
      }
      foreach $part (@parts) {
         if ($line =~ /<span class=\"toctext\">$part<\/span>/ ||
             $line =~ /<span class=\"mw-headline\" id="$part">$part<\/span>/) {
            $has_part++;
         }
      }
      foreach $bad (@bad) {
         if ($line =~ /$bad/) {
            if (!($has_bad =~ /$bad/)) {
               $has_bad .= "/$bad";
            }
         }
      }
   }
   close FILE;

#   print "$is_english $has_part\n";
   $nnn++;
   if ($is_english && $has_part && !length($has_bad)) {
      print "$word\n";
#      print STDERR "$nnn accept $word\n";
   }
   else {
      $rrr++;
      $ratio = int(100 * $rrr / $nnn);
      print STDERR "$nnn reject $ratio $word $is_english $has_part $has_bad\n";
   }

   sleep .1;
}
