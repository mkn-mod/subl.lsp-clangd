/**
Copyright (c) 2024, Philip Deegan.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the
distribution.
    * Neither the name of Philip Deegan nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <string_view>
#include <unordered_set>

#include "maiken/module/init.hpp"

namespace mkn::subl::lsp::clang {

void generate_clangd_file_for(maiken::Application const &app,
                              YAML::Node const &node) {
  mkn::kul::File conf{".clangd", app.project().dir()};
  mkn::kul::io::Writer w{conf};
  w.write("#  value: value", true);
  w << R"(
CompileFlags:
  Remove:
)";

  if (node["remove"]) {
    auto const &rem = node["remove"];
    if (rem.IsScalar()) {
      w << "    - " << rem.Scalar() << mkn::kul::os::EOL();
    } else if (rem.IsSequence()) {
      for (auto const &item : rem)
        w << "    - " << item.Scalar() << mkn::kul::os::EOL();
    }
  }

  w << R"(
  Add:
)";

  if (node["add"]) {
    auto const &add = node["add"];
    if (add.IsScalar()) {
      w << "    - " << add.Scalar() << mkn::kul::os::EOL();
    } else if (add.IsSequence()) {
      for (auto const &item : add)
        w << "    - " << item.Scalar() << mkn::kul::os::EOL();
    }
  }

  for (auto const &[inc, vis] : app.includes())
    w << "    - -I" << inc << mkn::kul::os::EOL();
}

class Module : public maiken::Module {
 public:
  void init(maiken::Application &a, YAML::Node const &node)
      KTHROW(std::exception) override {
    generate_clangd_file_for(a, node);
  }
};

}  // namespace mkn::subl::lsp::clang

extern "C" KUL_PUBLISH maiken::Module *maiken_module_construct() {
  return new mkn::subl::lsp::clang::Module;
}

extern "C" KUL_PUBLISH void maiken_module_destruct(maiken::Module *p) {
  delete p;
}
