#include "np.h"
#include "np_util.h"

#include <roapi.h>
#include <windows.media.control.h>

UUID manager_async_iid = {.Data1 = 0x10F0074E, .Data2 = 0x923D, .Data3 = 0x5510, .Data4 = {0x8F, 0x4A, 0xDD, 0xE3, 0x77, 0x54, 0xCA, 0x0E}};
UUID info_async_iid = {.Data1 = 0x84593A3D, .Data2 = 0x951A, .Data3 = 0x55B6, .Data4 = {0x83, 0x53, 0x52, 0x05, 0xE5, 0x77, 0x79, 0x7B}};
UUID unknown_iid = {.Data1 = 0x00000000, .Data2 = 0x0000, .Data3 = 0x0000, .Data4 = {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

struct async_handler {
	struct async_handler_vtable *vtable;
	UUID *iid;
	HANDLE event;
	int count;
};

struct async_handler_vtable {
	void *QueryInterface;
	void *AddRef;
	void *Release;
	void *Invoke;
};

ULONG __stdcall async_handler_AddRef(struct async_handler *this) {
	return ++this->count;
}

ULONG __stdcall async_handler_Release(struct async_handler *this) {
	return --this->count;
}

HRESULT __stdcall async_handler_QueryInterface(struct async_handler *this, const IID *riid, void **ptr) {
	if (!memcmp(riid, this->iid, sizeof(GUID)) || !memcmp(riid, &unknown_iid, sizeof(GUID))) {
		*ptr = this;
		async_handler_AddRef((void *)this);
		return NOERROR;
	}
	return E_NOINTERFACE;
}

HRESULT __stdcall async_handler_Invoke(struct async_handler *this, void *async_info, AsyncStatus status) {
	SetEvent(this->event);
	return NOERROR;
}

struct async_handler_vtable async_handler_vtable = {
	.QueryInterface = async_handler_QueryInterface,
	.AddRef = async_handler_AddRef,
	.Release = async_handler_Release,
	.Invoke = async_handler_Invoke,
};

void np_init() {
	RoInitialize(RO_INIT_MULTITHREADED);
}

void np_destroy() {
	RoUninitialize();
}

const wchar_t manager_static_class[] = L"Windows.Media.Control.GlobalSystemMediaTransportControlsSessionManager";

const UUID manager_static_iid = {.Data1 = 0x2050C4EE, .Data2 = 0x11A0, .Data3 = 0x57DE, .Data4 = {0xAE, 0xD7, 0xC9, 0x7C, 0x70, 0x33, 0x82, 0x45}};

char *move_hstring_to_utf8(HSTRING str) {
	if (!str) {
		return NULL;
	}
	const wchar_t *src = WindowsGetStringRawBuffer(str, NULL);
	if (!src) {
		return NULL;
	}
	int src_len = wcslen(src);
	int dst_len = WideCharToMultiByte(CP_UTF8, 0, src, src_len, 0, 0, NULL, NULL);
	char *dst = (char *)malloc(dst_len + 1);
	WideCharToMultiByte(CP_UTF8, 0, src, src_len, dst, dst_len, NULL, NULL);
	dst[dst_len] = '\0';
	WindowsDeleteString(str);
	return dst;
}

struct np_info *np_info_get() {
	struct np_info *r = NULL;

	HSTRING_HEADER manager_static_instance_type_header;
	HSTRING manager_static_instance_type;
	if (FAILED(WindowsCreateStringReference(
				manager_static_class, sizeof(manager_static_class) / 2 - 1, &manager_static_instance_type_header, &manager_static_instance_type))
		|| !manager_static_instance_type) {
		goto exit0;
	}
	__x_ABI_CWindows_CMedia_CControl_CIGlobalSystemMediaTransportControlsSessionManagerStatics *manager_static;
	if (FAILED(RoGetActivationFactory(manager_static_instance_type, &manager_static_iid, (void **)&manager_static)) || !manager_static) {
		goto exit0;
	}

	__FIAsyncOperation_1_Windows__CMedia__CControl__CGlobalSystemMediaTransportControlsSessionManager *manager_async_request;
	if (FAILED(manager_static->lpVtbl->RequestAsync(manager_static, &manager_async_request) || !manager_async_request)) {
		goto exit1;
	}

	struct async_handler manager_async = {
		.vtable = &async_handler_vtable,
		.iid = &manager_async_iid,
		.event = CreateEvent(NULL, 1, 0, NULL),
	};
	if (FAILED(manager_async_request->lpVtbl->put_Completed(manager_async_request, (void *)&manager_async))) {
		goto exit2;
	}
	if (WaitForSingleObject(manager_async.event, INFINITE) != WAIT_OBJECT_0) {
		goto exit2;
	}

	__x_ABI_CWindows_CMedia_CControl_CIGlobalSystemMediaTransportControlsSessionManager *manager;
	if (FAILED(manager_async_request->lpVtbl->GetResults(manager_async_request, &manager)) || !manager) {
		goto exit2;
	}

	__x_ABI_CWindows_CMedia_CControl_CIGlobalSystemMediaTransportControlsSession *session;
	if (FAILED(manager->lpVtbl->GetCurrentSession(manager, &session)) || !session) {
		goto exit3;
	}

	__FIAsyncOperation_1_Windows__CMedia__CControl__CGlobalSystemMediaTransportControlsSessionMediaProperties *info_async_request;
	if (FAILED(session->lpVtbl->TryGetMediaPropertiesAsync(session, &info_async_request)) || !info_async_request) {
		goto exit4;
	}

	struct async_handler info_async = {
		.vtable = &async_handler_vtable,
		.iid = &info_async_iid,
		.event = CreateEvent(NULL, 1, 0, NULL),
	};
	if (FAILED(info_async_request->lpVtbl->put_Completed(info_async_request, (void *)&info_async))) {
		goto exit5;
	}
	if (WaitForSingleObject(info_async.event, INFINITE) != WAIT_OBJECT_0) {
		goto exit5;
	}

	__x_ABI_CWindows_CMedia_CControl_CIGlobalSystemMediaTransportControlsSessionMediaProperties *info;
	if (FAILED(info_async_request->lpVtbl->GetResults(info_async_request, &info)) || !info) {
		goto exit5;
	}

	r = calloc(1, sizeof(struct np_info));

	HSTRING album_artist;
	if (SUCCEEDED(info->lpVtbl->get_AlbumArtist(info, &album_artist)) && album_artist) {
		r->album_artists = calloc(1, sizeof(*r->album_artists));
		r->album_artists[0] = move_hstring_to_utf8(album_artist);
		r->album_artists_size = 1;
	}
	HSTRING album_title;
	if (SUCCEEDED(info->lpVtbl->get_AlbumTitle(info, &album_title)) && album_title) {
		r->album = move_hstring_to_utf8(album_title);
	}
	INT32 album_track_count;
	if (SUCCEEDED(info->lpVtbl->get_AlbumTrackCount(info, &album_track_count))) {
		r->album_track_count = album_track_count;
	}
	HSTRING artist;
	if (SUCCEEDED(info->lpVtbl->get_Artist(info, &artist)) && artist) {
		r->artists = calloc(1, sizeof(*r->artists));
		r->artists[0] = move_hstring_to_utf8(artist);
		r->artists_size = 1;
	}
	__FIVectorView_1_HSTRING *genres;
	if (SUCCEEDED(info->lpVtbl->get_Genres(info, &genres)) && genres) {
		unsigned int len;
		if (SUCCEEDED(genres->lpVtbl->get_Size(genres, &len)) && len) {
			r->genres = (char **)calloc(sizeof(char *), len);
			int j = 0;
			for (int i = 0; i < len; ++i) {
				HSTRING genre;
				if (SUCCEEDED(genres->lpVtbl->GetAt(genres, i, &genre)) && genre) {
					r->genres[j++] = move_hstring_to_utf8(genre);
				}
			}
			r->genres_size = j;
		}
		genres->lpVtbl->Release(genres);
	}
	__FIReference_1_Windows__CMedia__CMediaPlaybackType *playback_type_ref;
	if (SUCCEEDED(info->lpVtbl->get_PlaybackType(info, &playback_type_ref)) && playback_type_ref) {
		__x_ABI_CWindows_CMedia_CMediaPlaybackType playback_type;
		if (SUCCEEDED(playback_type_ref->lpVtbl->get_Value(playback_type_ref, &playback_type))) {
			switch (playback_type) {
			case MediaPlaybackType_Music:
				r->playback_type = np_playback_type_music;
				break;
			case MediaPlaybackType_Video:
				r->playback_type = np_playback_type_video;
				break;
			case MediaPlaybackType_Image:
				r->playback_type = np_playback_type_image;
				break;
			default:
				r->playback_type = np_playback_type_unknown;
				break;
			}
		}
	}
	HSTRING subtitle;
	if (SUCCEEDED(info->lpVtbl->get_Subtitle(info, &subtitle)) && subtitle) {
		r->subtitle = move_hstring_to_utf8(subtitle);
	}
	// TODO:
	//   __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStreamReference *thumbnail;
	//	 info->lpVtbl->get_Thumbnail(info, &thumbnail);
	HSTRING title;
	if (SUCCEEDED(info->lpVtbl->get_Title(info, &title)) && title) {
		r->title = move_hstring_to_utf8(title);
	}
	INT32 track_number;
	if (SUCCEEDED(info->lpVtbl->get_TrackNumber(info, &track_number))) {
		r->track_number = track_number;
	}

exit6:;
	info->lpVtbl->Release(info);
exit5:;
	CloseHandle(info_async.event);
exit4:;
	session->lpVtbl->Release(session);
exit3:;
	manager->lpVtbl->Release(manager);
exit2:;
	CloseHandle(manager_async.event);
	manager_async_request->lpVtbl->Release(manager_async_request);
exit1:;
	manager_static->lpVtbl->Release(manager_static);
exit0:;

	return r;
}

void np_info_destroy(struct np_info *info) {
	free_info(info);
}
