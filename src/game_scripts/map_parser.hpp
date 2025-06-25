#pragma once

#include <fstream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace map_parser
{
	/*
	main data format

	#aaabb:cccccccc

	aaa: track number (from 000 to 999)
	bb : channel number where you want to send message.(from 00 to FF)
	cccccccc : <message>
	*/

	// object : a std::string which consists of two characters, like "00", "0A", "1B", etc.
	typedef enum CHANNEL_TYPE
	{
		BGM,
		MINUTE_OMIT,
		BPM_CHANGE,
		EXTENDED_BPM_CHANGE,
		BGA,
		NOTE,
		UNDEFINED
	} Channel_type;

	struct Note_type
	{
		int player;			  // 1 or 2
		bool is_regular_note; // true if regular note, false if long note
		bool is_hidden;		  // true if hidden note, false if visible
	};

	struct Header
	{
		std::unordered_map<std::string, std::string> metadata;
		std::unordered_map<std::string, std::string> audios; // <object, path>
		// Frames of a BGA
		std::unordered_map<std::string, std::string> bitmaps; // <object, path>

        Header &operator=(const Header &&other)
        {
            metadata = std::move(other.metadata);
            audios = std::move(other.audios);
            bitmaps = std::move(other.bitmaps);
            return *this;
        }

        Header &operator=(const Header &other)
        {
            metadata = other.metadata;
            audios = other.audios;
            bitmaps = other.bitmaps;
            return *this;
        }
	};

	struct Note
	{
		bool is_hidden;	   // false if visible, true if hidden
		int lane_index;	   // 0: scratch, 1~7 : regular lanes
		double start_time; // in milliseconds
		double end_time;   // in milliseconds, for long notes. For regular notes, it's the same as start_time

		Note(int lane_index, double start_time, double end_time, bool is_hidden = false)
			: is_hidden(is_hidden), lane_index(lane_index), start_time(start_time), end_time(end_time)
		{
		}
	};

	// TODO : add other key modes support other than 7K
	struct Map
	{
        Header header;
		// main data section : <milisecond, ...>

		std::map<double, std::string> bgms;

		std::vector<Note> notes;

		// TODO: add more channel numbers
	};

	void parse_map(std::ifstream file, Map &map_out);
	void parse_map(const std::vector<std::string> &lines, Map &map_out);

	void test();
} // namespace map_parser