#include "arg_parser.h"

static struct argp_option options[] = {
  { "product-id", 'p', "value", 0                  , "Product ID"},
  { "device-id" , 'd', "value", 0                  , "Device ID"},
  { "secret"    , 's', "value", 0                  , "Device secret"},
  { "daemon"    , 'D', 0      , OPTION_ARG_OPTIONAL, "Make program a daemon"},
  { 0 }
};
struct argp argp = { options, parse_opt, NULL, _ARG_DESC_ };

extern error_t parse_opt(int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;
  switch (key)
  {
    case 'p':
      arguments->productId = arg;
      break;
    case 'd':
      arguments->deviceId = arg;
      break;
    case 's':
      arguments->deviceSecret = arg;
      break;
    case 'D':
      arguments->isDaemon = 1;
      break;
    case ARGP_KEY_END:
      if (strcmp(arguments->productId, "") == 0 || strcmp(arguments->deviceId, "") == 0 || strcmp(arguments->deviceSecret, "") == 0)
      {
        argp_usage(state);
      }
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

extern struct arguments parse_args(int argc, char** argv)
{
  struct arguments args;
  
  args.deviceId = "";
  args.deviceSecret = "";
  args.productId = "";
  args.isDaemon = 0;

  argp_parse(&argp, argc, argv, 0, 0, &args);
  return args;
}