#include "raylib.h"
#include <stdio.h>
#include <cstring>

typedef struct ray_video_t {
  //doun't touch this!
  struct ray_video_thread_context_t * ctx;
  double time_in_s_till_next_frame;
  double frame_time_s;
  int video_state;

  //info data
  double worker_time;
  double update_time;
  double upload_time;
  double fps;
  int width;
  int height;
  int mipmaps;
  bool ok;

  Texture texture;
} ray_video_t;

// Keep in mind that ray_video keeps the file handle life and
// will close it on it's own when ray_video_destroy is called.
ray_video_t ray_video_open(const char * path);
ray_video_t ray_video_open_file_handle(FILE * file);
int ray_video_update(ray_video_t *, double deltatime_s);
void ray_video_destroy(ray_video_t *);



// WINAPI
#include <stdint.h>

// Multithreading

#include <stdint.h>
#ifndef _WIN32
#include <pthread.h>
#endif

typedef void* thread_ptr_t;
typedef union thread_atomic_int64_t 
{
  void* align;
  int64_t i;
} thread_atomic_int64_t;

static inline int64_t atomic_cmpxchg( thread_atomic_int64_t* atomic, int64_t expected, int64_t desired ) {
  #if defined( _WIN32 )
    return _InterlockedCompareExchange64( &atomic->i, desired, expected );
  #elif defined( __linux__ ) || defined( __APPLE__ ) || defined( __ANDROID__ )
    return __sync_val_compare_and_swap( &atomic->i, expected, desired );
  #else 
    #error Unknown platform.
  #endif
}

static inline bool atomic_cmpxchg_expect( thread_atomic_int64_t* atomic, int64_t expected, int64_t desired ) {
  return expected == atomic_cmpxchg(atomic, expected, desired);
}

static inline int64_t atomic_get( thread_atomic_int64_t* atomic ) {
    return atomic_cmpxchg(atomic, 0, 0);
}

static inline void atomic_set( thread_atomic_int64_t* atomic, int64_t desired ) {
  #if defined( _WIN32 )
    _InterlockedExchange64( &atomic->i, desired );
  #elif defined( __linux__ ) || defined( __APPLE__ ) || defined( __ANDROID__ )
    __sync_fetch_and_and( &atomic->i, 0 );
    __sync_fetch_and_or( &atomic->i, desired );
  #else 
     #error Unknown platform.
  #endif
}

static inline int64_t atomic_spin_till_expected( thread_atomic_int64_t* atomic, int64_t expected ) {
  int64_t spins = 0;
  while(atomic_get(atomic) != expected) {
    spins++;
  }
  return spins;
}

static inline thread_ptr_t thread_create( int (*thread_proc)( void* ), void* user_data, size_t stack_size ) {
    #if defined( _WIN32 )
    {
        DWORD thread_id;
        HANDLE handle = CreateThread( 
            NULL, stack_size,
            (LPTHREAD_START_ROUTINE)(uintptr_t) thread_proc,
            user_data, 0, &thread_id
        );
        (void)(thread_id);
        if( !handle ) return NULL;
        return (thread_ptr_t) handle;
    }
    #elif defined( __linux__ ) || defined( __APPLE__ ) || defined( __ANDROID__ )
    {
        pthread_t thread;
        if( 0 != pthread_create( &thread, NULL, ( void* (*)( void * ) ) thread_proc, user_data ) )
            return NULL;

        return (thread_ptr_t) thread;
    }
    #else 
    #error Unknown platform.
    #endif
}

// mini perf

#include <stdint.h>

typedef struct performance_counter_t {
  double ticktime_seconds;           // time scaler of the performance counter
  double ticktime_miliseconds;       // time scaler of the performance counter
  double ticktime_nanoseconds;       // time scaler of the performance counter
  int64_t start_time;                // starting time
  int64_t end_time;                  // ending time
} performance_counter_t;

inline void performance_counter_init(performance_counter_t * c) {
  memset(c, 0, sizeof(performance_counter_t));
#ifdef _WIN32
  // get the frequency of the performance counter
  LARGE_INTEGER frequency;
  QueryPerformanceFrequency(&frequency);
  c->ticktime_seconds     = 1.0 / frequency.QuadPart;
  c->ticktime_miliseconds = 1000.0 / frequency.QuadPart;
  c->ticktime_nanoseconds = 1000000.0 / frequency.QuadPart;

  // get the starting time
  LARGE_INTEGER LL;
  QueryPerformanceCounter(&LL);
  c->start_time = LL.QuadPart;
  c->end_time = c->start_time;
#endif  
}

inline void performance_counter_next(performance_counter_t * c) {
  // get the ending time
#ifdef _WIN32
  LARGE_INTEGER LL;
  QueryPerformanceCounter(&LL);
  c->end_time = LL.QuadPart;
#endif  
}

inline double performance_counter_next_seconds(performance_counter_t * c) {
  performance_counter_next(c);
  return (c->end_time - c->start_time) * c->ticktime_seconds;
}

inline double performance_counter_next_miliseconds(performance_counter_t * c) {
  performance_counter_next(c);
  return (c->end_time - c->start_time) * c->ticktime_miliseconds;
}

inline double performance_counter_next_nanoseconds(performance_counter_t * c) {
  performance_counter_next(c);
  return (c->end_time - c->start_time) * c->ticktime_nanoseconds;
}

inline void performance_counter_reset(performance_counter_t * c) {
  performance_counter_next(c);
  c->start_time = c->end_time;
}

// implementation

#define PL_MPEG_IMPLEMENTATION
#include "plmpeg/pl_mpeg.h"
#include <rlgl.h>
#include <stdint.h>

#define THREAD_STATE_IDLE 0
#define THREAD_STATE_WORK 1

enum ray_video_update_status {
  RAY_VIDEO_UPDATE_STATUS_NO_UPDATE = 0,
  RAY_VIDEO_UPDATE_STATUS_NEW_FRAME,
  RAY_VIDEO_UPDATE_STATUS_DONE,
  RAY_VIDEO_UPDATE_STATUS_ERROR
};

enum ray_video_state {
  RAY_VIDEO_STATE_FIRST_FRAME = 0,
  RAY_VIDEO_STATE_BUSY,
  RAY_VIDEO_STATE_READY,
  RAY_VIDEO_STATE_DONE
};

typedef struct ray_video_thread_context_t {
  thread_atomic_int64_t thread_state;
  plm_t   * plm;
  uint8_t * rgb_buffer;

  double thread_worker_time;
  int video_state;
  int stride;
} ray_video_thread_context_t;

static int ray_video_thread_state(ray_video_thread_context_t * ctx) {
  if(atomic_get(&ctx->thread_state) == THREAD_STATE_WORK) return RAY_VIDEO_STATE_BUSY;
  return ctx->video_state;
}

static int ray_video_thread_worker(void * user) {
  ray_video_thread_context_t * ctx = (ray_video_thread_context_t *)(user);

  performance_counter_t perf;
  performance_counter_init(&perf);
  
  int video_state;
  plm_frame_t * frame = plm_decode_video(ctx->plm);

  if(!frame) {
    //TODO: configure video to automatically replay in a loop.
    video_state = RAY_VIDEO_STATE_DONE;
  }
  else {
    plm_frame_to_rgb(frame, ctx->rgb_buffer, ctx->stride);
    video_state = RAY_VIDEO_STATE_READY;
  }
  
  ctx->thread_worker_time = performance_counter_next_seconds(&perf);
  ctx->video_state = video_state;
  bool ok = atomic_cmpxchg_expect(&ctx->thread_state, THREAD_STATE_WORK, THREAD_STATE_IDLE);
  return ok ? 0 : 1;
}

static void start_new_fetch_worker(ray_video_thread_context_t * ctx) {
  atomic_set(&ctx->thread_state, THREAD_STATE_WORK);
  thread_create(ray_video_thread_worker, ctx, 0);
}

static int update_video(ray_video_t * video, double deltatime_s) {
  switch(video->video_state) {
    break; case RAY_VIDEO_STATE_FIRST_FRAME:
      video->time_in_s_till_next_frame = video->frame_time_s;
      video->video_state = RAY_VIDEO_STATE_READY;
    break; case RAY_VIDEO_STATE_READY:
      video->time_in_s_till_next_frame -= deltatime_s;
      if(video->time_in_s_till_next_frame > 0.0) {
        return RAY_VIDEO_UPDATE_STATUS_NO_UPDATE;
      }
      else {
        video->time_in_s_till_next_frame += video->frame_time_s;
        if(video->time_in_s_till_next_frame < 0) {
          //video->time_in_s_till_next_frame = 0;
        }
      }
    break; case RAY_VIDEO_STATE_DONE:
      return RAY_VIDEO_UPDATE_STATUS_DONE;
    break; case RAY_VIDEO_STATE_BUSY:
    //Don't change the delta time before we get the next frame.
    break;
  }

  video->video_state = ray_video_thread_state(video->ctx);
  if(video->video_state == RAY_VIDEO_STATE_BUSY) {
    //Thread is still busy. Skip this frame and hope we get it next time.
    //TODO: add option to wait for frame.
    return RAY_VIDEO_UPDATE_STATUS_NO_UPDATE;
  }

  if(video->video_state == RAY_VIDEO_STATE_READY) {
    return RAY_VIDEO_UPDATE_STATUS_NEW_FRAME;
  }
  return RAY_VIDEO_UPDATE_STATUS_DONE;
}

ray_video_t ray_video_create_by_file_handle(FILE * file) {
  ray_video_t video;
  memset(&video, 0, sizeof(ray_video_t));
  video.ok = false;

  if(file) {
    plm_t * plm = plm_create_with_file(file, 0);
    if(!plm) return video;

    plm_set_audio_enabled(plm, 0);
    video.mipmaps = 1; //Does video even need mipmaps? UpdateTexture ignores the mipmaps anyway.
    video.width  = plm_get_width(plm);
    video.height = plm_get_height(plm);
    video.fps    = plm_get_framerate(plm);
    video.frame_time_s = 1.0 / video.fps;

    video.ctx = (ray_video_thread_context_t *)calloc(1, sizeof(ray_video_thread_context_t));
    video.ctx->plm = plm;
    video.ctx->rgb_buffer = (uint8_t *)malloc(3 * video.width * video.height);
    video.ctx->stride = 3 * video.width;
    start_new_fetch_worker(video.ctx);
    video.ok = true;
  }

  return video;
}

ray_video_t ray_video_open(const char * path) {
  return ray_video_create_by_file_handle(fopen(path, "rb"));
}

int ray_video_update(ray_video_t * video, double deltatime_s) {
  int state;
  performance_counter_t perf;

  if(!video->ok) return RAY_VIDEO_UPDATE_STATUS_ERROR;

  performance_counter_init(&perf);
  state = update_video(video, deltatime_s);
  video->update_time = performance_counter_next_seconds(&perf);
  if(state == RAY_VIDEO_UPDATE_STATUS_NEW_FRAME) {
    performance_counter_reset(&perf);
    if(video->texture.id == 0) {
      int format = RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8;

      video->texture.id = rlLoadTexture(video->ctx->rgb_buffer, video->width, video->height, format, video->mipmaps);
      video->texture.width = video->width;
      video->texture.height = video->height;
      video->texture.mipmaps = video->mipmaps;
      video->texture.format = format;
    }
    else {
      UpdateTexture(video->texture, video->ctx->rgb_buffer);
    }
    video->worker_time = video->ctx->thread_worker_time;
    video->upload_time = performance_counter_next_seconds(&perf);
    
    start_new_fetch_worker(video->ctx);
  }
  return state;
}

void ray_video_destroy(ray_video_t * video) {
  if(!video) return;

  if(video->ctx) {
    atomic_spin_till_expected(&video->ctx->thread_state, THREAD_STATE_IDLE);

    if(video->ctx->plm) plm_destroy(video->ctx->plm);
    if(video->ctx->rgb_buffer) free(video->ctx->rgb_buffer);
    free(video->ctx);
  }
  memset(video, 0, sizeof(ray_video_t));
}