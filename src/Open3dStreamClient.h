#pragma once

#include <string>

#include <nng/nng.h>
#include <nng/supplemental/http/http.h>

#include "Open3dStreamUtil.h"

class Open3dStreamerClient {

public:
  Open3dStreamerClient(const std::string& url);

private:
  bool
  allocate(nng_socket sock);

  std::string m_url;
};
