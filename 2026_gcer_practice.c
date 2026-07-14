#include <kipr/wombat.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>
#include <limits.h>

const int RUN_TIME = 120;

const int DROP = 1870;
const int CATCH = 1150;

const double DROP_GAP = 1.000;
const double FIRST_DROP_AT = 9.500;
const double DROP_PERIOD = 7.000;
const int DROP_COUNT = 8;

const double UI_LOOP_PERIOD = 0.005;

volatile sig_atomic_t dropper_should_stop = 0;
volatile sig_atomic_t a_running = 0;
volatile sig_atomic_t b_running = 0;


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

void drop_at(double target_time, int port)
{
	sleep_until_or_stop(target_time);

	if (dropper_should_stop)
	{
		set_servo_position(port, CATCH);
		return;
	}

	set_servo_position(port, DROP);

	sleep_until_or_stop(target_time + DROP_GAP);

	set_servo_position(port, CATCH);

	sleep_until_or_stop(target_time + 2.0 * DROP_GAP);
}

void *dropper_a(void *arg)
{
	a_running = 1;
	printf("Dropping side A\n");
	double match_start_time = now();

	for (int i = 0; i < DROP_COUNT; i++)
	{
		double drop_start_time = match_start_time + FIRST_DROP_AT + i * DROP_PERIOD;

		drop_at(drop_start_time, 0);

		if (dropper_should_stop)
		{
			break;
		}
	}

	set_all_servos(CATCH);

	printf("A finished\n");
	a_running = 0;
	return NULL;
}

void *dropper_b(void *arg)
{
	b_running = 1;
	printf("Dropping side B\n");
	double match_start_time = now();

	for (int i = 0; i < DROP_COUNT; i++)
	{
		double drop_start_time = match_start_time + FIRST_DROP_AT + i * DROP_PERIOD;

		drop_at(drop_start_time, 3);

		if (dropper_should_stop)
		{
			break;
		}
	}

	set_all_servos(CATCH);
	printf("B finished\n");

	b_running = 0;
	return NULL;
}

int main(void)
{
	printf("Starting practice UI . . .\n");
	pthread_t dropper_a_thread;
	pthread_t dropper_b_thread;

	unsigned long a_lights = ULONG_MAX, b_lights = ULONG_MAX;
	enable_servos();
	while(1) {
		if(now() > a_lights) {
			off(3);
			a_lights = ULONG_MAX;
		}
		if(now() > b_lights) {
			off(0);
			b_lights = ULONG_MAX;
		}
		if(c_button()) {
			printf("Aborting all droppers\n");
			dropper_should_stop = 1;
			a_lights = ULONG_MAX;
			b_lights = ULONG_MAX;
			while(c_button());
            msleep(500);
			dropper_should_stop = 0;
		}
   		if(!a_running && a_button()) {
			pthread_create(&dropper_a_thread, NULL, dropper_a, NULL);
			motor(3, 100);
			a_lights = now() + 5.0;
			while(a_button());
		}
   		if(!b_running && b_button()) {
			pthread_create(&dropper_b_thread, NULL, dropper_b, NULL);
			motor(0, 100);
			b_lights = now() + 5.0;
			while(b_button());
		}
	}

	disable_servos();

	return 0;
}

