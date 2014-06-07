#include <assert.h>
#include <pthread.h>
#include <ncurses.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

static WINDOW *main_win;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int paddle_x, paddle_y;

void *animate_ball(void *);
void *poll_paddle(void *);

int main(int argc, char *argv[]) {
	pthread_t paddle_thread, ball_thread;

	main_win = initscr();

	curs_set(0);
	keypad(main_win, TRUE);
	noecho();
	nodelay(main_win, TRUE);
	cbreak();

	pthread_create(&ball_thread, NULL, animate_ball, NULL);
	pthread_create(&paddle_thread, NULL, poll_paddle, NULL);

	pthread_join(paddle_thread, NULL);

	return EXIT_SUCCESS;
}

void *animate_ball(void *arg) {
	int px = 50, py = 20, vx = -1, vy = 0;

	/* no arg will ever be passed */
	assert(arg == NULL);

	while (true) {
		pthread_mutex_lock(&mutex);
		mvaddch(py, px, 'o');
		pthread_mutex_unlock(&mutex);

		usleep(50000 * sqrt(pow(vx,2) + pow(vy,2)));

		if ((px <= paddle_x && py >= paddle_y && py <= paddle_y+9)) {
			vx *= -1;

			if (py - paddle_y < 5)
				vy = 1;
			else
				vy = -1;
		} else if (px <= 0 || px > 50) {
			vx *= -1;
		} else if (py <= 0 || py > 30) {
			vy *= -1;
		}

		pthread_mutex_lock(&mutex);
		mvaddch(py, px, ' ');
		pthread_mutex_unlock(&mutex);

		py += vy;
		px += vx;
	}

	pthread_exit(NULL);
}

void *poll_paddle(void *arg) {
	int x = 10, y = 10, i;

	/* no arg will ever be passed */
	assert(arg == NULL);

	while (true) {
		paddle_x = x;
		paddle_y = y;

		pthread_mutex_lock(&mutex);
		for (i = 0; i < 10; ++i)
			mvaddch(y+i, x, '|');
		mvaddch(y+10, x, ' ');
		mvaddch(y-1,  x, ' ');
		pthread_mutex_unlock(&mutex);

		int key = getch();

		switch (key) {
		case KEY_UP:
			--y;
			break;
		case KEY_DOWN:
			++y;
			break;
		}
	}

	pthread_exit(NULL);
}
