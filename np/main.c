#include "np.h"

#include <stdio.h>

int main() {
	np_init();

	struct np_info *info = np_info_get();
	if (info) {
		for (size_t i = 0; i < info->album_artists_size; ++i) {
			printf("Album Artist: %s\n", info->album_artists[i]);
		}
		for (size_t i = 0; i < info->artists_size; ++i) {
			printf("Artist: %s\n", info->artists[i]);
		}
		for (size_t i = 0; i < info->composers_size; ++i) {
			printf("Composer: %s\n", info->composers[i]);
		}
		for (size_t i = 0; i < info->genres_size; ++i) {
			printf("Genre: %s\n", info->genres[i]);
		}
		for (size_t i = 0; i < info->lyricists_size; ++i) {
			printf("Lyricist: %s\n", info->lyricists[i]);
		}
		if (info->created) {
			printf("Created: %s\n", info->created);
		}
		if (info->first_played) {
			printf("First Played: %s\n", info->first_played);
		}
		if (info->last_played) {
			printf("Last Played: %s\n", info->last_played);
		}
		if (info->album) {
			printf("Album Title: %s\n", info->album);
		}
		if (info->album_track_count) {
			printf("Album Track Count: %d\n", info->album_track_count);
		}
		if (info->art_url) {
			printf("Art URL: %s\n", info->art_url);
		}
		if (info->bpm) {
			printf("BPM: %d\n", info->bpm);
		}
		if (info->disc_number) {
			printf("Disc Number: %d\n", info->disc_number);
		}
		if (info->length) {
			printf("Length: %llu\n", info->length / 1000000);
		}
		if (info->play_count) {
			printf("Play Count: %d\n", info->play_count);
		}
		if (info->subtitle) {
			printf("Subtitle: %s\n", info->subtitle);
		}
		if (info->title) {
			printf("Title: %s\n", info->title);
		}
		if (info->track_number) {
			printf("Track Number: %d\n", info->track_number);
		}
		if (info->start_time) {
			printf("Start Time: %d\n", info->start_time);
		}
		if (info->end_time) {
			printf("End Time: %d\n", info->end_time);
		}
		if (info->position) {
			printf("Position: %d\n", info->position);
		}
		if (info->url) {
			printf("URL: %s\n", info->url);
		}
		switch (info->playback_type) {
		case np_playback_type_image:
			printf("Type: Image\n");
			break;
		case np_playback_type_music:
			printf("Type: Music\n");
			break;
		case np_playback_type_video:
			printf("Type: Video\n");
			break;
		default:
			break;
		}
		switch (info->playback_status) {
		case np_playbackStatus_type_closed:
			printf("Status: Closed\n");
			break;
		case np_playbackStatus_type_opened:
			printf("Status: Opened\n");
			break;
		case np_playbackStatus_type_changing:
			printf("Status: Changing\n");
			break;
		case np_playbackStatus_type_stopped:
			printf("Status: Stopped\n");
			break;
		case np_playbackStatus_type_playing:
			printf("Status: Playing\n");
			break;
		case np_playbackStatus_type_paused:
			printf("Status: Paused\n");
			break;
		default:
			break;
		}

		np_info_destroy(info);
	}

	np_destroy();

	return 0;
}
