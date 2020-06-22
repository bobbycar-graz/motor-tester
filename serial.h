#include <string>
#include <cerrno>

#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace {
  int baud_rate_to_b(int baud_rate)
  {
    switch (baud_rate)
    {
      //case 0:
      //  return B0;
      case 50:
        return B50;
      case 75:
        return B75;
      case 110:
        return B110;
      case 134:
        return B134;
      case 150:
        return B150;
      case 200:
        return B200;
      case 300:
        return B300;
      case 600:
        return B600;
      case 1200:
        return B1200;
      case 1800:
        return B1800;
      case 2400:
        return B2400;
      case 4800:
        return B4800;
      case 9600:
        return B9600;
      case 19200:
        return B19200;
      case 38400:
        return B38400;
      case 57600:
        return B57600;
      case 115200:
        return B115200;
    }

    return -1;
  }

  int serial_open(const std::string& path, int baud_rate)
  {
    int baud_setting = baud_rate_to_b(baud_rate);
    if (baud_setting == -1)
    {
      errno = EINVAL;
      return -1;
    }

    int serial_fd = open(path.c_str(), O_RDWR);

    struct termios options;
    tcgetattr(serial_fd, &options);
    cfmakeraw(&options);
    cfsetispeed(&options, baud_setting);
    cfsetospeed(&options, baud_setting);
    options.c_cflag |= CLOCAL;
    tcsetattr(serial_fd, TCSANOW, &options);

    return serial_fd;
  }
}
