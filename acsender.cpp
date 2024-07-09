#include <boost/program_options.hpp>
#include <iostream>
#include <linux/lirc.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sysexits.h>
#include <errno.h>
#include <string.h>
#include "acsender.h"

namespace po = boost::program_options;

using namespace std;

// build AC binary command 
static Protocol build_command(bool turning_on, string mode, int temp, int fan_speed) {
  Protocol pc;
  pc.cmd = 0x0;
  pc.t_on = !turning_on;
  pc.cmd2 = cmd2.at(mode);
  pc.temp = (mode == "dehum" || mode == "fan") ? temp_mode.at(mode) : temp - 15;
  pc.fs = fan_speed;

  int T; 
  if (mode == "dehum" || mode == "fan")
    T = 0;
  else
    T = temp - fs2_base_temp.at(mode);
  pc.fs2 = fs2_base.at(fan_speed) + fs2_type.at(mode) + T + (turning_on ? 0x8 : 0x0);
  return pc;
}

static std::vector<unsigned> convert_to_raw(unsigned raw_code) {
  // raw IR starts with a pulse/space pair lasting ~3.3 ms / ~9.9 ms
  // then, code is followed by seven bit segments. 
  // each segment must be converted to a pair (pulse/space).
  // Therefore, buffer size is 2 + (4 * 7) * 2 = 58
  std::vector<unsigned> buf(59);

  buf[0] = 3300;
  buf[1] = 9900;

  // 7 * 4 bits
  for (int i = 0 ; i < 28; ++i) {
    bool bit = raw_code & (1 << (27 - i));

    buf[2 + 2 * i] = 500;
    buf[2 + 2 * i + 1]  = bit ? 1500 : 500;
  }

  // end with a pulse
  buf[58] = 500;
  return buf;
}

static bool sanity_check(std::string mode, int temp, int fan_speed) {
  if (cmd2.find(mode) == cmd2.end()) {
    fprintf(stderr, "mode %s is invalid\n", mode.c_str());
    return 1;
  }
  if (temp > 30 || temp < fs2_base_temp.at(mode)) {
    fprintf(stderr, "Temperature %d out of range for mode %s\n", temp, mode.c_str());
    return 1;
  }

  if (fan_speed < 0 || fan_speed > 5) {
    fprintf(stderr, "fan speed invalid!\n");
    return 1;
  }
  return 0;
}

static int lirc_send(vector<unsigned> buf) {
  int fd = open("/dev/lirc0", O_RDWR | O_CLOEXEC);
  if (fd == -1) {
    fprintf(stderr, "Cannot open /dev/lirc0\n");
    return -1;
  }

  struct stat st;
  auto rc = fstat(fd, &st);
  if (rc) {
    fprintf(stderr, "Cannot stat file descriptor!\n");
    close(fd);
    return -1;
  }

  if ((st.st_mode & S_IFMT) != S_IFCHR) {
    fprintf(stderr, "/dev/lirc0 is not a character device!\n");
    close(fd);
    return -1;
  }

  unsigned features;
  rc = ioctl(fd, LIRC_GET_FEATURES, &features);

  if (rc) {
    fprintf(stderr, "failed to get lirc features!\n");
    close(fd);
    return -1;
  }

  if (!(features & LIRC_CAN_SEND_PULSE)) {
    fprintf(stderr, "device cannot send!\n");
    close(fd);
    return -1;
  }

  auto mode = LIRC_MODE_PULSE;
  rc = ioctl(fd, LIRC_SET_SEND_MODE, &mode);
  if (rc) {
    fprintf(stderr, "Cannot set send mode on device\n");
    close(fd);
    return EX_UNAVAILABLE;
  }

  uint32_t freq = 38000;
  rc = ioctl(fd, LIRC_SET_SEND_CARRIER, &freq);
  if (rc) {
    fprintf(stderr, "Cannot set frequency on device\n");
    close(fd);
    return EX_UNAVAILABLE;
  }

  auto ret = write(fd, buf.data(), buf.size() * sizeof(unsigned));
  if (ret < 0) {
    fprintf(stderr, "Failed to send data! error: %s\n", strerror(errno));
    close(fd);
    return EX_IOERR;
  }

  if (buf.size() *sizeof(unsigned) < ret) {
    fprintf(stderr, "warning: sent %u elements. %u expected\n", buf.size(), ret / sizeof(unsigned));
  }

  close(fd);
  return 0;
}

int main(int argc, char * argv[]) {
  try {
    po::options_description desc("Allowed options");
    desc.add_options() ("help", "produce help message")
      ("mode", po::value<string>(), "A/C Mode:\n"
                                            "\tcold\n"
                                            "\thot\n"
                                            "\tdehum\n"
                                            "\tfan\n"
                                            "\tauto")
      ("temp", po::value<int>(), "A/C temperature")
      ("fan-speed", po::value<string>(), "Fan speed\n" 
       "\t[0-4] - lowest to highest speed\n"
       "\tauto  - automatic speed")
      ("on", "Turn on the A/C")
      ("off", "Turn off the A/C");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    bool turning_on = false;

    if (vm.count("help")) {
      cout << desc;
      return 1;
    }

    Protocol pc;
    if (vm.count("off"))
      pc.raw = off_cmd;
    else {
      if (vm.count("on"))
        turning_on = true;

      string fan_speed_str = vm.count("fan-speed") ? vm["fan-speed"].as<string>() : "auto";
      int fan_speed;
      if (fan_speed_str == "auto")
        fan_speed = 5;
      else
        fan_speed = stoi(fan_speed_str);

      string mode = vm.count("mode") ? vm["mode"].as<string>() : string("cold");
      int temp = vm.count("temp") ? vm["temp"].as<int>() : 21;

      if (sanity_check(mode, temp, fan_speed))
        return 1;

      pc = build_command(turning_on, mode, temp, fan_speed);
    }
    // don't forget the mask, although we really don't check the 4 MSBs
    auto buf = convert_to_raw(pc.raw & mask);
    if (lirc_send(buf))
      return 1;
  }
  catch(boost::wrapexcept<boost::program_options::unknown_option> &e) {
    cerr << "error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
