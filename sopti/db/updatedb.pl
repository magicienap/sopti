#! /usr/bin/perl

#BEGIN { $ENV{DBI_PUREPERL} = 2 }
use DBI;
use sigtrap;

@fields_csv = ('cycle', 'symbol', 'group', 'credits', 'places_room', 'period_code', 'room', 'theory_or_lab', 'lab_type', 'week', 'course_type', 'title', 'places_group', 'weekday', 'time');
@fields_courses = ('title');
@fields_courses_semester = ('course_type');
@fields_groups = ('places_room', 'places_group');
@fields_periods = ('room', 'weekday', 'time', 'week');

$CURRENT_SEMESTER='H2005';

sub connect_db() {
}

sub warning {
	print(("\033[1;33mwarning: \033[0m", @_, "\n"));
}

sub main() {
	print("DATABASE UPDATE\n");
	print("Connecting to database...\n");
	$dbh = DBI->connect('dbi:mysql:database=poly_courses;host=127.0.0.1', 'poly', 'pol') or die(DBI->errstr);
	
# 	print("Creating replacement tables...\n");
# 	$dbh->do('CREATE TABLE courses_new LIKE courses') or die $dbh->errstr;
# 	$dbh->do('CREATE TABLE courses_semester_new LIKE courses_semester') or die $dbh->errstr;

	# Get the unique for the current semester
	$semester_result = $dbh->selectall_arrayref('SELECT session.unique FROM session WHERE code="' . $CURRENT_SEMESTER . '"');
	if(scalar(@{$semester_result}) > 1) {
		die("More than 1 semester with code " . $CURRENT_SEMESTER . "\n");
	}
	elsif(scalar(@{$semester_result}) == 0) {
		die("No such semester in DB: " . $CURRENT_SEMESTER . "\n");
	}
	else {
		$current_semester_unique = $semester_result->[0][0];
	}
	
	# Download the courses table
	print("Downloading courses table...\n");
	$courses_table_ref = $dbh->selectall_hashref('SELECT * FROM courses', 'symbol');
	
	# Download the courses_semester table
	# Index the hash by course symbol (ex: ING1040)
	print("Downloading courses_semester table...\n");
	$courses_semester_table_ref = $dbh->selectall_hashref('SELECT courses.symbol as symbol, courses_semester.* FROM courses,courses_semester,session WHERE courses_semester.course=courses.unique AND session.code="' . $CURRENT_SEMESTER . '" and courses_semester.semester=session.unique', 'symbol');

	# Download the groups table
	# Index the hash by course_semester unique, then group name
	print("Downloading groups table...\n");
	$groups_table_array = $dbh->selectall_arrayref('SELECT groups.* FROM groups, courses_semester, session WHERE groups.course_semester=courses_semester.unique AND courses_semester.semester=session.unique AND session.unique="' . $current_semester_unique . '"', { Slice => {}});
	$groups_table_ref = {};
	foreach $row (@{$groups_table_array}) {
		my $course_semester = $row->{'course_semester'};
		my $name = $row->{'name'};
		my $theory_or_lab = $row->{'theory_or_lab'};
		
		$groups_table_ref->{$course_semester}->{$name}->{$theory_or_lab} = $row;
	}

	# Download the periods table
	# Index the hash by period code
	print("Downloading periods table...\n");
	$periods_table_array = $dbh->selectall_arrayref('SELECT periods.* FROM periods, groups, courses_semester, courses, session WHERE periods.group=groups.unique AND groups.course_semester=courses_semester.unique AND courses_semester.semester=session.unique AND courses_semester.course=courses.unique AND session.unique="' . $current_semester_unique . '"', { Slice => {}});
	$periods_table_ref = {};
	foreach $row (@{$periods_table_array}) {
		my $group = $row->{'group'};
		my $period_code = $row->{'period_code'};
		
		$periods_table_ref->{$group}->{$period_code} = $row;
	}
	
	print("Opening CSV...\n");
	open(DATAFILE, "<../data/courses.csv") or die("error opening data file");
	$i=0;
	my %course_sem_done, %courses_done, %current_line;
	$tmp=<DATAFILE>;
	print("Checking for differences with database...\n");
	while(<DATAFILE>) {
		chomp;
		$_ =~ s/[\n\r]*$//s;
		@fields = split(/;/);
		
		if(scalar(@fields) != scalar(@fields_csv)) {
			print(scalar(@fields), " != ", scalar(@fields_csv), "\n");
			die("bad field count");
		}
		
		# Transfer current line in a hash
		for($j=0; $j<scalar(@fields); $j++) {
			$current_line{$fields_csv[$j]} = $fields[$j];
		}
		
		# Modifiy the CSV data
		if($current_line{'week'} eq 'I') {
			$current_line{'week'}='B1';
		}
		elsif($current_line{'week'} eq 'P') {
			$current_line{'week'}='B2';
		}
		elsif($current_line{'week'} eq '') {
			$current_line{'week'}='A';
		}
		else {
			die('Invalid week expression in CSV');
		}

		# Update the courses table
		$current_course_entry = $courses_table_ref->{$current_line{'symbol'}};
		if($courses_done{$current_line{'symbol'}} != 1) {
			if($current_course_entry == undef) {
				# course not in DB, add it
				warning("[INSERT][courses] Course $current_line{'symbol'} not in table; adding it");
				$dbh->do("INSERT INTO courses (symbol, title) VALUES (\"$current_line{'symbol'}\", \"$current_line{'title'}\")") or die $dbh->errstr;
				# get the unique of the new entry
				my $unique = $dbh->selectall_arrayref("SELECT courses.unique FROM courses WHERE courses.symbol=\"$current_line{'symbol'}\"");
				if(scalar(@$unique) != 1) {
					die("cannot find unique of course just added");
				}
				$unique = $unique->[0]->[0];
				# insert new entry in local copy of DB
				$courses_table_ref->{$current_line{'symbol'}} = {'unique' => $unique };
				$current_course_entry = $courses_table_ref->{$current_line{'symbol'}};  # now we have a defined entry
			}
			else {
				for my $field (@fields_courses) {
					if($current_line{$field} ne $current_course_entry->{$field}) {
						warning("[UPDATE][courses] Course $current_line{'symbol'}, field $field needs update; doing it");
						$dbh->do("UPDATE courses SET $field=\"$current_line{$field}\" WHERE courses.unique=\'$current_course_entry->{'unique'}\'") or die $dbh->errstr;
					}
				}
			}
			
			$courses_done{$current_line{'symbol'}} = 1;
			$current_course_entry->{'used'} = 1; # Mark as used because it was in the CSV
		}
		
		# Update the courses_semester table
		$current_course_semester_entry = $courses_semester_table_ref->{$current_line{'symbol'}};
		if($courses_semester_done{$current_line{'symbol'}} != 1) {
			if($current_course_semester_entry == undef) {
				print $current_course_semester_entry,"\n";
				# course not in DB, add it
				warning("[INSERT][courses_semester] Course $current_line{'symbol'} was not in courses_semester table; adding it");
				$dbh->do("INSERT INTO courses_semester (course, semester, course_type) VALUES (\"$current_course_entry->{'unique'}\", \"$current_semester_unique\", \"$current_line{'course_type'}\")") or die $dbh->errstr;
				# get the unique of the new entry
				my $unique = $dbh->selectall_arrayref("SELECT courses_semester.unique FROM courses_semester WHERE course=\"$current_course_entry->{'unique'}\" AND semester=\"$current_semester_unique\" AND course_type=\"$current_line{'course_type'}\"");
				if(scalar(@$unique) != 1) {
					die("cannot find unique of course_semester just added");
				}
				$unique = $unique->[0]->[0];
				# insert new entry in local copy of DB
				$courses_semester_table_ref->{$current_line{'symbol'}} = {'unique' => $unique };
				$current_course_semester_entry = $courses_semester_table_ref->{$current_line{'symbol'}};  # now we have a defined entry
			}
			else {
				for my $field (@fields_courses_semester) {
					if($current_line{$field} ne $current_course_semester_entry->{$field}) {
						warning("[UPDATE][courses_semester] Course $current_line{'symbol'}, field $field needs update; doing it (csv: $current_line{$field}, DB: $course_semester_entry->{$field})");
						$dbh->do("UPDATE courses_semester SET $field=\"$current_line{$field}\" WHERE courses_semester.course=\'$course_semester_entry->{'unique'}\' AND courses_semester.semester=\'$current_semester_unique\'") or die $dbh->errstr;
					}
				}
			}
			
			$courses_semester_done{$current_line{'symbol'}} = 1;
			$current_course_semester_entry->{'used'} = 1; # Mark as used because it was in the CSV
		}
		
		# Update the groups table
		$current_course_semester_unique = $current_course_semester_entry->{'unique'};
		$current_group_entry = $groups_table_ref->{$current_course_semester_unique}->{$current_line{'group'}}->{$current_line{'theory_or_lab'}};
		if($groups_done{$current_line{'symbol'}}->{$current_line{'group'}}->{$current_line{'theory_or_lab'}} != 1) {
			if($current_group_entry == undef) {
				# course not in DB, add it
				warning("[INSERT][groups] Group $current_line{'symbol'}, $current_line{'group'}, $current_line{'theory_or_lab'} was not in groups table; adding it");
				$dbh->do("INSERT INTO groups (course_semester, name, theory_or_lab) VALUES (\"$current_course_semester_unique\", \"$current_line{'group'}\", \"$current_line{'theory_or_lab'}\")") or die $dbh->errstr;
				
				# get the unique of the new entry
				my $unique = $dbh->selectall_arrayref("SELECT groups.unique FROM groups WHERE course_semester=\"$current_course_semester_unique\" AND name=\"$current_line{'group'}\" AND theory_or_lab=\"$current_line{'theory_or_lab'}\"");
				if(scalar(@$unique) != 1) {
					die("cannot find unique of group just added");
				}
				$unique = $unique->[0]->[0];
				# insert new entry in local copy of DB
				$groups_table_ref->{$current_course_semester_unique}->{$current_line{'group'}}->{$current_line{'theory_or_lab'}} = {'unique' => $unique };
				$current_group_entry = $groups_table_ref->{$current_course_semester_unique}->{$current_line{'group'}}->{$current_line{'theory_or_lab'}};  # now we have a defined entry

			}
			else {
				for my $field (@fields_groups) {
					if($current_line{$field} ne $current_group_entry->{$field}) {
						warning("[UPDATE][groups] Group $current_line{'symbol'}, $current_line{'group'}, $current_line{'theory_or_lab'}, field $field needs update; doing it (csv: $current_line{$field}, DB: $courses_semester_table_ref{$current_line{'symbol'}}->{$field})");
						$dbh->do("UPDATE groups SET $field=\"$current_line{$field}\" WHERE groups.unique=\"$current_group_entry->{'unique'}\"") or die $dbh->errstr;
					}
				}
			}
			
			$groups_done{$current_line{'symbol'}}->{$current_line{'group'}}->{$current_line{'theory_or_lab'}} = 1;
			$current_group_entry->{'used'} = 1; # Mark as used because it was in the CSV
		}
		
		# Update the periods table
		$current_group_unique = $current_group_entry->{'unique'};
		$current_period_entry = $periods_table_ref->{$current_group_unique}->{$current_line{'period_code'}};
		#print("current period entry : $current_period_entry->{'time'}\n");
		if($current_period_entry == undef) {
			# course not in DB, add it
			warning("[INSERT][periods] Period $current_line{'symbol'}, $current_line{'group'}, $current_line{'theory_or_lab'}, $current_line{'period_code'} was not in periods table; adding it");
			$dbh->do("INSERT INTO periods (periods.group, periods.period_code, periods.room, periods.time, periods.weekday) VALUES (\"$current_group_unique\", \"$current_line{'period_code'}\", \"$current_line{'room'}\", \"$current_line{'time'}\", \"$current_line{'weekday'}\")") or die $dbh->errstr;
		}
		else {
			for my $field (@fields_periods) {
				if($current_line{$field} ne $current_period_entry->{$field}) {
					warning("[UPDATE][periods] Period (unique=$current_period_entry->{'unique'}) $current_line{'symbol'}, $current_line{'group'}, $current_line{'theory_or_lab'}, $current_line{'period_code'}, field $field needs update; doing it (csv: $current_line{$field}, DB: $current_period_entry->{$field})");
					$dbh->do("UPDATE periods SET $field=\"$current_line{$field}\" WHERE periods.unique=\"$current_period_entry->{'unique'}\"") or die $dbh->errstr;
				}
			}
			
			$current_period_entry->{'used'} = 1;
		}
		
# 		if($course_sem_done{$course_symbol} != '1') {
# 			$dbh->do("INSERT INTO courses_new (symbol, title) VALUES (\"$course_symbol\", \"$course_title\")") or die $dbh->errstr;
# 			$course_sem_done{$course_symbol} = '1';
# 		}
		$i++;
	}
	close DATAFILE;
	
	# Check for unused courses
	while ( my ($symbol, $entry) = each(%{$courses_table_ref}) ) {
		if($entry->{'used'} != 1) {
			warning("[INFO][courses] Course $symbol is historic");
			# Do not delete
		}
	}

	# Check for unused course_semesters
	while ( my ($symbol, $entry) = each(%{$courses_semester_table_ref}) ) {
		if($entry->{'used'} != 1) {
			warning("[DELETE][courses_semester] Course_semester $CURRENT_SEMESTER, $symbol is unused");
			$dbh->do("DELETE FROM courses_semester WHERE courses_semester.unique=\"$entry->{'unique'}\"") or die $dbh->errstr;
		}
	}
	
	# Check for unused groups
	while ( my ($symbol, $entry1) = each(%{$groups_table_ref}) ) {
		while ( my ($group_unique, $entry2) = each(%{$entry1}) ) {
			while ( my ($group_type, $entry3) = each(%{$entry2}) ) {
				if($entry3->{'used'} != 1) {
					warning("[DELETE][groups] Group $CURRENT_SEMESTER, $symbol, $group_name, $group_type (unique $entry3->{'unique'}) is unused");
					$dbh->do("DELETE FROM groups WHERE groups.unique=\"$entry3->{'unique'}\"") or die $dbh->errstr;
				}
			}
		}
	}
	
	# Check for unused periods
	while ( my ($group_unique, $entry1) = each(%{$periods_table_ref}) ) {
		while ( my ($period_code, $entry2) = each(%{$entry1}) ) {
			if($entry2->{'used'} != 1) {
				warning("[DELETE][periods] Period $entry2->{'unique'} is unused");
				$dbh->do("DELETE FROM periods WHERE periods.unique=\"$entry2->{'unique'}\"") or die $dbh->errstr;
			}
		}
	}
	
# 	print("Commiting changes...\n");
# 	$dbh->do(q{RENAME TABLE
# 		courses TO courses_old,
# 		courses_new TO courses,
# 		courses_semester TO courses_semester_old,
# 		courses_semester_new TO courses_semester
# 		}) or die $dbh->errstr;
# 	# Race condition if other update process starts here
# 	$dbh->do(q{DROP TABLE courses_old, courses_semester_old});
	
	print("Disconnecting...\n");
	$dbh->disconnect();
	print("done.\n");
}

&main();

# Sanity checks
# - Check that all course symbols are only once in the courses table
# SELECT count(*) AS num FROM courses GROUP BY courses.symbol DESC ORDER BY num LIMIT 1; == 1
# - Check that all courses_semesters are associated to a course
# select * from (select courses_semester.unique, count(*) as cnt from courses,courses_semester where courses_semester.course=courses.unique group by courses.unique) as tab where cnt != 1; == empty set
