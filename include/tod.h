#define PTICKS_PER_CLOCK	1500

/* game time */
#define HOURS_PER_DAY		28
#define DAYS_PER_WEEK		7
#define WEEKS_PER_MONTH		5
#define MONTHS_PER_YEAR		17

/* convenience */
#define WEEKS_PER_YEAR		(WEEKS_PER_MONTH*MONTHS_PER_YEAR)
#define DAYS_PER_MONTH		(DAYS_PER_WEEK*WEEKS_PER_MONTH)
#define DAYS_PER_YEAR		(DAYS_PER_MONTH*MONTHS_PER_YEAR)
#define HOURS_PER_WEEK		(HOURS_PER_DAY*DAYS_PER_WEEK)
#define HOURS_PER_MONTH		(HOURS_PER_WEEK*WEEKS_PER_MONTH)
#define HOURS_PER_YEAR		(HOURS_PER_MONTH*MONTHS_PER_YEAR)

#define LUNAR_DAYS		DAYS_PER_MONTH

typedef struct _timeofday {
	int year;
	int month;
	int day;
	int dayofweek;
	int hour;
	int minute;
	int weekofmonth;
	int season;
} timeofday_t;

/* from common/time.c */
extern void get_tod(timeofday_t *tod);
