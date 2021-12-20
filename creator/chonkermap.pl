#!/usr/bin/perl
use warnings;
use strict;
use Data::Dumper;
# sudo apt install libgd-perl
use GD;

sub help{
print <<HELP;
Chonkermap ist ein CLI Werkzeug, was ein PNG-Bild in ein Chonker-Defense Level umbaut.

Nutzung: chonkermap <bild> <level>
HELP
}

if(scalar(@ARGV) != 2){
    help();
    exit -1;
}

foreach $a (@ARGV){
    if($a eq "-h" || $a eq "-?" || $a eq "--help"){
        help();
        exit 0;
    }
}

my $ image = new GD::Image($ARGV[0]) or die("Couldn't open " . $ARGV[0]);
if($image->width > 255 || $image->height > 255){
    print STDERR "Bild ist zu Groß! (max. 255x255)\n";
    exit -1;
}

my %meta = (
    name => "Namenlos",
    stil => "default",
    prio => 255,
    b => 32,
    h => 18,
    katzen => 0
);

my $promt = 1;
while($promt){
    print "Name des Levels: ";
    # Read Line
    $meta{'name'} = <STDIN>;
    # Remove the \n at the end
    $meta{'name'} = substr $meta{'name'}, 0, length($meta{'name'}) - 1;

    $promt = 0;
    if(length($meta{'name'}) < 1){
        $promt = 1;
    }elsif(length($meta{'name'}) > 64){
        print "Der Name \"";
        print $meta{'name'};
        print "\" ist zu lange!\n"; 
        $promt = 1;
    }
}
$promt = 1;
while($promt){
    print "Stil des Levels [default]: ";
    # Read Line
    $meta{'stil'} = <STDIN>;
    # Remove the \n at the end
    $meta{'stil'} = substr $meta{'stil'}, 0, length($meta{'stil'}) - 1;

    $promt = 0;
    if(length($meta{'stil'}) < 1){
        $meta{'stil'} = "default";
    }elsif(length($meta{'stil'}) > 32){
        print "Der Name des Stils \"";
        print $meta{'stil'};
        print "\" ist zu lange!\n"; 
        $promt = 1;
    }
}

my $file = $ARGV[1];
unless(open FILE, '>'.$file) {
    # Die with error message 
    # if we can't open it.
    die "\n$file kann nicht erstellt werden!\n";
}

binmode FILE or die "binmode failed: $!";;


for (0..64-1){
    if($_ < length($meta{'name'})){
        print FILE substr($meta{'name'}, $_, 1);
    }else{
        #print $_."\n";
        print FILE pack("c", 0);
    }
}

for (0..32-1){
    if($_ < length($meta{'stil'})){
        print FILE substr($meta{'stil'}, $_, 1);
    }else{
        print FILE pack("c", 0);
    }
}

print FILE pack("C", $image->width);
print FILE pack("C", $image->height);

# Reservierte Bytes
for (0..16-1){
    print FILE pack("c", 64);
}

my @tmparr;
my $counter = 0;

for (my $y=0; $y<$image->height; $y++) {
    for (my $x=0; $x<$image->width; $x++) {
        my $index = $image->getPixel($x, $y);
        my ($r,$g,$b) = $image->rgb($index);

        my $resultnibble = 0;
        # Shape the Data correctly
        if($b > 127){
            $resultnibble |= 1 << 0;
        }
        if($g > 127){
            $resultnibble |= 1 << 1;
        }
        if($r > 127){
            $resultnibble |= 1 << 2;
        }

        if($counter % 2){
            $tmparr[(scalar @tmparr) - 1] |= $resultnibble;
            #print "Reslutbyte: ".$tmparr[(scalar @tmparr) - 1]."\n";
        }else{
            push(@tmparr, $resultnibble << 4);
        }
        $counter++;
    }
}

foreach (@tmparr){
    print FILE pack("C", $_);
}

close FILE;
print "Das Level ist Fertig!\n";
