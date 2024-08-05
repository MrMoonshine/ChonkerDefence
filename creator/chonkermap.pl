#!/usr/bin/perl
use warnings;
use strict;
use Data::Dumper;
# sudo apt install libgd-perl
use GD;
use Getopt::Long;

sub help{
print <<HELP;
Chonkermap is a CLI Tool, which converts PNG-Images in a Chonker-Defence level.

Usage: chonkermap.pl [OPTIONS]

Options:
 -l, --layout           Layout .png file
 -t, --terrain          Terrain .png file
 -o, --output           Output .chonkmap Level file
 -h, --help             Show this
HELP
}

my $needhelp = 0;
my $layoutfile = '';
my $terrainfile = '';
my $outputfile = '';

GetOptions( 
    'layout=s' => \$layoutfile,
    'terrain=s' => \$terrainfile,
    'output=s' => \$outputfile,
    'help' => \$needhelp
);

if(length($layoutfile) < 1){
    print STDERR "\e[0;31mNo Layout file specified! (option --layout)\e[0m\n";
    $needhelp += 1;
}

if(length($outputfile) < 1){
    print STDERR "\e[0;31mNo Output file specified! (option --output)\e[0m\n";
    $needhelp += 1;
}

if($needhelp > 0){
    help();
    exit 0;
}

my $ image = new GD::Image($layoutfile) or die("Couldn't open " . $layoutfile);
if($image->width > 255 || $image->height > 255){
    print STDERR "Image is too big! (max. 255x255)\n";
    exit -1;
}
my $terrainimgok = length($terrainfile);
my $terrainimage;#new GD::Image($terrainfile) or die();
if($terrainimgok > 0){
    eval {$terrainimage = new GD::Image($terrainfile)};
    if ($@) {
        print STDERR "Unable to open Terrain file ".$terrainfile.":\t[$@]\n";
        $terrainimgok = 0;
    }
}

if($terrainimgok > 0){
    if($terrainimage->width != $image->width || $terrainimage->height != $image->height){
        print STDERR "Terrain image is not same size as layout image\n";
        $terrainimgok = 0;
    }
}

#print("Image dimensions are (".$image->width."|".$image->height.")\n");
$image->flipHorizontal();
if($terrainimgok > 0){
    $terrainimage->flipHorizontal();
}

my %meta = (
    name => "Nameless",
    style => "default",
    prio => 255,
    b => 32,
    h => 18,
    cats => 0
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
        print "The Name \"";
        print $meta{'name'};
        print "\" is too long!\n"; 
        $promt = 1;
    }
}
$promt = 1;
while($promt){
    print "Style Levels [default]: ";
    # Read Line
    $meta{'style'} = <STDIN>;
    # Remove the \n at the end
    $meta{'style'} = substr $meta{'style'}, 0, length($meta{'style'}) - 1;

    $promt = 0;
    if(length($meta{'style'}) < 1){
        $meta{'style'} = "default";
    }elsif(length($meta{'style'}) > 32){
        print "Der Name des styles \"";
        print $meta{'style'};
        print "\" ist zu lange!\n"; 
        $promt = 1;
    }
}

unless(open FILE, '>'.$outputfile) {
    # Die with error message 
    # if we can't open it.
    die "\n$outputfile couldn't be created!\n";
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
    if($_ < length($meta{'style'})){
        print FILE substr($meta{'style'}, $_, 1);
    }else{
        print FILE pack("c", 0);
    }
}

print FILE pack("C", $image->width);
print FILE pack("C", $image->height);

# Reserved Bytes
for (0..16-1){
    print FILE pack("c", 64);
}

my @tmparr = ();

for (my $y=0; $y<$image->height; $y++) {
    for (my $x=0; $x<$image->width; $x++) {
        my $index = $image->getPixel($x, $y);
        my ($r,$g,$b) = $image->rgb($index);
        my $alpha = $image->alpha($index);
        #if($alpha != 0){
        #    print("xy(".$x."|".$y.") = rgba(".$r."|".$g."|".$b."|".$alpha.")\n");
        #}

        my $layoutnibble = 0;
        # Shape the Data correctly
        if($b > 127){
            $layoutnibble |= 1 << 0;
        }
        if($g > 127){
            $layoutnibble |= 1 << 1;
        }
        if($r > 127){
            $layoutnibble |= 1 << 2;
        }
        if($alpha <= 64){
            $layoutnibble |= 1 << 3;
        }

        my $terrainnibble = 0;
        if($terrainimgok >0){
            my $index = $terrainimage->getPixel($x, $y);
            my ($r,$g,$b) = $terrainimage->rgb($index);
            my $alpha = $terrainimage->alpha($index);

            if($b > 127){
                $terrainnibble |= 1 << 0;
            }
            if($g > 127){
                $terrainnibble |= 1 << 1;
            }
            if($r > 127){
                $terrainnibble |= 1 << 2;
            }
            if($alpha <= 64){
                $terrainnibble |= 1 << 3;
            }
        }

        #$tmparr[(scalar @tmparr) - 1] |= $layoutnibble;
        #$tmparr[(scalar @tmparr) - 1] |= $terrainnibble << 4;
        my $databyte = 0;
        $databyte |= $layoutnibble;
        $databyte |= $terrainnibble << 4;
        push(@tmparr, $databyte);
    }
}

foreach (@tmparr){
    print FILE pack("C", $_);
}

close FILE;
print "Das Level ist Fertig!\n";
