#pragma once

#include <cstdlib>
#include <string>

#include <nng/nng.h>
#include <nng/supplemental/http/http.h>

#include "Open3dStreamUtil.h"

enum WorkState {
  INIT,
  RECV,
  WAIT,
  SEND
};

struct Open3dStreamerWork {
  nng_aio* aio;
  nng_msg* msg;
  nng_ctx ctx;
};

class Open3dStreamerServer {

public:
  Open3dStreamerServer(const std::string& url);

private:
  bool
  allocate(nng_socket sock);

  std::string m_url;
};
