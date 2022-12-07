#pragma once
#include <stdint.h>
#include <time.h>

void np_init();

void np_destroy();

enum np_playback_type {
	np_playback_type_unknown,
	np_playback_type_music,
	np_playback_type_video,
	np_playback_type_image,
};

struct np_info {
	char **album_artists;
	size_t album_artists_size;

	char **artists;
	size_t artists_size;

	char **composers; // linux only
	size_t composers_size; // linux only

	char **genres; // linux only
	size_t genres_size; // linux only

	char **lyricists; // linux only
	size_t lyricists_size; // linux only

	// RFC3339, without milliseconds, optional time offset
	char *created; // linux only
	char *first_played; // linux only
	char *last_played; // linux only

	char *album;
	int album_track_count; // windows only
	char *art_url; // linux only
	int bpm; // linux only
	int disc_number; // linux only
	uint64_t length; // linux only
	int play_count; // linux only
	char *subtitle; // windows only
	char *title;
	int track_number;
	char *url; // linux only

	enum np_playback_type playback_type; // windows only
};

struct np_info *np_info_get();

void np_info_destroy(struct np_info *info);
