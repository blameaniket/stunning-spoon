
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/resource.h>
#endif

#define SECOND 1000000000L



static struct timespec time_diff(struct timespec cur, struct timespec old);
static int indent = 0;


static void print_indent(FILE *file) {
    for (int i = 0; i < indent; i++) {
        fprintf(file, "    ");
    }
}

#ifdef _WIN32

/*
 * Get wall-clock time.
 * Windows FILETIME uses 100-nanosecond intervals since
 * January 1, 1601. Convert it to Unix time.
 */
static struct timespec get_realtime(void) {
    FILETIME ft;
    ULARGE_INTEGER value;

    GetSystemTimeAsFileTime(&ft);
    value.LowPart = ft.dwLowDateTime;
    value.HighPart = ft.dwHighDateTime;

    const uint64_t WINDOWS_TO_UNIX_EPOCH = 116444736000000000ULL;
    uint64_t unix_time = value.QuadPart - WINDOWS_TO_UNIX_EPOCH;

    struct timespec result;
    result.tv_sec = (time_t)(unix_time / 10000000ULL);
    result.tv_nsec = (long)((unix_time % 10000000ULL) * 100);

    return result;
}

/*
 * Get CPU time used by this process.
 */
static struct timespec get_cpu_time(void) {
    FILETIME creation_time;
    FILETIME exit_time;
    FILETIME kernel_time;
    FILETIME user_time;

    if (!GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time,
            &kernel_time, &user_time)) {
        struct timespec zero = { 0, 0 };
        return zero;
    }

    ULARGE_INTEGER kernel;
    ULARGE_INTEGER user;
    kernel.LowPart = kernel_time.dwLowDateTime;
    kernel.HighPart = kernel_time.dwHighDateTime;
    user.LowPart = user_time.dwLowDateTime;
    user.HighPart = user_time.dwHighDateTime;

    uint64_t total_100ns = kernel.QuadPart + user.QuadPart;

    struct timespec result;
    result.tv_sec = (time_t)(total_100ns / 10000000ULL);
    result.tv_nsec = (long)((total_100ns % 10000000ULL) * 100);

    return result;
}

#else

/*
 * Linux/macOS implementation.
 */
static struct timespec get_realtime(void) {
    struct timespec result;

    clock_gettime(CLOCK_REALTIME, &result);

    return result;
}

static struct timespec get_cpu_time(void) {
    struct timespec result;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &result);

    return result;
}

#endif

void log_indent(void) {
    indent++;
}

void log_unindent(void) {
    if (indent > 0) {
        indent--;
    }
}

void log_error(const char *const fmt, ...) {
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "[ERROR]: ");
    vfprintf(stderr, fmt, args);

    va_end(args);
}

void log_warning(const char *const fmt, ...) {
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "[WARNING]: ");
    vfprintf(stderr, fmt, args);

    va_end(args);
}

void log_debug(const char *const fmt, ...) {
#ifndef DEBUG
    (void)fmt;
    return;
#endif
    static struct timespec start_time;
    if (start_time.tv_sec == 0 && start_time.tv_nsec == 0) {
        fprintf(stderr, "[    real,      cpu,   maxRSS]\n");
        start_time = get_realtime();
    }

    struct timespec real_time;
    struct timespec cpu_time;
    real_time = get_realtime();
    cpu_time = get_cpu_time();
    real_time = time_diff(real_time, start_time);

    long max_rss_kb = 0;

#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;

    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        max_rss_kb = (long)(pmc.PeakWorkingSetSize / 1024);
    }

#else
    struct rusage usage;

    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        max_rss_kb = usage.ru_maxrss;
    }
#endif

    va_list args;
    va_start(args, fmt);

    fprintf(stderr,
        "[%ld.%06ld, %ld.%06ld, %5ld KB][DEBUG]: ", (long)real_time.tv_sec,
        (long)(real_time.tv_nsec / 1000), (long)cpu_time.tv_sec,
        (long)(cpu_time.tv_nsec / 1000), max_rss_kb);
    print_indent(stderr);
    vfprintf(stderr, fmt, args);

    va_end(args);
}

void log_info(const char *const fmt, ...) {
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "[INFO]: ");
    print_indent(stderr);
    vfprintf(stderr, fmt, args);

    va_end(args);
}

void log_append_error(const char *const fmt, ...) {
    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);

    va_end(args);
}

void log_append_warning(const char *const fmt, ...) {
    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);

    va_end(args);
}

void log_append_debug(const char *const fmt, ...) {
#ifndef DEBUG
    (void)fmt;
    return;
#endif
    va_list args;
    va_start(args, fmt);

    vprintf(fmt, args);

    va_end(args);
}

void log_append_info(const char *const fmt, ...) {
    va_list args;
    va_start(args, fmt);

    vprintf(fmt, args);

    va_end(args);
}

static struct timespec time_diff(struct timespec cur, struct timespec old) {
    struct timespec diff;
    diff.tv_sec = cur.tv_sec - old.tv_sec;

    if (cur.tv_nsec >= old.tv_nsec) {
        diff.tv_nsec = cur.tv_nsec - old.tv_nsec;
    } else {
        diff.tv_nsec = SECOND + cur.tv_nsec - old.tv_nsec;
        diff.tv_sec--;
    }

    return diff;
}
