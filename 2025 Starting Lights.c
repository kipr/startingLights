#include <kipr/wombat.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define P1 30, 150, 130, 200
#define P2 190, 150, 290, 200
#define P3 350, 150, 450, 200
#define P4 510, 150, 610, 200
#define P5 670, 150, 770, 200
#define PINK 252, 15, 192
#define GREEN 0, 255, 0
#define BLUE 0, 183, 235
#define SERVO_STOPPER_POSITION 2000
#define SERVO_DROPPER_CLOSED 2000
#define SERVO_DROPPER_OPEN 600
#define RUN_TIME 120
#define SENSOR_CHECK_COUNT 10
#define SERVO_A_PORT 0
#define SERVO_B_PORT 3
#define SENSOR_A_PORT 0
#define SENSOR_B_PORT 5
#define SENSOR_TRIGGER_MARGIN 100

void set_all_servos();
void motors_on();
void read_sensor(int readings[], int port);
int sensor_avg(int readings[]);
int calibrate_sensor(int readings[], int port);
bool sensor_triggered(int avg, int calibrated, int port);
void flash(int number);
void rand_color();
void count5();
void select_button();
int run_button();
void checklist();
void run();

int button = -1;

int main()
{
    printf("Starting tournament UI . . .\n");
    msleep(1000);
    enable_servos();
    run();
    return 0;
}

// Sets any plugged in servos
void set_all_servos()
{
    set_servo_position(SERVO_A_PORT, SERVO_DROPPER_CLOSED);
    set_servo_position(SERVO_B_PORT, SERVO_DROPPER_CLOSED);
}

// Turns on any plugged in lights
void motors_on()
{
    motor(0, 100);
    motor(1, 100);
    motor(2, 100);
    motor(3, 100);
}

void read_sensor(int readings[], int port) {
    int i;
    for(i = 0; i < SENSOR_CHECK_COUNT - 1; i++) {
        readings[i] = readings[i + 1];
    }
    readings[SENSOR_CHECK_COUNT - 1] = analog(port);
}

int sensor_avg(int readings[]) {
    int i;
    int sum = 0;
    for(i = 0; i < SENSOR_CHECK_COUNT; i++) {
        sum = sum + readings[i];
    }
    return sum / SENSOR_CHECK_COUNT;
}

int calibrate_sensor(int readings[], int port) {
    int i, avg;
    for(i = 0; i < SENSOR_CHECK_COUNT; i++) {
        readings[i] = analog(port);
        msleep(50);
    }
    avg = sensor_avg(readings);
    if(avg < 0 || avg > 4095) {
        printf("Issue with sensor reading on port %d. Received value of %d.\nFix sensors and restart program.\nPrinting readings from sensor:\n[", port, avg);
        for(i = 0; i < SENSOR_CHECK_COUNT - 1; i++) {
            printf("%d, ", readings[i]);
        }
        printf("%d]\n", readings[SENSOR_CHECK_COUNT - 1]);
        return 0;
    }
    else {
        printf("Calibrated value of %d.\n", avg);
        return avg;
    }
}

bool sensor_triggered(int avg, int calibrated, int port) {
    bool trigger = ((calibrated + SENSOR_TRIGGER_MARGIN) < avg) || ((calibrated - SENSOR_TRIGGER_MARGIN) > avg);
    if(trigger) {
        printf("Sensor in port %d triggered with value %d and calibrated at %d\n", port, avg, calibrated);
    }
    return trigger;
}

void flash(int number)
{
    int x = 0;
    for (x = 0; x < number; x++)
    {
        motors_on();
        msleep(500);
        ao();
        msleep(500);
    }
}

void rand_color()
{
    srand(seconds());
    int rand_num = rand() % 6;
    int order[3];
    switch(rand_num) {
        case 0:
            order[0] = 0;
            order[1] = 1;
            order[2] = 2;
        case 1:
            order[0] = 0;
            order[1] = 2;
            order[2] = 1;
        case 2:
            order[0] = 1;
            order[1] = 0;
            order[2] = 2;
        case 3:
            order[0] = 1;
            order[1] = 2;
            order[2] = 0;
        case 4:
            order[0] = 2;
            order[1] = 0;
            order[2] = 1;
        case 5:
            order[0] = 2;
            order[1] = 1;
            order[2] = 0;
    }
    // Left
    switch(order[0]) {
        case 0:
            graphics_rectangle(P2,PINK);
        case 1:
            graphics_rectangle(P2,GREEN);
        case 2:
            graphics_rectangle(P2,BLUE);
    }
    // Middle
    switch(order[1]) {
        case 0:
            graphics_rectangle(P3,PINK);
        case 1:
            graphics_rectangle(P3,GREEN);
        case 2:
            graphics_rectangle(P3,BLUE);
    }
    // Right
    switch(order[2]) {
        case 0:
            graphics_rectangle(P4,PINK);
        case 1:
            graphics_rectangle(P4,GREEN);
        case 2:
            graphics_rectangle(P4,BLUE);
    }
    printf("\n");
}

void count5()
{
    graphics_print_string("5", 378, 150, 255, 255, 255, 6);
    graphics_update();
    msleep(1000);
    graphics_clear();
    graphics_print_string("4", 378, 150, 255, 255, 255, 6);
    graphics_update();
    msleep(1000);
    graphics_clear();
    graphics_print_string("3", 378, 150, 255, 255, 255, 6);
    graphics_update();
    msleep(1000);
    graphics_clear();
    graphics_print_string("2", 378, 150, 255, 255, 255, 6);
    graphics_update();
    msleep(1000);
    graphics_clear();
    graphics_print_string("1", 378, 150, 255, 255, 255, 6);
    graphics_update();
    msleep(1000);
    graphics_clear();
}

void select_button()
{
    if (push_button())
        button = 0;
    if (a_button())
        button = 1;
    if (b_button())
        button = 2;
    if (c_button())
        printf("Can't use C button\n");
    if (left_button())
        button = 4;
    if (right_button())
        button = 5;
}

int run_button()
{
    select_button();
    switch (button)
    {
        case 0:
            button = -1;
            return push_button();
        case 1:
            button = -1;
            return a_button();
        case 2:
            button = -1;
            return b_button();
        case 3:
            button = -1;
            return c_button();
        case 4:
            button = -1;
            return left_button();
        case 5:
            button = -1;
            return right_button();
        default:
            return 0;
    }
}

void checklist()
{
    printf("Checklist for starting lights:\n");
    printf("- Lights plugged into any motor port.\n");
    printf("- Dropper servos plugged into servo port %d and %d.\n", SERVO_A_PORT, SERVO_B_PORT);
    printf("- Rangefinders plugged into analog ports %d and %d.\n", SENSOR_A_PORT, SENSOR_B_PORT);
    msleep(2000);
    printf("Press grey push button to proceed.\n");
    while (push_button() == 0)
    {
        msleep(10);
    }
}

// Run program on loop
void run()
{
    double start_time, end_time, curr_time, countdown, prev_time;
    // int time115 = 0;
    // int time5 = 0;
    char sec[3];
    int time;
    double decimal;
    int rangefinderA[SENSOR_CHECK_COUNT], rangefinderB[SENSOR_CHECK_COUNT], rangefinder_A_calibration, rangefinder_B_calibration;
    bool triggered_A, triggered_B;
    while (c_button() == 0)
    {
        printf("Press button to set servos\n");
        while (push_button() == 0)
        {
            if (c_button())
                return;
            msleep(10);
        }
        set_all_servos();
        msleep(1000);
        printf("Servos set . . .\n");
        msleep(1000);
        checklist();

        // Press grey button to randomize cups
        printf("Press grey push button to randomize cups\n");
        while (push_button() == 0)
        {
            if (c_button())
                return;
            msleep(10);
        }
        graphics_open(800, 350);
        msleep(500);
        rand_color();
        while (push_button() == 0)
        {
            if (c_button())
                return;
            msleep(10);
        }
        graphics_close();

        printf("Calibrating rangefinder sensors . . .\n");
        rangefinder_A_calibration = calibrate_sensor(rangefinderA, SENSOR_A_PORT);
        rangefinder_B_calibration = calibrate_sensor(rangefinderB, SENSOR_B_PORT);
        if(rangefinder_A_calibration == 0 || rangefinder_B_calibration == 0) {
            msleep(2000);
            break;
        }
        triggered_A = false;
        triggered_B = false;
        printf("Sensors calibrated . . .\n");
        
        // Wait to start
        printf("Press grey push button to Start\n");
        printf("Press c button to stop program\n");
        while (push_button() == 0)
        {
            if (c_button())
                return;
            msleep(10);
        }

        // Countdown
        graphics_open(800, 350);
        msleep(500);
        count5();

        // Start
        start_time = seconds();
        // printf("Start time: %lf\n", start_time);
        end_time = start_time + RUN_TIME;
        curr_time = start_time;
        motors_on();
        graphics_clear();
        graphics_print_string("Timer", 296, 50, 255, 255, 255, 6);
        graphics_print_string("119", 340, 150, 255, 255, 255, 6);
        printf("Press button to Stop\n");
        graphics_update();
        while (curr_time < end_time)
        {
            if (push_button() == 1)
            {
                break;
            }
            if (c_button())
            {
                return;
            }
            countdown = end_time - curr_time;
            if (countdown < RUN_TIME-5 && countdown > 5)
            {
                ao();
            }
            if(!triggered_A) {
                read_sensor(rangefinderA, SENSOR_A_PORT);
                if (sensor_triggered(sensor_avg(rangefinderA), rangefinder_A_calibration, SENSOR_A_PORT))
                {
                    set_servo_position(SERVO_A_PORT, SERVO_DROPPER_OPEN);
                    triggered_A = true;
                }
            }
            if(!triggered_B) {
                read_sensor(rangefinderB, SENSOR_B_PORT);
                if (sensor_triggered(sensor_avg(rangefinderB), rangefinder_B_calibration, SENSOR_B_PORT))
                {
                    set_servo_position(SERVO_B_PORT, SERVO_DROPPER_OPEN);
                    triggered_B = true;
                }
            }

            // Show time left
            time = (int)countdown;
            if (curr_time - prev_time > 1)
            {
                prev_time = curr_time;
                sprintf(sec, "%d", time);
                graphics_clear();
                graphics_print_string("Timer", 296, 50, 255, 255, 255, 6);
                graphics_print_string(sec, 340, 150, 255, 255, 255, 6);
                graphics_print_string("Press button to Stop", 50, 250, 255, 255, 255, 5);
                graphics_update();
            }

            if (countdown <= 5)
            {
                decimal = countdown - (double)time;
                printf("decimal = %lf", decimal);
                if (decimal > 0.5)
                {
                    motors_on();
                }
                else
                {
                    ao();
                }
            }
            curr_time = seconds();
            msleep(10);
        }

        // Stopped
        graphics_close();
        ao();
        msleep(3000);

        // Wait to restart
        printf("Press button to restart\n");
        while (push_button() == 0)
        {
            if (c_button())
                return;
            msleep(10);
        }
        msleep(1000);
        console_clear();
    }
}
