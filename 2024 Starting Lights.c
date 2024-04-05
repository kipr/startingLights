#include <kipr/wombat.h>
// #include <kipr/graphics/graphics_characters.h>
#include <stdlib.h>
#include <stdio.h>

#define P1 30, 150, 130, 200
#define P2 190, 150, 290, 200
#define P3 350, 150, 450, 200
#define P4 510, 150, 610, 200
#define P5 670, 150, 770, 200
#define SERVO_STOPPER_POSITION 2000
#define SERVO_DROPPER_CLOSED 2000
#define SERVO_DROPPER_OPEN 100
#define SERVO_FLAG_DOWN 2000
#define SERVO_FLAG_UP 1000

void set_all_servos();
void motors_on();
void flash(int number);
int rand_color();
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
    set_servo_position(0, SERVO_DROPPER_CLOSED);
    set_servo_position(1, SERVO_DROPPER_CLOSED);
    set_servo_position(2, SERVO_FLAG_DOWN);
    set_servo_position(3, SERVO_FLAG_DOWN);
}

// Turns on any plugged in lights
void motors_on()
{
    motor(0, 100);
    motor(1, 100);
    motor(2, 100);
    motor(3, 100);
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

int rand_color()
{
    srand(seconds());
    int rand_num = rand() % 2;
    int i;
    printf("Proof that numbers are random: ");
    for (i = 0; i < 10; i++)
    {
        rand_num = rand() % 2;
        printf("%d ", rand_num);
    }
    printf("\n");
    return rand_num;
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
    printf("- Dropper servos plugged into servo port 1 and 3.\n");
    printf("- Light switch plugged into digital port 2 and 6.\n");
    printf("- Dropper light switch off.\n");
    printf("- Flag servo plugged into servo port 0 and 2.\n");
    printf("- Flag station touch sensor plugged into digital port 0 and 4.\n");
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
    int decimal;
    while (c_button() == 0)
    {
        printf("Press button to set servos\n");
        while (push_button() == 0)
        {
            if (c_button())
                return;
            msleep(10);
        }
        set_all_servos(SERVO_STOPPER_POSITION);
        msleep(1000);
        printf("Servos set . . .\n");
        msleep(1000);

        checklist();
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
        end_time = start_time + 120;
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
            if (countdown < 115 && countdown > 5)
            {
                ao();
            }
            if (digital(2) == 1)
            {
                set_servo_position(1, SERVO_DROPPER_OPEN);
            }
            if (digital(6) == 1)
            {
                set_servo_position(3, SERVO_DROPPER_OPEN);
            }
            // Check if flag buttons are pressed
            if (digital(0) == 1)
            {
                set_servo_position(0, SERVO_FLAG_UP);
                printf("Flag sensor in port 0 pressed\n");
            }
            if (digital(4) == 1)
            {
                set_servo_position(2, SERVO_FLAG_UP);
                printf("Flag sensor in port 4 pressed\n");
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
                if (countdown <= 5)
                {
                    decimal = countdown - time;
                    if (decimal > 0.5)
                    {
                        motors_on();
                    }
                    else
                    {
                        ao();
                    }
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