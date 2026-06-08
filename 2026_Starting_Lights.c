#include <kipr/wombat.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>

const int RUN_TIME = 120;

const int DROP = 1870;
const int CATCH = 1150;

const double DROP_GAP = 1.000;
const double FIRST_DROP_AT = 9.500;
const double DROP_PERIOD = 7.000;
const int DROP_COUNT = 8;

const double UI_LOOP_PERIOD = 0.005;

volatile sig_atomic_t dropper_should_stop = 0;

double now(void)
{
	return seconds();
}

void sleep_until(double target_time)
{
	while (now() < target_time)
	{
		double remaining = target_time - now();

		if (remaining > 0.020)
		{
			msleep(10);
		}
		else
		{
			msleep(1);
		}
	}
}

void sleep_until_or_stop(double target_time)
{
	while (!dropper_should_stop && now() < target_time)
	{
		double remaining = target_time - now();

		if (remaining > 0.020)
		{
			msleep(10);
		}
		else
		{
			msleep(1);
		}
	}
}

void set_all_servos(int pos)
{
	set_servo_position(0, pos);
	set_servo_position(1, pos);
	set_servo_position(2, pos);
	set_servo_position(3, pos);
}

void motors_on(void)
{
	motor(0, 100);
	motor(1, 100);
	motor(2, 100);
	motor(3, 100);
}

void motors_off(void)
{
	ao();
}

void shuffle_ints(int *array, int length)
{
	for (int i = length - 1; i > 0; i--)
	{
		int j = rand() % (i + 1);

		int temp = array[i];
		array[i] = array[j];
		array[j] = temp;
	}
}

const char *color_name(int color)
{
	switch (color)
	{
		case 0: return "RED";
		case 1: return "GREEN";
		case 2: return "YELLOW";
		default: return "UNKNOWN";
	}
}

void rand_color(void)
{
	static bool seeded = false;

	if (!seeded)
	{
		srand((unsigned int)(seconds() * 1000000.0));
		seeded = true;
	}

	printf("Upper large crate: ");

	if (rand() % 2)
	{
		printf("RED\n");
	}
	else
	{
		printf("GREEN\n");
	}

	int order[3] = {0, 1, 2};
	shuffle_ints(order, 3);

	printf("Small crate stack order: %s %s %s\n\n",
		color_name(order[0]),
		color_name(order[1]),
		color_name(order[2])
	);
}

void draw_centered_number(const char *text)
{
	graphics_clear();
	graphics_print_string(text, 378, 150, 255, 255, 255, 6);
	graphics_update();
}

void draw_timer(int seconds_left)
{
	char sec[8];

	sprintf(sec, "%d", seconds_left);

	graphics_clear();
	graphics_print_string("Timer", 296, 50, 255, 255, 255, 6);
	graphics_print_string(sec, 340, 150, 255, 255, 255, 6);
	graphics_print_string("Press B button to Stop", 25, 250, 255, 255, 255, 5);
	graphics_update();
}

void count5(void)
{
	double start_time = now();

	for (int i = 5; i >= 1; i--)
	{
		char text[8];

		sprintf(text, "%d", i);
		draw_centered_number(text);

		sleep_until(start_time + (6 - i));
	}

	graphics_clear();
	graphics_update();

	sleep_until(start_time + 5.0);
}

void drop_at(double target_time)
{
	sleep_until_or_stop(target_time);

	if (dropper_should_stop)
	{
		set_all_servos(CATCH);
		return;
	}

	set_all_servos(DROP);

	sleep_until_or_stop(target_time + DROP_GAP);

	set_all_servos(CATCH);

	sleep_until_or_stop(target_time + 2.0 * DROP_GAP);
}

void *dropper(void *arg)
{
	double match_start_time = *(double *)arg;

	for (int i = 0; i < DROP_COUNT; i++)
	{
		double drop_start_time = match_start_time + FIRST_DROP_AT + i * DROP_PERIOD;

		drop_at(drop_start_time);

		if (dropper_should_stop)
		{
			break;
		}
	}

	set_all_servos(CATCH);

	return NULL;
}

void wait_b(void)
{
	while (!b_button())
	{
		if (c_button())
		{
			exit(EXIT_SUCCESS);
		}

		sleep_until(now() + 0.010);
	}

	while (b_button())
	{
		sleep_until(now() + 0.010);
	}
}

void checklist(void)
{
	printf("Checklist for starting lights:\n");
	printf("- Lights plugged into any motor port.\n");
	printf("- Dropper servos plugged into any servo ports.\n");
}

void run(void)
{
	pthread_t dropper_thread;

	while (!c_button())
	{
		printf("Press B button to set servos\n");
		wait_b();

		set_all_servos(CATCH);

		printf("Servos set . . .\n");
		printf("Press B button to proceed\n");
		wait_b();

		console_clear();

		checklist();

		printf("Press B button to proceed\n");
		wait_b();

		printf("After teams are calibrated, press B button to get random items\n\n");
		wait_b();

		rand_color();

		printf("Press B button to Start\n");
		printf("Press C button to stop program\n");
		wait_b();

		graphics_open(800, 325);

		count5();

		double start_time = now();
		double end_time = start_time + RUN_TIME;

		double next_display_update = start_time;
		double next_loop_tick = start_time;

		dropper_should_stop = 0;

		motors_on();

		pthread_create(&dropper_thread, NULL, dropper, &start_time);

		printf("Press B button to Stop\n");

		while (now() < end_time)
		{
			double current_time = now();
			double remaining = end_time - current_time;

			if (b_button())
			{
				break;
			}

			if (c_button())
			{
				break;
			}

			if (current_time < start_time + 5.0)
			{
				motors_on();
			}
			else if (remaining > 5.0)
			{
				motors_off();
			}
			else
			{
				int phase = (int)((current_time - (end_time - 5.0)) * 2.0);

				if (phase % 2 == 0)
				{
					motors_on();
				}
				else
				{
					motors_off();
				}
			}

			if (current_time >= next_display_update)
			{
				int seconds_left = (int)(remaining + 0.999);

				if (seconds_left < 0)
				{
					seconds_left = 0;
				}

				draw_timer(seconds_left);

				next_display_update += 1.0;

				while (next_display_update < current_time)
				{
					next_display_update += 1.0;
				}
			}

			next_loop_tick += UI_LOOP_PERIOD;
			sleep_until(next_loop_tick);

			while (next_loop_tick < now())
			{
				next_loop_tick += UI_LOOP_PERIOD;
			}
		}

		printf("Stopping...\n");

		dropper_should_stop = 1;
		pthread_join(dropper_thread, NULL);

		set_all_servos(CATCH);
		motors_off();

		graphics_close();
		console_clear();

		sleep_until(now() + 3.0);

		printf("Press B button to restart or C to exit\n");
		wait_b();

		console_clear();
	}
}

int main(void)
{
	printf("Starting tournament UI . . .\n");

	enable_servos();

	run();

	set_all_servos(CATCH);
	motors_off();
	disable_servos();

	return 0;
}
