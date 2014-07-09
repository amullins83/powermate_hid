#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "powermate_hid.h"

#define INTERACTIVE 0
#define RUN_ONCE    1

void run_command(char command, PowermateHid *this);

int main(int argc, char* argv[])
{
    char command_sequence[128];
    char mode, in_chars, i = 0;
    
    if(argc > 1)
    {
        mode = RUN_ONCE;
        strncpy((char *)command_sequence, argv[1], 127);
        command_sequence[126] = 0;
        strcat(command_sequence, "q");
    }
    else
    {
        mode = INTERACTIVE;
        command_sequence[0] = 0;
    }

    char command = command_sequence[0];

    PowermateHid *powermate = powermate_hid_new();

    if(mode == INTERACTIVE)
    {
        printf("Powermate device opened\n");
    }

    while(command != 'q')
    {
        if(command == 0 && mode == INTERACTIVE)
        {
            printf("%s\n", "Please enter a command (h for help):");
            in_chars = scanf("%127s", command_sequence);
            i = 0;
        }

        command = command_sequence[i++];
        
        run_command(command, powermate);
    }

    powermate_hid_delete(powermate);

    if(mode == INTERACTIVE)
    {
        printf("Powermate device closed\n");
    }

    return 0;
}

typedef void (*Action)(PowermateHid *this);

typedef struct command_map
{
  char key;
  Action      action;
  const char *description;
} CommandMap;

void show_help(PowermateHid *this);
void led_on(PowermateHid *this);
void led_off(PowermateHid *this);
void get_input(PowermateHid *this);
void pulse_on(PowermateHid *this);
void pulse_off(PowermateHid *this);
void pulse_fast(PowermateHid *this);
void pulse_slow(PowermateHid *this);
void led_medium(PowermateHid *this);
void do_nothing(PowermateHid *this) { }

static CommandMap null_command = { 0, NULL, NULL };

static CommandMap commands[] = {
    { 'h', show_help, "Show this help" },
    { 'l', led_on, "Turn LED on"},
    { 'm', led_medium, "Set LED to half power (medium)"},
    { 'L', led_off, "Turn LED off"},
    { 'i', get_input, "Get an input report"},
    { 'p', pulse_on, "Enable pulse mode"},
    { 'P', pulse_off, "Disable pulse mode"},
    { 'f', pulse_fast, "Fast pulse"},
    { 's', pulse_slow, "Slow pulse"},
    { 'q', do_nothing, "Quit"},
    { 0, NULL, NULL }
};

void run_command(char command, PowermateHid *this)
{
    int i;
    for(i = 0; commands[i].key != 0; i++)
    {
        if(command == commands[i].key)
        {
            commands[i].action(this);
            break;
        }
    }
}

void show_help(PowermateHid *this)
{
    int i;
    printf("%s\n", "Commands:");
    for(i = 0; commands[i].key != 0; i++)
    {
        printf("\t%c: %30s\n", commands[i].key, commands[i].description);
    }
    printf("\n");
}

void led_on(PowermateHid *this)
{
    powermate_hid_set_control(this, powermate_control_led_bright);
    powermate_hid_send_output(this);
}

void led_off(PowermateHid *this)
{
    powermate_hid_set_control(this, powermate_control_led_off);
    powermate_hid_send_output(this);
}

void get_input(PowermateHid *this)
{
    PowermateData *input;
    powermate_hid_get_input(this);
    powermate_hid_wait(this);

    input = &this->last_input;

    printf("%s\n", "Powermate Report:");
        printf("\t%s:\t\t%s\n", "Button",
        (input->button_state?
            "PRESSED" : "off"));

        printf("\t%s:\t\t%02x\n", "Knob", input->knob_displacement);
        printf("\t%s:\t\t%02x\n", "LED", input->led_brightness);
        printf("\t%s:\n", "Pulse mode");
            printf("\t\t%s:\t%s\n", "Pulse enabled",
                (input->led_status.bits.pulse_enabled?
                    "ON" : "off"));
            printf("\t\t%s:\t%s\n", "Pulse while asleep",
                (input->led_status.bits.pulse_while_asleep?
                    "ON" : "off"));
            printf("\t\t%s:\t%d\n", "Pulse speed",
                input->led_status.bits.pulse_speed);
            printf("\t\t%s:\t%d\n", "Pulse table",
                input->led_status.bits.pulse_style);
        printf("\t%s:\t\t%d\n", "LED Multiplier", input->led_multiplier);
    printf("\n");
}

void pulse_on(PowermateHid *this)
{
    powermate_hid_set_control(this, powermate_control_pulse_awake_on);
    powermate_hid_send_output(this);
}

void pulse_off(PowermateHid *this)
{
    powermate_hid_set_control(this, powermate_control_pulse_awake_off);
    powermate_hid_send_output(this);
}

void pulse_fast(PowermateHid *this)
{
    powermate_hid_set_control(this, powermate_control_pulse_fast);
    powermate_hid_send_output(this);
}

void pulse_slow(PowermateHid *this)
{
    powermate_hid_set_control(this, powermate_control_pulse_slow);
    powermate_hid_send_output(this);
}

void led_medium(PowermateHid *this)
{
    PowermateControl medium = powermate_control_led_off;
    medium.index = 0x50;
    powermate_hid_set_control(this, medium);
    powermate_hid_send_output(this);
}
