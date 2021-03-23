#include <bits/stdc++.h>

using namespace std;

struct course;
struct group;
struct room;

struct course {
	string title, abbr, faculty;
	int hours;
	course(string title, string abbr, string faculty, int hours) {
		this -> title = title;
		this -> abbr = abbr;
		this -> faculty = faculty;
		this -> hours = hours;
	}
	course() {}
};
vector<course> courses;
map <string, course> courseByAbbr;

struct room {
	string number;
	int capacity = 0;
	room(string number, int cap) {
		this -> number = number;
		this -> capacity = cap;
	}
	room(){}
};

vector<room> rooms;

struct group {
	string code;
	vector <string> courses;
	int studentsNumber;
	pair <course, room> c[6][18];
	int used[6][20];
	group(string code, vector <string> c, int number) {
		this -> code = code;
		this -> courses = c;
		this -> studentsNumber = number;
		for (int i = 1; i <= 5; ++i)
			for (int j = 9; j <= 18; ++j)
				used[i][j] = 1;
	}
	group(){
		for (int i = 1; i <= 5; ++i)
			for (int j = 9; j <= 18; ++j)
				used[i][j] = 1;
	}
	void setCourse(int d, int h, course cou, room r) {
		c[d][h] = make_pair(cou, r);
		used[d][h] = 0;
	}
	bool isFree(int d, int h) {
		return used[d][h];
	}
	vector<course> v;
	vector<course> getCourses() {
		v.clear();
		for (int i = 0; i < courses.size(); ++i) {
			v.push_back(courseByAbbr[courses[i]]);
		}
		return v;
	}
};
vector<group> groups;

void readCourses() {
	ifstream fin("courses.txt");
	string title, abbr, faculty;
	while (fin >> title) {
		int hours;
		fin >> abbr >> faculty >> hours;
		course c = course(title, abbr, faculty, hours);
		courses.push_back(c);
		courseByAbbr[abbr] = c;
	}

	fin.close();
}

void readGroups() {
	ifstream fin("groups.txt");
	string code;
	string courseAbbr;
	int number, cnt;
	while (fin >> code >> number >> cnt) {
		vector <string> cs;
		while (cnt--) {
			fin >> courseAbbr;
			cs.push_back(courseAbbr);
		}
		groups.push_back(group(code, cs, number));
	}
	fin.close();
}

void readRooms() {
	ifstream fin("rooms.txt");

	string number;
	while (fin >> number) {
		int cap;
		fin >> cap;
		rooms.push_back(room(number, cap));
	}
	fin.close();
}

bool cmp(room a, room b) {
	return a.capacity > b.capacity;
}

bool coursesCmp(course a, course b) {
	return (a.abbr[a.abbr.size() - 1] == 'L' && !b.abbr[b.abbr.size()-1] != 'L');
}

map <string, string> courseAt[6][20];
map <string, int> ellapsed[6][20];
map <string, set <pair <int, int> > > available;
map <string, pair<room, pair<int, int> > > lectureRoom;

bool putLectureOnPair(int c, int r, int i, int j1, int j2) {
	if (j1 == 13 || j2 == 13 || j1 < 9 || j2 > 17) return 0;
	if (!available[rooms[r].number].count(make_pair(i, j1)) &&
		!available[rooms[r].number].count(make_pair(i, j2))
		) {
		available[rooms[r].number].insert(make_pair(i, j1));
		available[rooms[r].number].insert(make_pair(i, j2));
		lectureRoom[courses[c].abbr] = make_pair(rooms[r], make_pair(i, j1));
		return true;
	}
	return false;
}

bool putLessonOnPair(vector <course> courses, int i, int j, int r, int d, int h1, int h2) {
	if (h1 == 13 || h2 == 13 || h1 < 9 || h2 > 17) return 0;
	if (!available[rooms[r].number].count(make_pair(d, h1)) &&
		!available[rooms[r].number].count(make_pair(d, h2))
		) {
		available[rooms[r].number].insert(make_pair(d, h1));
		available[rooms[r].number].insert(make_pair(d, h2));
		groups[i].setCourse(d, h1, courses[j], rooms[r]);
		groups[i].setCourse(d, h2, courses[j], rooms[r]);
		courseAt[d][h1][rooms[r].number] = courses[j].abbr;
		courseAt[d][h2][rooms[r].number] = courses[j].abbr;
		return true;
	}
	return false;
}


int main() {
	readCourses();
	readGroups();
	readRooms();
	sort(rooms.begin(), rooms.end(), cmp);
	srand(time(0));

	for (int c = 0; c < courses.size(); ++c) {
		string abbr = courses[c].abbr;
		if (abbr[abbr.size()-1] != 'L') continue;
		// cout << abbr << endl;
		bool found = false;
		int d, h;
		room ro;
		vector <int> days;
		days.push_back(1);
		days.push_back(2);
		days.push_back(3);
		days.push_back(4);
		days.push_back(5);

		random_shuffle(days.begin(), days.end());
		vector <int> hours;
		hours.push_back(9);
		hours.push_back(11);
		hours.push_back(14);
		hours.push_back(16);
		hours.push_back(17);

		random_shuffle(hours.begin(), hours.end());
		for (int di = 0; di < 5; ++di) {
			int i = days[di];
			if (found) break;
			for (int hj = 0; hj < hours.size(); ++hj) {
				int j = hours[hj];
				if (found) break;
				for (int r = 0; r < rooms.size(); ++r) {
					if (rooms[r].capacity < 100) continue;
					if (!putLectureOnPair(c, r, i, j, j + 1))
						if (!putLectureOnPair(c, r, i, j - 1, j))
							continue;
					found = true;
					break;
				}
			}
		}
	}

	random_shuffle(rooms.begin(), rooms.end());
	random_shuffle(groups.begin(), groups.end());
	random_shuffle(courses.begin(), courses.end());

	for (int i = 0; i < groups.size(); ++i) {
		vector <course> courses = groups[i].getCourses();
		sort(courses.begin(), courses.end(), coursesCmp);
		for (int j = 0; j < courses.size(); ++j) {
			bool found = false;
			string abbr = courses[j].abbr;
			bool isLecture = abbr[abbr.size()-1] == 'L';
			if (isLecture) {
				groups[i].setCourse(lectureRoom[abbr].second.first,
					lectureRoom[abbr].second.second,
					courses[j],
					lectureRoom[abbr].first);
				groups[i].setCourse(lectureRoom[abbr].second.first,
					lectureRoom[abbr].second.second + 1,
					courses[j],
					lectureRoom[abbr].first);
				continue;
			}

			vector <int> days;
			days.push_back(1);
		    days.push_back(2);
		    days.push_back(3);
		    days.push_back(4);
		    days.push_back(5);

			random_shuffle(days.begin(), days.end());
			vector <int> hours;
            hours.push_back(9);
		    hours.push_back(11);
		    hours.push_back(14);
		    hours.push_back(16);
			random_shuffle(hours.begin(), hours.end());
			for (int di = 0; di < 5; ++di) {
				int d = days[di];
				if (found) break;
				for (int hi = 0; hi < hours.size(); ++hi) {
					int h = hours[hi];
					if (!groups[i].isFree(d, h)) {
						continue;
					}
					if (found)
						break;
					for (int r = 0; r < rooms.size(); ++r) {
						if (rooms[r].capacity > 50) continue;
						if (!putLessonOnPair(courses, i, j, r, d, h, h + 1))
							if (!putLessonOnPair(courses, i, j, r, d, h - 1, h)) {
								continue;
							}
						found = true;
						break;

					}
				}
			}

		}
	}

	string weekDay[] = {"", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};

	freopen("READY", "w", stdout);

	for (int i = 0; i < groups.size(); ++i) {
		cout << groups[i].code << ": \n\n";
		for (int d = 1; d <= 5; ++d) {
			for (int h = 9; h <= 18; ++h) {
				if (!groups[i].isFree(d, h)) {
					cout << weekDay[d] << " "
					<< h <<":00" << " "
					<< groups[i].c[d][h].first.abbr << " "
					<< groups[i].c[d][h].second.number << " "
					<< groups[i].c[d][h].first.faculty << endl<<endl;
				}

			}
		}
	}
    //fclose(stdout);
	return 0;
}
