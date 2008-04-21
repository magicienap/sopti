use strict;
use Sys::Hostname;

our(%CONFIG, $SOPTI_DIR, %upstream_to_sopti_sem);
%CONFIG=();
$SOPTI_DIR='.';

%upstream_to_sopti_sem = ( 1 => 'H', 2 => 'E', 3 => 'A' );

sub read_config {
	my $current_semester;

	#print "Opening config file...\n";
	open(CONFIGFILE, "<" . $SOPTI_DIR . "/sopti.conf") or die("error opening config file");
	
	while(<CONFIGFILE>) {
		my $line;
		chomp;
		$line = $_;
		
		# remove comments
		$line =~ s/([^#]*)#.*/\1/;
		
		if($line =~ /^[ \t]*$/) {
			next;
		}
		
		my $varname;
		my $varval;
		
		if($line =~ /^[ \t]*[^ \t]+[ \t]+[^ \t"]+[ \t]*$/) {
			$varname = $line;
			$varval = $line;
			$varname =~ s/^[ \t]*([^ \t]+)[ \t]+[^ \t"]+[ \t]*$/\1/;
			$varval =~ s/^[ \t]*[^ \t]+[ \t]+([^ \t"]+)[ \t]*$/\1/;
			#print "got [$varname][$varval]\n";
		}
		elsif($line =~ /^[ \t]*[^ \t]+[ \t]+"[^"]*"[ \t]*$/) {
			$varname = $line;
			$varval = $line;
			$varname =~ s/^[ \t]*([^ \t]+)[ \t]+"[^"]*"[ \t]*$/\1/;
			$varval =~ s/^[ \t]*[^ \t]+[ \t]+"([^"]*)"[ \t]*$/\1/;
		}
		else {
			die("invalid config file line: $line\n");
		}
		
		$CONFIG{$varname} = $varval;
	}
	
	#print "Closing config file...\n";
	close CONFIGFILE;
}

sub get_upstream_semester {
	my $tmp;

	#print("Opening closed.csv...\n");
	open(CLOSEDFILE, "<".$SOPTI_DIR."/data/closed.csv") or die("error opening data file");
	$tmp=<CLOSEDFILE>;

	my $current_semester;
	while(<CLOSEDFILE>) {
		chomp;

                if(length($_) < 5) {
                        print("WARNING: skipping line [" . printable($_) . "] which seems to contain garbage\n");
			next;
                }

		$_ =~ s/[\n\r]*$//s;
		my @fields = split(/;/);
		
		if(scalar(@fields) != 6) {
			die("bad field count");
		}
		
		#print "return semester\n";
		$current_semester = $fields[0];
		last;
	}
	
	#print("Closing Closed CSV...\n");
	close CLOSEDFILE;
	
	return $current_semester;
}

sub send_mail {
	my ($to, $from, $subject, $message) = @_;
	my $sendmail = '/usr/sbin/sendmail';
	open(MAIL, "|$sendmail -oi -t");
	print MAIL "From: $from\n";
	print MAIL "To: $to\n";
	print MAIL "Subject: $subject\n\n";
	print MAIL "$message\n";
	close(MAIL);
}

sub main() {
	my $upstream_code;
	my $sopti_code;
	my $upstream_year;
	my $upstream_sem;
	my $upstream_conv_code;
	&read_config;

	$upstream_code = &get_upstream_semester;
	$sopti_code = $CONFIG{'default_semester'};
	print "upstream semester: ".$upstream_code."\n";
	print "sopti semester: ".$sopti_code."\n";
	
	$upstream_year = $upstream_code;
	$upstream_sem = $upstream_code;
	$upstream_year =~ s/^(....).*/\1/;
	$upstream_sem =~ s/^....(.).*/\1/;

	$upstream_conv_code = $upstream_to_sopti_sem{$upstream_sem}.$upstream_year;

	print "upstream conv code: ".$upstream_conv_code."\n";

	if ($upstream_conv_code ne $sopti_code) {
		send_mail($CONFIG{'private_admin_email'}, $CONFIG{'admin_email'}, "Upstream semester change on sopti", "Hello,\n\nThis is the sopti installation at ".&hostname.". The upstream semester has changed. Sopti is configured to use ".$sopti_code.", but upstream now announces ".$upstream_conv_code.".\n\nThanks.\n\nSopti");
	}
}

&main();
