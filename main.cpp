#include <cstdio>

#include <cstring>
#include <cerrno>
#include <unistd.h>

#include "protocol.h"
#include "serial.h"

static ssize_t header_pos(char* buf, size_t len)
{
  bool saw_aa = false;
  for (int i = 0; i < len; i++)
  {
    if (!saw_aa && buf[i] == '\xAA')
    {
      saw_aa = true;
      continue;
    }
    else if (saw_aa)
    {
      if (buf[i] == '\xAA')
        return i - 1;
      else
        saw_aa = false;
    }
  }

  return -1;
}

static void hexdump(char* buf, size_t len)
{
  for (size_t i = 0; i < len; i++)
  {
    printf("%02x ", buf[i]);
  }
  puts("");
}

static void print_motor(const char* label, MotorFeedback* motor)
{
  float speed = motor->speed / 60.f * 165 / 1000.f * 3.14159265359f * 3.6f;
  printf("%-05s: S%8.4f E%02hu H%01u%01u%01u\n", label, speed, (uint16_t)motor->error, motor->hallA, motor->hallB, motor->hallC);
}

static void receive_feedback(int serial_fd)
{
  MotorState state;

  char buf[sizeof(Feedback)];
  size_t off = 0;
  ssize_t retval;
  bool saw_header = false;
  while ((retval = read(serial_fd, buf + off, sizeof(buf) - off)) > 0)
  {
    //printf("Got %zd bytes\n", retval);
    off += retval;

    ssize_t hp;
    if (!saw_header && (hp = header_pos(buf, off)) != -1)
    {
      //printf("Saw header at %zd\n", hp);
      //hexdump(buf, sizeof(buf));
      memmove(buf, buf + hp, off - hp);

      off -= hp;
      saw_header = true;
    }
  }

  if (off != sizeof(Feedback) || !saw_header)
  {
    fprintf(stderr, "Could not find valid header after %zu\n", off);
    return;
  }

  // ugly
  Feedback* feedback = (Feedback*)buf;
  //printf("start: %04x\n", feedback->start);
  print_motor("left", &feedback->left);
  print_motor("right", &feedback->right);
}

static void send_command(int serial_fd, int speed)
{
  Command command;

  command.left.enable = true;
  command.left.ctrlTyp = ControlType::FieldOrientedControl;
  command.left.ctrlMod = ControlMode::Voltage;
  command.left.iMotMax = 2;
  command.left.pwm = speed;

  command.start = Command::VALID_HEADER;
  command.checksum = calculateChecksum(command);

  write(serial_fd, (void*)&command, sizeof(command));
}

int main(int argc, char** argv)
{
  if (argc < 2 || argc > 3)
  {
    fprintf(stderr, "Usage: motor-tester <serial port> [baud rate]\n");
    return 1;
  }

  int baud_rate = 38400;
  if (argc == 3)
  {
    char* endptr = NULL;
    int baud_rate = strtol(argv[2], &endptr, 10);
    if (argv[2][0] == '\0' || *endptr != '\0' || baud_rate_to_b(baud_rate) == -1)
    {
      fprintf(stderr, "Invalid baudrate\n");
      return 1;
    }
  }

  int serial_fd = serial_open(argv[1], baud_rate);
  if (serial_fd < 0)
  {
    char error[256];
    if (strerror_r(errno, error, sizeof(error)) == 0)
    {
      fprintf(stderr, "Failed to open serial port: %s\n", error);
    }

    return 2;
  }

  int speed = 0;
  while (true)
  {
    if (speed < 1000)
    {
      speed += 100;
    }

    receive_feedback(serial_fd);
    send_command(serial_fd, speed);
  }

cleanup:
  close(serial_fd);

  return 0;
}
