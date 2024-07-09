#include <map>
#include <set>

#define SEND_REPETITIONS 5

union Protocol {
  uint32_t raw;

  struct {
    uint32_t fs2  :4;
    uint32_t fs   :4;
    uint32_t temp :4;
    uint32_t cmd2 :3;
    uint32_t t_on :1;
    uint32_t cmd  :4;
    uint32_t sign :8 = 0x88;
  };
};

using byte = unsigned char;

const uint32_t mask = 0xfffffff;
const uint32_t off_cmd = 0x88c0051;

const std::map<std::string, byte> cmd2 {
  {"cold", 0x0},
  {"dehum", 0x1},
  {"fan", 0x2},
  {"auto", 0x3},
  {"hot", 0x4}
};

// dehumidify and fan mode have fixed temperatures
const std::map<std::string, byte> temp_mode {
  {"dehum", 0x9},
  {"fan", 0x3}
};

const std::map<int, byte> fs1 {
  {0, 0x0},
  {1, 0x9},
  {2, 0x2},
  {3, 0xa},
  {4, 0x4},
  {5, 0x5}
};

const std::map<int, byte> fs2_base {
  {0, 0xb},
  {1, 0x4},
  {2, 0xd},
  {3, 0x5},
  {4, 0xf},
  {5, 0x0}
};

const std::map<std::string, byte> fs2_type {
  {"cold", 0x0},
  {"dehum", 0x7},
  {"fan", 0x2},
  {"auto", 0x3},
  {"hot", 0x2}
};

const std::map<std::string, byte> fs2_base_temp {
  {"hot", 16},
  {"cold", 18},
  {"auto", 18},
  {"dehum", 0},
  {"fan", 0}
};

static Protocol build_command(bool turning_on, std::string mode, int temp, int fan_speed); 
static std::vector<unsigned> convert_to_raw(unsigned raw_code);
static bool sanity_check(std::string mode, int temp, int fan_speed);
static int lirc_send(std::vector<unsigned> buf);
