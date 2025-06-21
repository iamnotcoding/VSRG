#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "map_parser.hpp"

using namespace std;
using namespace map_parser;

// Channel range constants
constexpr int P1_REGULAR_MIN = 11, P1_REGULAR_MAX = 19;
constexpr int P2_REGULAR_MIN = 21, P2_REGULAR_MAX = 29;
constexpr int P1_HIDDEN_MIN = 31, P1_HIDDEN_MAX = 39;
constexpr int P2_HIDDEN_MIN = 41, P2_HIDDEN_MAX = 49;
constexpr int P1_LONG_MIN = 51, P1_LONG_MAX = 59;
constexpr int P2_LONG_MIN = 61, P2_LONG_MAX = 69;

static pair<string::size_type, string::size_type> find_field_range(const vector<string> &lines, string field_name);
static void parse_header(const vector<string> &lines, Header &header_out);
static pair<string, string> parse_header_line(const string &line);

struct Main_data_line
{
	int measure;
	int channel;
	vector<string> objects;
};

static Main_data_line parse_main_data_line(const string &line);
static void parse_main_data(const vector<string> &lines, double initial_bpm, Map &map_out);
static vector<Main_data_line> concate_main_data(const vector<Main_data_line> &data);

static Channel_type get_channel_type(int channel);
static Note_type get_note_type(int channel);

static int get_lane_index(int channel);

constexpr const char *DEFINITION_SETTER = "*----------------------";

namespace map_parser
{
	void parse_map(ifstream file, Map &map_out)
	{
		if (!file.is_open())
		{
			throw runtime_error("Failed to open file");
		}

		vector<string> lines;
		string line;

		while (getline(file, line))
		{
			lines.push_back(line);
		}

		file.close();

		parse_map(lines, map_out);
	}

	void parse_map(const vector<string> &lines, Map &map_out)
	{
		if (lines.empty())
		{
			throw runtime_error("Input lines are empty");
		}

		parse_header(lines, map_out.header);

		double initial_bpm;

		try
		{
			initial_bpm = stod(map_out.header.metadata.at("BPM"));
		}
		catch (const std::exception &e)
		{
			throw runtime_error("BPM not found in header or invalid format: " + string(e.what()));
		}

		parse_main_data(lines, initial_bpm, map_out);
	}

	void test()
	{
		// Example usage of parse_header_line
		string header_line = "#PLAYER 1";
		auto [key, value] = parse_header_line(header_line);
		cout << "Key: " << key << ", Value: " << value << endl;

		// Example usage of find_field_start
		vector<string> lines = {"#PLAYER 1", "#GENRE Postmodern Psytrance", "#TITLE Corgito Ergosum",
								"*---------------------- HEADER FIELD", "#ENEMY 2"};

		pair<string::size_type, string::size_type> range = find_field_range(lines, "HEADER");
		cout << "Field 'HEADER' starts at line: " << range.first << ", ends at line: " << range.second << endl;

		fstream f("../../ex.bms", ios::in);

		if (!f.is_open())
		{
			throw runtime_error("Failed to open file: ../../ex.bms");
		}

		lines.clear();
		string line;

		while (getline(f, line))
		{
			lines.push_back(line);
		}

		// Example usage of parse_header
		Header header;

		parse_header(lines, header);

		cout << "Parsed Header:" << endl;

		for (const auto &pair : header.metadata)
		{
			cout << pair.first << ": " << pair.second << endl;
		}

		cout << "Parsed Audios:" << endl;

		long i = 0;

		for (const auto &pair : header.audios)
		{
			if (i >= 10)
			{
				break; // Limit output to first 10 audio entries
			}

			cout << "Audio " << pair.first << ": " << pair.second << endl;

			i++;
		}

		Main_data_line main_data_line = parse_main_data_line("#00713:3H003200003200000000000046000000");
		cout << "Parsed Main Data Line:" << endl;
		cout << "Measure: " << main_data_line.measure << ", Channel: " << main_data_line.channel << ", Objects: ";

		for (const auto &object : main_data_line.objects)
		{
			cout << object << " ";
		}

		cout << endl;

		// parse main data
		Map map;
		parse_map(ifstream("../../ex.bms"), map);

		cout << "Parsed Map:" << endl;

		for (const auto &note : map.notes)
		{
			cout << "Note: Lane " << note.lane_index << ", Start Time: " << note.start_time / 1000.0 << "s"
				 << (note.start_time != note.end_time ? ", End Time: " + to_string(note.end_time / 1000.0) + "s" : "")
				 << ", Hidden: " << (note.is_hidden ? "Yes" : "No")
				 << (note.start_time == note.end_time ? " (Regular)" : " (Long)") << endl;
		}
	}
} // namespace map_parser

// Lines starts from 0
// pair<start, end>
static pair<string::size_type, string::size_type> find_field_range(const vector<string> &lines, string field_name)
{
	auto it = find_if(lines.begin(), lines.end(), [field_name](const string &line) {
		return line.find(string(DEFINITION_SETTER) + " " + field_name) != string::npos;
	});

	if (it == lines.end())
	{
		throw runtime_error(field_name + " start not found");
	}

	string::size_type start = distance(lines.begin(), it) + 1; // +1 to skip the DEFINITION_SETTER line

	// Find the end of the field
	auto end_it = find_if(lines.begin() + start, lines.end(),
						  [](const string &line) { return line.find(DEFINITION_SETTER) != string::npos; });

	string::size_type end = (end_it == lines.end()) ? lines.size() : distance(lines.begin(), end_it);

	return {start, end};
}

static void parse_header(const vector<string> &lines, Header &header_out)
{
	pair<string::size_type, string::size_type> range = find_field_range(lines, "HEADER");

	for (long i = range.first; i < (long)range.second; ++i)
	{
		const auto &line = lines[i];

		if (line.empty() || line[0] != '#')
		{
			continue; // Skip empty lines and non-header lines
		}

		string key = parse_header_line(line).first;
		string value = parse_header_line(line).second;

		if (key.length() <= 3)
		{
			header_out.metadata[key] = value;
			continue;
		}

		string meaning = key.substr(0, 3);
		string object = key.substr(3);

		if (object.length() != 2)
		{
			header_out.metadata[key] = value;
			continue;
		}

		if (key.substr(0, 3) == "WAV")
		{
			header_out.audios[object] = value;
		}

		// TODO : add bitmaps and BPMs
	}

	range = find_field_range(lines, "EXPANSION");

	for (long i = range.first; i < (long)range.second; ++i)
	{
		const auto &line = lines[i];

		if (line.empty() || line[0] != '#')
		{
			continue; // Skip empty lines and non-header lines
		}

		try
		{
			header_out.metadata[parse_header_line(line).first] = parse_header_line(line).second;
		}
		catch (const runtime_error &e)
		{
			header_out.metadata[line.substr(1)] = "";
		}
	}
}

static pair<string, string> parse_header_line(const string &line)
{
	string::size_type pos = line.find(' ');

	if (pos == string::npos)
	{
		throw runtime_error("Invalid header line format: " + line);
	}

	string key = line.substr(1, pos - 1);
	string value = line.substr(pos + 1);

	return {key, value};
}

static Main_data_line parse_main_data_line(const string &line)
{
	Main_data_line data;
	string objects_str;
	int measure;
	int channel;

	measure = stoi(line.substr(1, 3));
	channel = stoi(line.substr(4, 2));
	objects_str = line.substr(7);

	data.measure = measure;
	data.channel = channel;

	for (long i = 0; i < (long)objects_str.size(); i += 2)
	{
		data.objects.push_back(objects_str.substr(i, 2));
	}

	return data;
}

static vector<Main_data_line> concate_main_data(const vector<Main_data_line> &data)
{
	vector<Main_data_line> concatenated_data;

	if (data.empty())
	{
		return vector<Main_data_line>();
	}

	Main_data_line current_data = data[0];

	for (long i = 1; i < (long)data.size(); ++i)
	{
		if (data[i].measure == current_data.measure && data[i].channel == current_data.channel)
		{
			// Add objects to the current data
			current_data.objects.insert(current_data.objects.end(), data[i].objects.begin(), data[i].objects.end());
			continue;
		}

		concatenated_data.push_back(current_data);
		current_data = data[i];
	}

	// Add the last accumulated data
	concatenated_data.push_back(current_data);

	return concatenated_data;
}

// Fills out the main data part of the map
static void parse_main_data(const vector<string> &lines, double initial_bpm, Map &map_out)
{
	vector<Main_data_line> main_data_lines;
	auto range = find_field_range(lines, "MAIN DATA");

	for (long i = range.first; i < (long)range.second; ++i)
	{
		const auto &line = lines[i];

		if (line.empty() || line[0] != '#')
		{
			continue; // Skip empty lines and comments
		}

		try
		{
			main_data_lines.push_back(parse_main_data_line(line));
		}
		catch (const invalid_argument &e)
		{
			throw runtime_error("Invalid main data line format: " + line);
			continue; // Skip invalid lines
		}
	}

	main_data_lines = concate_main_data(main_data_lines);

	double measure_interval = (60.0 / initial_bpm) * 4 * 1000; // 4 beats per measure

	// Add this to track long notes for each lane
	std::map<int, double> long_note_starts; // lane_index -> start_time

	for (auto &main_data_line : main_data_lines)
	{
		// Calculate time based on measure number instead of accumulating
		double measure_start_time = main_data_line.measure * measure_interval;

		long object_count = main_data_line.objects.size();
		double beat_interval = measure_interval / object_count;

		for (long i = 0; i < object_count; i++)
		{
			const string &object = main_data_line.objects[i];

			if (object == "00")
			{
				continue; // Skip empty objects
			}

			Channel_type channel_type = get_channel_type(main_data_line.channel);

			if (channel_type != NOTE)
			{
				continue; // TODO : Will be implemented later
			}

			Note_type note_type = get_note_type(main_data_line.channel);
			int lane_index = get_lane_index(main_data_line.channel);

			// Calculate time from measure start + position within measure
			double object_time = measure_start_time + i * beat_interval;

			if (note_type.is_regular_note)
			{
				map_out.notes.push_back(Note(lane_index, object_time, object_time, note_type.is_hidden));
				continue;
			}

			// Regular notes are already handled above with a continue, so we only have long notes here
			auto it = long_note_starts.find(lane_index);

			if (it == long_note_starts.end())
			{
				// First marker case
				long_note_starts[lane_index] = object_time;
				continue;
			}

			// Second marker case
			map_out.notes.push_back(Note(lane_index, it->second, object_time, note_type.is_hidden));
			long_note_starts.erase(it);
		}
	}
}

static Channel_type get_channel_type(int channel)
{
	if (channel == 1)
	{
		return BGM;
	}
	else if (channel == 2)
	{
		return MINUTE_OMIT;
	}
	else if (channel == 3)
	{
		return BPM_CHANGE;
	}
	else if (channel == 4)
	{
		return BGA;
	}
	else if (channel == 8)
	{
		return EXTENDED_BPM_CHANGE;
	}
	else
	{
		try
		{
			get_note_type(channel);
		}
		catch (const runtime_error &e)
		{
			return UNDEFINED;
		}

		return NOTE;
	}
}

static Note_type get_note_type(int channel)
{
	Note_type note_type;

	if (channel >= P1_REGULAR_MIN && channel <= P1_REGULAR_MAX)
	{
		note_type.player = 1;
		note_type.is_regular_note = true; // Regular note
		note_type.is_hidden = false;	  // Visible note
	}
	else if (channel >= P2_REGULAR_MIN && channel <= P2_REGULAR_MAX)
	{
		note_type.player = 2;
		note_type.is_regular_note = true; // Regular note
		note_type.is_hidden = false;	  // Visible note
	}
	else if (channel >= P1_HIDDEN_MIN && channel <= P1_HIDDEN_MAX)
	{
		note_type.player = 1;
		note_type.is_regular_note = false; // Not sure about this
		note_type.is_hidden = true;		   // Hidden note
	}
	else if (channel >= P2_HIDDEN_MIN && channel <= P2_HIDDEN_MAX)
	{
		note_type.player = 2;
		note_type.is_regular_note = false; // Not sure about this
		note_type.is_hidden = true;		   // Hidden note
	}
	else if (channel >= P1_LONG_MIN && channel <= P1_LONG_MAX)
	{
		note_type.player = 1;
		note_type.is_regular_note = false; // FIXED: Long notes correctly marked
		note_type.is_hidden = false;
	}
	else if (channel >= P2_LONG_MIN && channel <= P2_LONG_MAX)
	{
		note_type.player = 2;
		note_type.is_regular_note = false; // FIXED: Long notes correctly marked
		note_type.is_hidden = false;
	}
	else
	{
		throw runtime_error("Unknown note type for channel: " + to_string(channel));
	}

	return note_type;
}

// TODO : Add more key modes support other than 7K
static int get_lane_index(int channel)
{
	cout << "get_lane_index called with channel: " << channel << endl;
	if (channel >= P1_REGULAR_MIN && channel <= P1_REGULAR_MAX)
	{
		channel -= 10;
	}
	else if (channel >= P2_REGULAR_MIN && channel <= P2_REGULAR_MAX)
	{
		channel -= 20;
	}
	else if (channel >= P1_HIDDEN_MIN && channel <= P1_HIDDEN_MAX)
	{
		channel -= 30;
	}
	else if (channel >= P2_HIDDEN_MIN && channel <= P2_HIDDEN_MAX)
	{
		channel -= 40;
	}
	else if (channel >= P1_LONG_MIN && channel <= P1_LONG_MAX)
	{
		channel -= 50;
	}
	else if (channel >= P2_LONG_MIN && channel <= P2_LONG_MAX)
	{
		channel -= 60;
	}

	if (channel == 6)
	{
		return 0; // Scratch
	}
	else if (channel >= 1 && channel <= 5)
	{
		return channel;
	}
	else if (channel >= 8 && channel <= 9)
	{
		return channel - 2; // Adjust range
	}

	throw runtime_error("Invalid channel for lane index: " + to_string(channel));
}