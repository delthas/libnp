#include "np_util.h"

#include <stdlib.h>

void free_array(char **buf, size_t size) {
	for (size_t i = 0; i < size; ++i) {
		free(buf[i]);
	}
	free(buf);
}

void free_info(struct np_info *info) {
	free_array(info->album_artists, info->album_artists_size);
	free_array(info->artists, info->artists_size);
	free_array(info->composers, info->composers_size);
	free_array(info->genres, info->genres_size);
	free_array(info->lyricists, info->lyricists_size);
	free(info->created);
	free(info->first_played);
	free(info->last_played);
	free(info->album);
	free(info->art_url);
	free(info->subtitle);
	free(info->title);
	free(info->url);
	free(info);
}
